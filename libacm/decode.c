/*  
 * ACM decoder.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libacm.h"

#define ACM_BUFLEN	(64*1024)

#define ACM_EXPECTED_EOF -99

typedef int (*filler_t)(ACMStream *acm, unsigned ind, unsigned col);

/**************************************
 * Stream processing
 **************************************/

/* NB: bits <= 31!  Thus less checks in code. */

static int load_buf(ACMStream *acm)
{
	int res = 0;

	if (acm->file_eof)
		return 0;

	acm->buf_start_ofs += acm->buf_size;

	if (acm->io.read_func != NULL)
		res = acm->io.read_func(acm->buf, 1, acm->buf_max,
				acm->io_arg);

	if (res < 0)
		return ACM_ERR_READ_ERR;

	if (res == 0) {
		acm->file_eof = 1;
		/* add single zero byte */
		acm->buf[0] = 0;
		acm->buf_size = 1;
	} else {
		acm->buf_size = res;
	}
	acm->buf_pos = 0;
	return 0;
}

static int load_bits(ACMStream *acm)
{
	int err;
	unsigned data, got;
	uint8_t *p = acm->buf + acm->buf_pos;
	switch (acm->buf_size - acm->buf_pos) {
	default:
		data = 0;
		got = 0;
		break;
	case 1:
		data = p[0];
		got = 8;
		break;
	case 2:
		data = p[0] + (p[1] << 8);
		got = 16;
		break;
	case 3:
		data = p[0] + (p[1] << 8) + (p[2] << 16);
		got = 24;
		break;
	}
	
	if ((err = load_buf(acm)) < 0)
		return err;
	
	while (got < 32) {
		if (acm->buf_size - acm->buf_pos == 0)
			break;
		data |= acm->buf[acm->buf_pos] << got;
		got += 8;
		acm->buf_pos++;
	}
	acm->bit_data = data;
	acm->bit_avail = got;
	return 0;
}

static int get_bits_reload(ACMStream *acm, unsigned bits)
{
	int got, err;
	unsigned data, b_data, b_avail;

	data = acm->bit_data;
	got = acm->bit_avail;
	bits -= got;

	if (acm->buf_size - acm->buf_pos >= 4) {
		uint8_t *p = acm->buf + acm->buf_pos;
		acm->buf_pos += 4;
		b_data = p[0] + (p[1] << 8) + (p[2] << 16) + (p[3] << 24);
		b_avail = 32;
	} else	{
		if ((err = load_bits(acm)) < 0)
			return err;
		if (acm->bit_avail < bits)
			return ACM_ERR_UNEXPECTED_EOF;
		b_data = acm->bit_data;
		b_avail = acm->bit_avail;
	}
	
	data |= (b_data & ((1 << bits) - 1)) << got;
	acm->bit_data = b_data >> bits;
	acm->bit_avail = b_avail - bits;
	return data;
}

#define GET_BITS_NOERR(tmpval, acm, bits) do { \
		if (acm->bit_avail >= bits) { \
			tmpval = acm->bit_data & ((1 << bits) - 1); \
			acm->bit_data >>= bits; \
			acm->bit_avail -= bits; \
		} else \
			tmpval = get_bits_reload(acm, bits); \
	} while (0)

#define GET_BITS(res, acm, bits) do { \
		int tmpval; \
		GET_BITS_NOERR(tmpval, acm, bits); \
		if (tmpval < 0) \
			return tmpval; \
		res = tmpval; \
	} while (0)

#define GET_BITS_EXPECT_EOF(res, acm, bits) do { \
		int tmpval; \
		GET_BITS_NOERR(tmpval, acm, bits); \
		if (tmpval < 0) { \
			if (tmpval == ACM_ERR_UNEXPECTED_EOF) \
				return ACM_EXPECTED_EOF; \
			return tmpval; \
		} \
		res = tmpval; \
	} while (0)

/*************************************************
 * Table filling
 *************************************************/
static const int map_1bit[] = { -1, +1 };
static const int map_2bit_near[] = { -2, -1, +1, +2 };
static const int map_2bit_far[] = { -3, -2, +2, +3 };
static const int map_3bit[] = { -4, -3, -2, -1, +1, +2, +3, +4 };

/* IOW: (r * acm->subblock_len) + c */
#define set_pos(acm, r, c, idx) do { \
		unsigned _pos = ((r) << acm->info.acm_level) + (c); \
		acm->block[_pos] = acm->midbuf[idx]; \
	} while (0)

/************ Fillers **********/

