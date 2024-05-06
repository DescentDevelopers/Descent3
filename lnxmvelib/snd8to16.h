/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

** #include <math.h>
** void init_snd_8to16(void)
** {
**    int i;
**    for (i=0; i<44; ++i)
**      snd_8to16[i] = i;
**    for (i=44; i<128; ++i)
**      snd_8to16[i] = (unsigned)floor(pow(65535,i/127.0)+.5);
**    for (i=1; i<128; ++i)
**      snd_8to16[256-i] = -snd_8to16[i];
**    snd_8to16[128] = snd_8to16[129];
** }
*/
#pragma warning(disable : 4245)

signed short snd_8to16[256] = {
    0,      1,      2,      3,      4,      5,      6,      7,      8,      9,      10,     11,     12,     13,
    14,     15,     16,     17,     18,     19,     20,     21,     22,     23,     24,     25,     26,     27,
    28,     29,     30,     31,     32,     33,     34,     35,     36,     37,     38,     39,     40,     41,
    42,     43,     47,     51,     56,     61,     66,     72,     79,     86,     94,     102,    112,    122,
    133,    145,    158,    173,    189,    206,    225,    245,    267,    292,    318,    348,    379,    414,
    452,    493,    538,    587,    640,    699,    763,    832,    908,    991,    1081,   1180,   1288,   1405,
    1534,   1673,   1826,   1993,   2175,   2373,   2590,   2826,   3084,   3365,   3672,   4008,   4373,   4772,
    5208,   5683,   6202,   6767,   7385,   8059,   8794,   9597,   10472,  11428,  12471,  13609,  14851,  16206,
    17685,  19298,  21060,  22981,  25078,  27367,  29864,  32589,  35563,  38808,  42350,  46214,  50431,  55033,
    60055,  65535,  -65535, -65535, -60055, -55033, -50431, -46214, -42350, -38808, -35563, -32589, -29864, -27367,
    -25078, -22981, -21060, -19298, -17685, -16206, -14851, -13609, -12471, -11428, -10472, -9597,  -8794,  -8059,
    -7385,  -6767,  -6202,  -5683,  -5208,  -4772,  -4373,  -4008,  -3672,  -3365,  -3084,  -2826,  -2590,  -2373,
    -2175,  -1993,  -1826,  -1673,  -1534,  -1405,  -1288,  -1180,  -1081,  -991,   -908,   -832,   -763,   -699,
    -640,   -587,   -538,   -493,   -452,   -414,   -379,   -348,   -318,   -292,   -267,   -245,   -225,   -206,
    -189,   -173,   -158,   -145,   -133,   -122,   -112,   -102,   -94,    -86,    -79,    -72,    -66,    -61,
    -56,    -51,    -47,    -43,    -42,    -41,    -40,    -39,    -38,    -37,    -36,    -35,    -34,    -33,
    -32,    -31,    -30,    -29,    -28,    -27,    -26,    -25,    -24,    -23,    -22,    -21,    -20,    -19,
    -18,    -17,    -16,    -15,    -14,    -13,    -12,    -11,    -10,    -9,     -8,     -7,     -6,     -5,
    -4,     -3,     -2,     -1};
#pragma warning(default : 4245)
