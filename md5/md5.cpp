/*
        This is the C++ implementation of the MD5 Message-Digest
        Algorithm desrcipted in RFC 1321.
        I translated the C code from this RFC to C++.
        There is now warranty.

        Feb. 12. 2005
        Benjamin Grüdelbach
*/

/*
        Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
        rights reserved.

        License to copy and use this software is granted provided that it
        is identified as the "RSA Data Security, Inc. MD5 Message-Digest
        Algorithm" in all material mentioning or referencing this software
        or this function.

        License is also granted to make and use derivative works provided
        that such works are identified as "derived from the RSA Data
        Security, Inc. MD5 Message-Digest Algorithm" in all material
        mentioning or referencing the derived work.

        RSA Data Security, Inc. makes no representations concerning either
        the merchantability of this software or the suitability of this
        software for any particular purpose. It is provided "as is"
        without express or implied warranty of any kind.

        These notices must be retained in any copies of any part of this
        documentation and/or software.
*/

// md5 class include
#include "md5.h"
#include <BYTESWAP.H>
#include <string.h>

#include "stdio.h"
static FILE *md5log = NULL;

#define MD5_DEBUG_LOG 0

#ifdef OUTRAGE_BIG_ENDIAN
void byteReverse(unsigned char *buf, unsigned longs) {
  uint32_t t;
  do {
    t = (uint32_t)((unsigned)buf[3] << 8 | buf[2]) << 16 | ((unsigned)buf[1] << 8 | buf[0]);
    *(uint32_t *)buf = t;
    buf += 4;
  } while (--longs);
}
#else
#define byteReverse(buf, len) /* Nothing */
#endif

MD5::~MD5() {
#if MD5_DEBUG_LOG
  if (md5log) {
    fclose(md5log);
    md5log = NULL;
  }
#endif
}

/* MD5 initialization. Begins an MD5 operation, writing a new context. */
void MD5::MD5Init() {
  MD5_CTX *context = &ctx;
  context->buf[0] = 0x67452301;
  context->buf[1] = 0xefcdab89;
  context->buf[2] = 0x98badcfe;
  context->buf[3] = 0x10325476;

  context->bits[0] = 0;
  context->bits[1] = 0;
#if MD5_DEBUG_LOG
  if (!md5log) {
    md5log = fopen("md5.log", "wt+");
  }
  if (md5log) {
    fprintf(md5log, "Starting new sum...\n");
  }
#endif
}

void MD5::MD5Update(float valin) {
  float val = INTEL_FLOAT(valin);
  unsigned char *p = (unsigned char *)&val;
#if MD5_DEBUG_LOG
  if (md5log) {
    fprintf(md5log, "[float]");
  }
#endif
  MD5Update(p, sizeof(float));
}

void MD5::MD5Update(int valin) {
  int val = INTEL_INT(valin);
  unsigned char *p = (unsigned char *)&val;
#if MD5_DEBUG_LOG
  if (md5log) {
    fprintf(md5log, "[int]");
  }
#endif
  MD5Update(p, sizeof(int));
}

void MD5::MD5Update(short valin) {
  short val = INTEL_SHORT(valin);
  unsigned char *p = (unsigned char *)&val;
#if MD5_DEBUG_LOG
  if (md5log) {
    fprintf(md5log, "[short]");
  }
#endif
  MD5Update(p, sizeof(short));
}

void MD5::MD5Update(unsigned int valin) {
  unsigned int val = INTEL_INT(valin);
  unsigned char *p = (unsigned char *)&val;
#if MD5_DEBUG_LOG
  if (md5log) {
    fprintf(md5log, "[u_int]");
  }
#endif
  MD5Update(p, sizeof(unsigned int));
}

void MD5::MD5Update(unsigned char val) {
  unsigned char *p = (unsigned char *)&val;
#if MD5_DEBUG_LOG
  if (md5log) {
    fprintf(md5log, "[u_char]");
  }
#endif
  MD5Update(p, sizeof(unsigned char));
}

