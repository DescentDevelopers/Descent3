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

#define S int16_t
int16_t snd_8to16[256] = {(S)0,         (S)1,      (S)2,      (S)3,      (S)4,      (S)5,      (S)6,      (S)7,
                          (S)(S)8,      (S)9,      (S)10,     (S)11,     (S)12,     (S)13,     (S)14,     (S)15,
                          (S)(S)16,     (S)17,     (S)18,     (S)19,     (S)20,     (S)21,     (S)22,     (S)23,
                          (S)(S)24,     (S)25,     (S)26,     (S)27,     (S)28,     (S)29,     (S)30,     (S)31,
                          (S)(S)32,     (S)33,     (S)34,     (S)35,     (S)36,     (S)37,     (S)38,     (S)39,
                          (S)(S)40,     (S)41,     (S)42,     (S)43,     (S)47,     (S)51,     (S)56,     (S)61,
                          (S)(S)66,     (S)72,     (S)79,     (S)86,     (S)94,     (S)102,    (S)112,    (S)122,
                          (S)(S)133,    (S)145,    (S)158,    (S)173,    (S)189,    (S)206,    (S)225,    (S)245,
                          (S)(S)267,    (S)292,    (S)318,    (S)348,    (S)379,    (S)414,    (S)452,    (S)493,
                          (S)(S)538,    (S)587,    (S)640,    (S)699,    (S)763,    (S)832,    (S)908,    (S)991,
                          (S)(S)1081,   (S)1180,   (S)1288,   (S)1405,   (S)1534,   (S)1673,   (S)1826,   (S)1993,
                          (S)(S)2175,   (S)2373,   (S)2590,   (S)2826,   (S)3084,   (S)3365,   (S)3672,   (S)4008,
                          (S)(S)4373,   (S)4772,   (S)5208,   (S)5683,   (S)6202,   (S)6767,   (S)7385,   (S)8059,
                          (S)(S)8794,   (S)9597,   (S)10472,  (S)11428,  (S)12471,  (S)13609,  (S)14851,  (S)16206,
                          (S)(S)17685,  (S)19298,  (S)21060,  (S)22981,  (S)25078,  (S)27367,  (S)29864,  (S)32589,
                          (S)(S)35563,  (S)38808,  (S)42350,  (S)46214,  (S)50431,  (S)55033,  (S)60055,  (S)65535,
                          (S)(S)-65535, (S)-65535, (S)-60055, (S)-55033, (S)-50431, (S)-46214, (S)-42350, (S)-38808,
                          (S)(S)-35563, (S)-32589, (S)-29864, (S)-27367, (S)-25078, (S)-22981, (S)-21060, (S)-19298,
                          (S)(S)-17685, (S)-16206, (S)-14851, (S)-13609, (S)-12471, (S)-11428, (S)-10472, (S)-9597,
                          (S)(S)-8794,  (S)-8059,  (S)-7385,  (S)-6767,  (S)-6202,  (S)-5683,  (S)-5208,  (S)-4772,
                          (S)(S)-4373,  (S)-4008,  (S)-3672,  (S)-3365,  (S)-3084,  (S)-2826,  (S)-2590,  (S)-2373,
                          (S)(S)-2175,  (S)-1993,  (S)-1826,  (S)-1673,  (S)-1534,  (S)-1405,  (S)-1288,  (S)-1180,
                          (S)(S)-1081,  (S)-991,   (S)-908,   (S)-832,   (S)-763,   (S)-699,   (S)-640,   (S)-587,
                          (S)(S)-538,   (S)-493,   (S)-452,   (S)-414,   (S)-379,   (S)-348,   (S)-318,   (S)-292,
                          (S)(S)-267,   (S)-245,   (S)-225,   (S)-206,   (S)-189,   (S)-173,   (S)-158,   (S)-145,
                          (S)(S)-133,   (S)-122,   (S)-112,   (S)-102,   (S)-94,    (S)-86,    (S)-79,    (S)-72,
                          (S)(S)-66,    (S)-61,    (S)-56,    (S)-51,    (S)-47,    (S)-43,    (S)-42,    (S)-41,
                          (S)(S)-40,    (S)-39,    (S)-38,    (S)-37,    (S)-36,    (S)-35,    (S)-34,    (S)-33,
                          (S)(S)-32,    (S)-31,    (S)-30,    (S)-29,    (S)-28,    (S)-27,    (S)-26,    (S)-25,
                          (S)(S)-24,    (S)-23,    (S)-22,    (S)-21,    (S)-20,    (S)-19,    (S)-18,    (S)-17,
                          (S)(S)-16,    (S)-15,    (S)-14,    (S)-13,    (S)-12,    (S)-11,    (S)-10,    (S)-9,
                          (S)(S)-8,     (S)-7,     (S)-6,     (S)-5,     (S)-4,     (S)-3,     (S)-2,     (S)-1};
#undef S
