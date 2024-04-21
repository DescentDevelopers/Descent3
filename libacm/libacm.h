/*
 * libacm - Interplay ACM audio decoder.
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

#ifndef __LIBACM_H
#define __LIBACM_H

#ifdef __cplusplus // DG: added extern "C", will try to get it upstream
extern "C" {
#endif

#define LIBACM_VERSION "1.3"

#define ACM_ID		0x032897
#define ACM_WORD	2

#define ACM_OK			 0
#define ACM_ERR_OTHER		-1
#define ACM_ERR_OPEN		-2
#define ACM_ERR_NOT_ACM		-3
#define ACM_ERR_READ_ERR	-4
#define ACM_ERR_BADFMT		-5
#define ACM_ERR_CORRUPT		-6
#define ACM_ERR_UNEXPECTED_EOF	-7
#define ACM_ERR_NOT_SEEKABLE	-8

typedef struct ACMInfo {
	unsigned channels;
	unsigned rate;
	unsigned acm_id;
	unsigned acm_version;
	unsigned acm_channels;		/* channels from header (usually wrong) */
	unsigned acm_level;
	unsigned acm_cols;		/* 1 << acm_level */
	unsigned acm_rows;
} ACMInfo;

typedef struct {
	/* read bytes */
	int (*read_func)(void *ptr, int size, int n, void *datasrc);
	/* optional, must support seeking into start*/
	int (*seek_func)(void *datasrc, int offset, int whence);
	/* optional, called on acm_close */
	int (*close_func)(void *datasrc);
	/* returns size in bytes*/
	int (*get_length_func)(void *datasrc);
} acm_io_callbacks;

struct ACMStream {
	ACMInfo info;
	unsigned total_values;

	/* acm data stream */
	void *io_arg;
	acm_io_callbacks io;
	unsigned data_len;

	/* acm stream buffer */
	unsigned char *buf;
	unsigned buf_max, buf_size, buf_pos, bit_avail;
	unsigned bit_data;
	unsigned buf_start_ofs;

	/* block lengths (in samples) */
	unsigned block_len;
	unsigned wrapbuf_len;
	/* buffers */
	int *block;
	int *wrapbuf;
	int *ampbuf;
	int *midbuf;			/* pointer into ampbuf */
	/* result */
	unsigned block_ready:1;
	unsigned file_eof:1;
	unsigned wavc_file:1;
	unsigned stream_pos;			/* in words. absolute */
	unsigned block_pos;			/* in words, relative */
};
typedef struct ACMStream ACMStream;

/* decode.c */
int acm_open_decoder(ACMStream **res, void *io_arg, acm_io_callbacks io, int force_chans);
int acm_read(ACMStream *acm, void *buf, unsigned nbytes,
		int bigendianp, int wordlen, int sgned);
void acm_close(ACMStream *acm);

/* util.c */
int acm_open_file(ACMStream **acm, const char *filename, int force_chans);
const ACMInfo *acm_info(ACMStream *acm);
int acm_seekable(ACMStream *acm);
unsigned acm_bitrate(ACMStream *acm);
unsigned acm_rate(ACMStream *acm);
unsigned acm_channels(ACMStream *acm);
unsigned acm_raw_total(ACMStream *acm);
unsigned acm_raw_tell(ACMStream *acm);
unsigned acm_pcm_total(ACMStream *acm);
unsigned acm_pcm_tell(ACMStream *acm);
unsigned acm_time_total(ACMStream *acm);
unsigned acm_time_tell(ACMStream *acm);
int acm_read_loop(ACMStream *acm, void *dst, unsigned len,
		int bigendianp, int wordlen, int sgned);
int acm_seek_pcm(ACMStream *acm, unsigned pcm_pos);
int acm_seek_time(ACMStream *acm, unsigned pos_ms);
const char *acm_strerror(int err);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