static int f_zero(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i;
	for (i = 0; i < acm->info.acm_rows; i++)
		set_pos(acm, i, col, 0);
	
	return 1;
}

static int f_bad(ACMStream *acm, unsigned ind, unsigned col)
{
	/* corrupt block? */
	return ACM_ERR_CORRUPT;
}

static int f_linear(ACMStream *acm, unsigned ind, unsigned col)
{
	uint32_t i;
	int b, middle = 1 << (ind - 1);

	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, ind);
		set_pos(acm, i, col, b - middle);
	}
	return 1;
}

static int f_k13(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i++, col, 0);
			if (i >= acm->info.acm_rows)
				break;
			set_pos(acm, i, col, 0);
			continue;
		}
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 1, 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		/* 1, 1, ? */
		GET_BITS(b, acm, 1);
		set_pos(acm, i, col, map_1bit[b]);
	}
	return 1;
}

static int f_k12(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		
		/* 1, ? */
		GET_BITS(b, acm, 1);
		set_pos(acm, i, col, map_1bit[b]);
	}
	return 1;
}

static int f_k24(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i++, col, 0);
			if (i >= acm->info.acm_rows) break;
			set_pos(acm, i, col, 0);
			continue;
		}
		
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 1, 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		
		/* 1, 1, ?, ? */
		GET_BITS(b, acm, 2);
		set_pos(acm, i, col, map_2bit_near[b]);
	}
	return 1;
}

static int f_k23(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i, col, 0);
			continue;
		}

		/* 1, ?, ? */
		GET_BITS(b, acm, 2);
		set_pos(acm, i, col, map_2bit_near[b]);
	}
	return 1;
}

static int f_k35(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i++, col, 0);
			if (i >= acm->info.acm_rows)
				break;
			set_pos(acm, i, col, 0);
			continue;
		}
		
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 1, 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 1, 1, 0, ? */
			GET_BITS(b, acm, 1);
			set_pos(acm, i, col, map_1bit[b]);
			continue;
		}
		
		/* 1, 1, 1, ?, ? */
		GET_BITS(b, acm, 2);
		set_pos(acm, i, col, map_2bit_far[b]);
	}
	return 1;
}

static int f_k34(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 1, 0, ? */
			GET_BITS(b, acm, 1);
			set_pos(acm, i, col, map_1bit[b]);
			continue;
		}
		
		/* 1, 1, ?, ? */
		GET_BITS(b, acm, 2);
		set_pos(acm, i, col, map_2bit_far[b]);
	}
	return 1;
}

static int f_k45(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i, col, 0); i++;
			if (i >= acm->info.acm_rows)
				break;
			set_pos(acm, i, col, 0);
			continue;
		} 
		
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 1, 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		
		/* 1, 1, ?, ?, ? */
		GET_BITS(b, acm, 3);
		set_pos(acm, i, col, map_3bit[b]);
	}
	return 1;
}

static int f_k44(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	for (i = 0; i < acm->info.acm_rows; i++) {
		GET_BITS(b, acm, 1);
		if (b == 0) {
			/* 0 */
			set_pos(acm, i, col, 0);
			continue;
		}
		
		/* 1, ?, ?, ? */
		GET_BITS(b, acm, 3);
		set_pos(acm, i, col, map_3bit[b]);
	}
	return 1;
}

static int f_t15(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	int n1, n2, n3, tmp;
	for (i = 0; i < acm->info.acm_rows; i++) {
		/* b = (x1) + (x2 * 3) + (x3 * 9) */
		GET_BITS(b, acm, 5);
		if (b >= 3 * 3 * 3)
			return ACM_ERR_CORRUPT;

		n1 = b % 3 - 1;
		tmp = b / 3;
		n2 = tmp % 3 - 1;
		n3 = tmp / 3 - 1;

		set_pos(acm, i++, col, n1);
		if (i >= acm->info.acm_rows)
			break;
		set_pos(acm, i++, col, n2);
		if (i >= acm->info.acm_rows)
			break;
		set_pos(acm, i, col, n3);
	}
	return 1;
}

static int f_t27(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	int n1, n2, n3, tmp;
	for (i = 0; i < acm->info.acm_rows; i++) {
		/* b = (x1) + (x2 * 5) + (x3 * 25) */
		GET_BITS(b, acm, 7);
		if (b >= 5 * 5 * 5)
			return ACM_ERR_CORRUPT;

		n1 = b % 5 - 2;
		tmp = b / 5;
		n2 = tmp % 5 - 2;
		n3 = tmp / 5 - 2;

		set_pos(acm, i++, col, n1);
		if (i >= acm->info.acm_rows)
			break;
		set_pos(acm, i++, col, n2);
		if (i >= acm->info.acm_rows)
			break;
		set_pos(acm, i, col, n3);
	}
	return 1;
}

