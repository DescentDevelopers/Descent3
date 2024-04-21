/*
 * Utility functions for libacm.
 *
 * Copyright (c) 2004-2010, Marko Kreen
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "libacm.h"

#define WAVC_HEADER_LEN	28
#define ACM_HEADER_LEN	14

/*
 * error strings
 */
static const char *_errlist[] = {
	"No error",
	"ACM error",
	"Cannot open file",
	"Not an ACM file",
	"Read error",
	"Bad format",
	"Corrupt file",
	"Unexcpected EOF",
	"Stream not seekable"
};

const char *acm_strerror(int err)
{
	int nerr = sizeof(_errlist) / sizeof(char *);
	if ((-err) < 0 || (-err) >= nerr)
		return "Unknown error";
	return _errlist[-err];
}

/*
 * File IO using stdio
 */

static int _read_file(void *ptr, int size, int n, void *arg)
{
	FILE *f = (FILE *)arg;
	return fread(ptr, size, n, f);
}
                                                                                
static int _close_file(void *arg)
{
	FILE *f = (FILE *)arg;
	return fclose(f);
}
                                                                                
static int _seek_file(void *arg, int offset, int whence)
{
	FILE *f = (FILE *)arg;
	return fseek(f, offset, whence);
}

static int _get_length_file(void *arg)
{
	FILE *f = (FILE *)arg;
	int res, pos, len = -1;

	pos = ftell(f);
	if (pos < 0)
		return -1;

	res = fseek(f, 0, SEEK_END);
	if (res >= 0) {
		len = ftell(f);
		fseek(f, pos, SEEK_SET);
	}
	return len;
}

int acm_open_file(ACMStream **res, const char *filename, int force_chans)
{
	int err;
	FILE *f;
	acm_io_callbacks io;
	ACMStream *acm;

	if ((f = fopen(filename, "rb")) == NULL)
       		return ACM_ERR_OPEN;

	memset(&io, 0, sizeof(io));
	io.read_func = _read_file;
	io.seek_func = _seek_file;
	io.close_func = _close_file;
	io.get_length_func = _get_length_file;

	if ((err = acm_open_decoder(&acm, f, io, force_chans)) < 0) {
		fclose(f);
		return err;
	}
	*res = acm;
	return 0;
}

/*
 * utility functions
 */

static unsigned pcm2time(ACMStream *acm, unsigned long long pcm)
{
	return pcm * 1000 / acm->info.rate;
	/* return ((10 * pcm) / acm->info.rate) * 100; */
}

static unsigned time2pcm(ACMStream *acm, unsigned long long time_ms)
{
	return time_ms * acm->info.rate / 1000;
	/* return (time_ms / 100) * (acm->info.rate / 10); */
}

/*
 * info functions
 */

const ACMInfo *acm_info(ACMStream *acm)
{
	return &acm->info;
}

unsigned acm_rate(ACMStream *acm)
{
	return acm->info.rate;
}

unsigned acm_channels(ACMStream *acm)
{
	return acm->info.channels;
}

int acm_seekable(ACMStream *acm)
{
	return acm->data_len > 0;
}

unsigned acm_bitrate(ACMStream *acm)
{
	unsigned long long bits, time, bitrate = 0;

	if (acm_raw_total(acm) == 0)
		return 13000;

	time = acm_time_total(acm);
	if (time > 0) {
		bits = 8 * acm_raw_total(acm);
		bitrate = 1000 * bits / time;
	}
	return bitrate;
}

unsigned acm_pcm_tell(ACMStream *acm)
{
	return acm->stream_pos / acm->info.channels;
}

unsigned acm_pcm_total(ACMStream *acm)
{
	return acm->total_values / acm->info.channels;
}

unsigned acm_time_tell(ACMStream *acm)
{
	return pcm2time(acm, acm_pcm_tell(acm));
}

unsigned acm_time_total(ACMStream *acm)
{
	return pcm2time(acm, acm_pcm_total(acm));
}

unsigned acm_raw_tell(ACMStream *acm)
{
	return acm->buf_start_ofs + acm->buf_pos;
}

unsigned acm_raw_total(ACMStream *acm)
{
	return acm->data_len;
}

/*
 * seeking
 */

int acm_seek_time(ACMStream *acm, unsigned time_ms)
{
	int res = acm_seek_pcm(acm, time2pcm(acm, time_ms));
	if (res <= 0)
		return res;
	return pcm2time(acm, res);
}

int acm_seek_pcm(ACMStream *acm, unsigned pcm_pos)
{
	unsigned word_pos = pcm_pos * acm->info.channels;
	unsigned start_ofs;

	if (word_pos < acm->stream_pos) {
		if (acm->io.seek_func == NULL)
			return ACM_ERR_NOT_SEEKABLE;

		start_ofs = ACM_HEADER_LEN;
		if (acm->wavc_file)
			start_ofs += WAVC_HEADER_LEN;

		if (acm->io.seek_func(acm->io_arg, start_ofs, SEEK_SET) < 0)
			return ACM_ERR_NOT_SEEKABLE;
	
		acm->file_eof = 0;
		acm->buf_pos = 0;
 		acm->buf_size = 0;
		acm->bit_avail = 0;
		acm->bit_data = 0;

		acm->stream_pos = 0;
		acm->block_pos = 0;
		acm->block_ready = 0;
		acm->buf_start_ofs = ACM_HEADER_LEN;

		memset(acm->wrapbuf, 0, acm->wrapbuf_len * sizeof(int));
	}
	while (acm->stream_pos < word_pos) {
		int step = 2048, res;
		if (acm->stream_pos + step > word_pos)
			step = word_pos - acm->stream_pos;

		res = acm_read(acm, NULL, step*2, 0,2,1);
		if (res < 1)
			break;
	}
	return acm->stream_pos / acm->info.channels;
}

/*
 * read loop - full block reading
 */
int acm_read_loop(ACMStream *acm, void *dst, unsigned bytes,
		int bigendianp, int wordlen, int sgned)
{
	unsigned char *dstp = dst;
	int res, got = 0;
	while (bytes > 0) {
		res = acm_read(acm, dstp, bytes, bigendianp, wordlen, sgned);
		if (res > 0) {
			if (dstp)
				dstp += res;
			got += res;
			bytes -= res;
		} else {
			if (res < 0 && got == 0)
				return res;
			break;
		}
	}
	return got;
}