/*
         MD5 block update operation. Continues an MD5 message-digest
         operation, processing another message block, and updating the
         context.
*/
void MD5::MD5Update(unsigned char *buf, unsigned int len) {
  MD5_CTX *context = &ctx;
#if MD5_DEBUG_LOG
  if (md5log) {
    fprintf(md5log, "(%d) ", len);
    for (unsigned int a = 0; a < len; a++) {
      fprintf(md5log, "%x ", buf[a] & 0xff);
    }
    fprintf(md5log, "\n");
  }
#endif
  uint32_t t;

  /* Update bitcount */

  t = context->bits[0];
  if ((context->bits[0] = t + ((uint32_t)len << 3)) < t)
    context->bits[1]++; /* Carry from low to high */
  context->bits[1] += len >> 29;

  t = (t >> 3) & 0x3f; /* Bytes already in shsInfo->data */

  /* Handle any leading odd-sized chunks */

  if (t) {
    unsigned char *p = (unsigned char *)context->in + t;

    t = 64 - t;
    if (len < t) {
      memcpy(p, buf, len);
      return;
    }
    memcpy(p, buf, t);
    byteReverse(context->in, 16);
    MD5Transform(context->buf, (uint32_t *)context->in);
    buf += t;
    len -= t;
  }
  /* Process data in 64-byte chunks */

  while (len >= 64) {
    memcpy(context->in, buf, 64);
    byteReverse(context->in, 16);
    MD5Transform(context->buf, (uint32_t *)context->in);
    buf += 64;
    len -= 64;
  }

  /* Handle any remaining bytes of data. */

  memcpy(context->in, buf, len);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
  the message digest and zeroizing the context.
 */
void MD5::MD5Final(unsigned char digest[16]) {
  MD5_CTX *context = &ctx;
  unsigned count;
  unsigned char *p;

  /* Compute number of bytes mod 64 */
  count = (context->bits[0] >> 3) & 0x3F;

  /* Set the first char of padding to 0x80.  This is safe since there is
       always at least one byte free */
  p = context->in + count;
  *p++ = 0x80;

  /* Bytes of padding needed to make 64 bytes */
  count = 64 - 1 - count;

  /* Pad out to 56 mod 64 */
  if (count < 8) {
    /* Two lots of padding:  Pad the first block to 64 bytes */
    memset(p, 0, count);
    byteReverse(context->in, 16);
    MD5Transform(context->buf, (uint32_t *)context->in);

    /* Now fill the next block with 56 bytes */
    memset(context->in, 0, 56);
  } else {
    /* Pad block to 56 bytes */
    memset(p, 0, count - 8);
  }
  byteReverse(context->in, 14);

  /* Append length in bits and transform */
  ((uint32_t *)context->in)[14] = context->bits[0];
  ((uint32_t *)context->in)[15] = context->bits[1];

  MD5Transform(context->buf, (uint32_t *)context->in);
  byteReverse((unsigned char *)context->buf, 4);
  memcpy(digest, context->buf, 16);

  memset(context, 0, sizeof(*context)); /* In case it's sensitive */
                                        /* The original version of this code omitted the asterisk. In
       effect, only the first part of context was wiped
                                         * with zeros, not
       the whole thing. Bug found by Derek Jones. Original line: */
                                        // memset(context, 0, sizeof(context));	/* In case it's sensitive */
}

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) (w += f(x, y, z) + data, w = w << s | w >> (32 - s), w += x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new
 * data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
void MD5::MD5Transform(uint32_t buf[4], uint32_t const in[16]) {
  uint32_t a, b, c, d;

  a = buf[0];
  b = buf[1];
  c = buf[2];
  d = buf[3];

  MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
  MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
  MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
  MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
  MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
  MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
  MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
  MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
  MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
  MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
  MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
  MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
  MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
  MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
  MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
  MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

  MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
  MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
  MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
  MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
  MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
  MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
  MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
  MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
  MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
  MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
  MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
  MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
  MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
  MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
  MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
  MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

  MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
  MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
  MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
  MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
  MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
  MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
  MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
  MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
  MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
  MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
  MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
  MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
  MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
  MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
  MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
  MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

  MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
  MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
  MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
  MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
  MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
  MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
  MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
  MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
  MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
  MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
  MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
  MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
  MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
  MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
  MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
  MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}

/* Encodes input (unsigned long int) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
void MD5::Encode(unsigned char *output, unsigned int *input, unsigned int len) {
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    unsigned int inp = INTEL_INT(input[i]);
    output[j] = (unsigned char)(inp & 0xff);
    output[j + 1] = (unsigned char)((inp >> 8) & 0xff);
    output[j + 2] = (unsigned char)((inp >> 16) & 0xff);
    output[j + 3] = (unsigned char)((inp >> 24) & 0xff);
  }
}

/* Decodes input (unsigned char) into output (unsigned long int). Assumes len is
  a multiple of 4.
 */
void MD5::Decode(unsigned int *output, unsigned char *input, unsigned int len) {
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    unsigned int inp0 = INTEL_INT(input[j]);
    unsigned int inp1 = INTEL_INT(input[j + 1]);
    unsigned int inp2 = INTEL_INT(input[j + 2]);
    unsigned int inp3 = INTEL_INT(input[j + 3]);
    output[i] = ((unsigned int)inp0) | (((unsigned int)inp1) << 8) | (((unsigned int)inp2) << 16) |
                (((unsigned int)inp3) << 24);
  }
}

/* Note: Replace "for loop" with standard memcpy if possible.
 */

void MD5::MD5_memcpy(POINTER output, POINTER input, unsigned int len) {
  unsigned int i;

  for (i = 0; i < len; i++)
    output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
 */
void MD5::MD5_memset(POINTER output, int value, unsigned int len) {
  unsigned int i;
  for (i = 0; i < len; i++) {
    ((char *)output)[i] = (char)value;
  }
}

MD5 *MD5::Clone() {
  MD5 *clone = new MD5();
  clone->ctx = this->ctx;
  return clone;
}

void MD5::Destroy(MD5 *obj) {
  if (obj)
    delete obj;
}