static int f_t37(ACMStream *acm, unsigned ind, unsigned col)
{
	unsigned i, b;
	int n1, n2;
	for (i = 0; i < acm->info.acm_rows; i++) {
		/* b = (x1) + (x2 * 11) */
		GET_BITS(b, acm, 7);
		if (b >= 11 * 11)
			return ACM_ERR_CORRUPT;

		n1 = b % 11 - 5;
		n2 = b / 11 - 5;

		set_pos(acm, i++, col, n1);
		if (i >= acm->info.acm_rows)
			break;
		set_pos(acm, i, col, n2);
	}
	return 1;
}

/****************/

static const filler_t filler_list[] = {
	f_zero, f_bad, f_bad, f_linear, 	/* 0..3 */
	f_linear, f_linear, f_linear, f_linear,	/* 4..7 */
	f_linear, f_linear, f_linear, f_linear,	/* 8..11 */
	f_linear, f_linear, f_linear, f_linear,	/* 12..15 */
	f_linear, f_k13, f_k12, f_t15,		/* 16..19 */
	f_k24, f_k23, f_t27, f_k35,		/* 20..23 */
	f_k34, f_bad, f_k45, f_k44,		/* 24..27 */
	f_bad, f_t37, f_bad, f_bad		/* 28..31 */
};

static int fill_block(ACMStream *acm)
{
	unsigned i, ind;
	int err;
	for (i = 0; i < acm->info.acm_cols; i++) {
		GET_BITS_EXPECT_EOF(ind, acm, 5);
		err = filler_list[ind](acm, ind, i);
		if (err < 0)
			return err;
	}
	return 1;
}

/**********************************************
 * Decompress code
 **********************************************/

static void juggle(int *wrap_p, int *block_p, unsigned sub_len, unsigned sub_count)
{
	uint32_t i, j;
	int *p;
	uint32_t r0, r1, r2, r3;
	for (i = 0; i < sub_len; i++) {
		p = block_p;
		r0 = wrap_p[0];
		r1 = wrap_p[1];
		for (j = 0; j < sub_count/2; j++) {
			r2 = *p;  *p = r1*2 + (r0 + r2);  p += sub_len;
			r3 = *p;  *p = r2*2 - (r1 + r3);  p += sub_len;
			r0 = r2;  r1 = r3;
		}
		*wrap_p++ = r0;
		*wrap_p++ = r1;
		block_p++;
	}
}

static void juggle_block(ACMStream *acm)
{
	unsigned sub_count, sub_len, todo_count, step_subcount, i;
	int *wrap_p, *block_p, *p;
	
	/* juggle only if subblock_len > 1 */
	if (acm->info.acm_level == 0)
		return;

	/* 2048 / subblock_len */
	if (acm->info.acm_level > 9)
		step_subcount = 1;
	else
		step_subcount = (2048 >> acm->info.acm_level) - 2;

	/* Apply juggle()  (rows)x(cols)
	 * from (step_subcount * 2)            x (subblock_len/2)
	 * to   (step_subcount * subblock_len) x (1)
	 */
	todo_count = acm->info.acm_rows;
	block_p = acm->block;
	while (1) {
		wrap_p = acm->wrapbuf;
		sub_count = step_subcount;
		if (sub_count > todo_count)
			sub_count = todo_count;

		sub_len = acm->info.acm_cols / 2;
		sub_count *= 2;
		
		juggle(wrap_p, block_p, sub_len, sub_count);
		wrap_p += sub_len*2;
		
		for (i = 0, p = block_p; i < sub_count; i++) {
			p[0]++;
			p += sub_len;
		}
		
		while (sub_len > 1) {
			sub_len /= 2;
			sub_count *= 2;
			juggle(wrap_p, block_p, sub_len, sub_count);
			wrap_p += sub_len*2;
		}
		if (todo_count <= step_subcount)
			break;
		todo_count -= step_subcount;
		block_p += step_subcount << acm->info.acm_level;
	}
}

/***************************************************************/
static int decode_block(ACMStream *acm)
{
	int pwr, count, val, i, x, err;

	acm->block_ready = 0;
	acm->block_pos = 0;

	/* read header */
	GET_BITS_EXPECT_EOF(pwr, acm, 4);
	GET_BITS_EXPECT_EOF(val, acm, 16);

	/* generate tables */
	count = 1 << pwr;
	for (i = 0, x = 0; i < count; i++) {
		acm->midbuf[i] = x;
		x += val;
	}
	for (i = 1, x = -val; i <= count; i++) {
		acm->midbuf[-i] = (unsigned)x;
		x -= val;
	}

	/* to_check? */
	if ((err = fill_block(acm)) <= 0)
		return err;

	juggle_block(acm);

	acm->block_ready = 1;

	return 1;
}

/******************************
 * Output formats
 ******************************/

static uint8_t *out_s16le(int *src, uint8_t *dst, unsigned n, unsigned shift)
{
	while (n--) {
		int val = *src++ >> shift;
		*dst++ = val & 0xFF;
		*dst++ = (val >> 8) & 0xFF;
	}
	return dst;
}

static uint8_t *out_s16be(int *src, uint8_t *dst, unsigned n, unsigned shift)
{
	while (n--) {
		int val = *src++ >> shift;
		*dst++ = (val >> 8) & 0xFF;
		*dst++ = val & 0xFF;
	}
	return dst;
}

static uint8_t *out_u16le(int *src, uint8_t *dst, unsigned n, unsigned shift)
{
	while (n--) {
		int val = (*src++ >> shift) + 0x8000;
		*dst++ = val & 0xFF;
		*dst++ = (val >> 8) & 0xFF;
	}
	return dst;
}

static uint8_t *out_u16be(int *src, uint8_t *dst, unsigned n, unsigned shift)
{
	while (n--) {
		int val = (*src++ >> shift) + 0x8000;
		*dst++ = (val >> 8) & 0xFF;
		*dst++ = val & 0xFF;
	}
	return dst;
}

static int output_values(int *src, uint8_t *dst, int n,
		int acm_level, int bigendianp, int wordlen, int sgned)
{
	uint8_t *res = NULL;
	if (wordlen == 2) {
		if (bigendianp == 0) {
			if (sgned)
				res = out_s16le(src, dst, n, acm_level);
			else
				res = out_u16le(src, dst, n, acm_level);
		} else {
			if (sgned)
				res = out_s16be(src, dst, n, acm_level);
			else
				res = out_u16be(src, dst, n, acm_level);
		}
	}
	if (res != NULL)
		return res - dst;
	return ACM_ERR_BADFMT;
}

/*
 * WAVC (compressed WAV) files are ACM files with additional header.
 *
 * 'WAVC' + 'V1.00' + uncompr(4b) + compr(4b) + 12b
 */

#define WAVC_ID 0x564157  /* 'WAV' */

static int read_wavc_header(ACMStream *acm)
{
	static const uint16_t expect[12] = {
		/* 'V1.0', raw_size, acm_size */
		0x3156, 0x302E, 0,0, 0,0,
		/* hdrlen?, chans?, bits?, hz */
		28,0, 1, 16, 22050, 0
	};
	uint16_t i, buf[12];

	for (i = 0; i < 12; i++)
		GET_BITS(buf[i], acm, 16);
	if (memcmp(buf, expect, 4) != 0)
		return -1;
	/* full comparision is too strict */
	if (0 && memcmp(buf + 6, expect + 6, 12) != 0)
		return -1;
	/* just make sure the magic 28 is there */
	if (expect[6] != buf[6])
		return -1;

	acm->wavc_file = 1;
	return 0;
}

static int read_header(ACMStream *acm)
{
	uint32_t tmp;

	/* read header */

	GET_BITS(tmp, acm, 24);
	if (tmp == WAVC_ID) {
		GET_BITS(tmp, acm, 8);
		if (tmp != 'C')
			return ACM_ERR_NOT_ACM;
		if (read_wavc_header(acm) < 0)
			return ACM_ERR_NOT_ACM;
		GET_BITS(tmp, acm, 24);
	}
	if (tmp != ACM_ID)
		return ACM_ERR_NOT_ACM;
	acm->info.acm_id = tmp;

	GET_BITS(acm->info.acm_version, acm, 8);
	if (acm->info.acm_version != 1)
		return ACM_ERR_NOT_ACM;
	GET_BITS(acm->total_values, acm, 16);
	GET_BITS(tmp, acm, 16);
	acm->total_values += tmp << 16;
	if (acm->total_values == 0)
		return ACM_ERR_NOT_ACM;
	GET_BITS(acm->info.channels, acm, 16);
	if (acm->info.channels < 1 || acm->info.channels > 2)
		return ACM_ERR_NOT_ACM;
	acm->info.acm_channels = acm->info.channels;
	GET_BITS(acm->info.rate, acm, 16);
	if (acm->info.rate < 4096)
		return ACM_ERR_NOT_ACM;
	
	GET_BITS(acm->info.acm_level, acm, 4);
	GET_BITS(acm->info.acm_rows, acm, 12);
	if (!acm->info.acm_rows)
		return ACM_ERR_NOT_ACM;
	return 0;
}

/***********************************************
 * Public functions
 ***********************************************/

int acm_open_decoder(ACMStream **res, void *arg, struct acm_io_callbacks io_cb, int force_chans)
{
	int err = ACM_ERR_OTHER;
	ACMStream *acm;
	
	acm = malloc(sizeof(*acm));
	if (!acm)
		return err;
	memset(acm, 0, sizeof(*acm));

	acm->io_arg = arg;
	acm->io = io_cb;

	if (acm->io.get_length_func) {
		acm->data_len = acm->io.get_length_func(acm->io_arg);
	} else {
		acm->data_len = 0;
	}
	
	acm->buf_max = ACM_BUFLEN;
	acm->buf = malloc(acm->buf_max);
	if (!acm->buf) 
		goto err_out;

	/* read header data */
	err = ACM_ERR_NOT_ACM;
	if (read_header(acm) < 0)
		goto err_out;

	/*
	 * Overwrite channel info if requested, if force_chans == 0
	 * use channel count from the header.
	 * For force_chans == -1, assume that "plain" ACM files are always stereo
	 * (there are many plain ACM files in the wild that are really stereo
	 *  even though the header specifies 1 channel), but still trust the
	 * header of WAVC ACM files, as those seem to be correct.
	 */
	if (force_chans > 0)
		acm->info.channels = force_chans;
	else if (force_chans == -1 && !acm->wavc_file && acm->info.channels < 2)
		acm->info.channels = 2;
	/* else if force_chans == 0, trust the file's header */

	/* calculate blocks */
	acm->info.acm_cols = 1 << acm->info.acm_level;
	acm->wrapbuf_len = 2 * acm->info.acm_cols - 2;
	acm->block_len = acm->info.acm_rows * acm->info.acm_cols;

	/* allocate */
	acm->block = malloc(acm->block_len * sizeof(int));
	acm->wrapbuf = malloc(acm->wrapbuf_len * sizeof(int));
	acm->ampbuf = malloc(0x10000 * sizeof(int));
	acm->midbuf = acm->ampbuf + 0x8000;

	memset(acm->wrapbuf, 0, acm->wrapbuf_len * sizeof(int));

	*res = acm;
	return ACM_OK;

err_out:
	/* disable callbacks */
	memset(&acm->io, 0, sizeof(acm->io));
	acm->io_arg = NULL;

	acm_close(acm);
	return err;
}

int acm_read(ACMStream *acm, void *dst, unsigned numbytes,
		 int bigendianp, int wordlen, int sgned)
{
	int avail, gotbytes = 0, err;
	int *src, numwords;

	if (wordlen == 2)
		numwords = numbytes / 2;
	else
		return ACM_ERR_BADFMT;

	if (acm->stream_pos >= acm->total_values)
		return 0;

	if (!acm->block_ready) {
		err = decode_block(acm);
		if (err == ACM_EXPECTED_EOF)
			return 0;
		if (err < 0)
			return err;
	}

	/* check how many words can be read */
	avail = acm->block_len - acm->block_pos;
	if (avail < numwords)
		numwords = avail;

	if (acm->stream_pos + numwords > acm->total_values)
		numwords = acm->total_values - acm->stream_pos;

	if (acm->info.channels > 1)
		numwords -= numwords % acm->info.channels;

	/* convert, but if dst == NULL, simulate */
	if (dst != NULL) {
		src = acm->block + acm->block_pos;
		gotbytes = output_values(src, dst, numwords,
				acm->info.acm_level,
				bigendianp, wordlen, sgned);
	} else
		gotbytes = numwords * wordlen;

	if (gotbytes >= 0) {
		acm->stream_pos += numwords;
		acm->block_pos += numwords;
		if (acm->block_pos == acm->block_len)
			acm->block_ready = 0;
	}

	return gotbytes;
}

void acm_close(ACMStream *acm)
{
	if (acm == NULL)
		return;
	if (acm->io.close_func)
		acm->io.close_func(acm->io_arg);
	if (acm->buf)
		free(acm->buf);
	if (acm->block)
		free(acm->block);
	if (acm->wrapbuf)
		free(acm->wrapbuf);
	if (acm->ampbuf)
		free(acm->ampbuf);
	free(acm);
}

