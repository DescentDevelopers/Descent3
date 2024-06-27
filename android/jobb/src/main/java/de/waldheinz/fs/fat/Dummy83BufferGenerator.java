/*
 * Copyright (C) 2012 Google, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; If not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

package de.waldheinz.fs.fat;

import java.security.SecureRandom;

/**
 * Generates dummy 8.3 buffers that are associated with the long names.
 * 
 * @author Daniel Galpin &lt;dgalpin@google.com&gt; based upon the work of
 *         Andrew Tridgell &lt;tridge@samba.org&gt;
 */
final class Dummy83BufferGenerator {
    final SecureRandom mRandom;

    /**
     * Creates a new instance of {@code Dummy83BufferGenerator} that uses
     * randomness only to avoid short name collisions.
     */
    public Dummy83BufferGenerator() {
        mRandom = new SecureRandom();
    }

    /*
     * Its in the DOS manual!(DOS 5: page 72) Valid: A..Z 0..9 _ ^ $ ~ ! # % & - {} () @ ' `
     *
     * Invalid: spaces/periods,
     */
    public static boolean validChar(char toTest) {
        if (toTest >= 'A' && toTest <= 'Z') return true;
        if (toTest >= 'a' && toTest <= 'z') return true;
        if (toTest >= '0' && toTest <= '9') return true;
        if (toTest == '_' || toTest == '^' || toTest == '$' || toTest == '~' ||
                toTest == '!' || toTest == '#' || toTest == '%' || toTest == '&' ||
                toTest == '-' || toTest == '{' || toTest == '}' || toTest == '(' ||
                toTest == ')' || toTest == '@' || toTest == '\'' || toTest == '`')
            return true;

        return false;
    }
    
    public static boolean isSkipChar(char c) {
        return (c == '.') || (c == ' ');
    }

    public static String tidyString(String dirty) {
        final StringBuilder result = new StringBuilder();

        /* epurate it from alien characters */
        for (int src=0; src < dirty.length(); src++) {
            final char toTest = Character.toUpperCase(dirty.charAt(src));
            if (isSkipChar(toTest)) continue;

            if (validChar(toTest)) {
                result.append(toTest);
            } else {
                result.append('_');
            }
        }

        return result.toString();
    }

    public static boolean cleanString(String s) {
        for (int i=0; i < s.length(); i++) {
            if (isSkipChar(s.charAt(i))) return false;
            if (!validChar(s.charAt(i))) return false;
        }

        return true;
    }

    public static String stripLeadingPeriods(String str) {
        final StringBuilder sb = new StringBuilder(str.length());

        for (int i=0; i < str.length(); i++) {
            if (str.charAt(i) != '.') { //NOI18N
                sb.append(str.substring(i));
                break;
            }
        }
        
        return sb.toString();
    }    /*
     * These characters are all invalid in 8.3 names, plus have been shown to be
     * harmless on all tested devices
     */
    static final private char[] invalidchar = {
            (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06,
            (char) 0x0B,
            (char) 0x0C, (char) 0x0E, (char) 0x0F, (char) 0x10, (char) 0x11, (char) 0x12,
            (char) 0x13,
            (char) 0x14, (char) 0x15, (char) 0x16, (char) 0x17, (char) 0x18, (char) 0x19,
            (char) 0x1A,
            (char) 0x1B, (char) 0x1C, (char) 0x1D, (char) 0x1E, (char) 0x1F, (char) 0x22,
            (char) 0x2a,
            (char) 0x3a, (char) 0x3c, (char) 0x3e, (char) 0x3f, (char) 0x5b, (char) 0x5d,
            (char) 0x7c
    };

    /**
     * See original C Linux patch by Andrew Tridgell &lt;tridge@samba.org&gt;
     * build a 11 byte 8.3 buffer which is not a short filename. We want 11
     * bytes which: - will be seen as a constant string to all APIs on Linux and
     * Windows - cannot be matched with wildcard patterns - cannot be used to
     * access the file - has a low probability of collision within a directory -
     * has an invalid 3 byte extension - contains at least one non-space and
     * non-nul byte
     * 
     * @param longFullName the long file name to generate the buffer for
     * @return the generated 8.3 buffer
     */
    public ShortName generate83BufferNew(String longFullName)
            throws IllegalStateException {

        char[] retBuffer = new char[11];

        boolean hasRealShortName = false;// getRealShortNameInstead(longFullName,
                                         // retBuffer);
        if (!hasRealShortName) {
            int i, tilde_pos, slash_pos;
            int randomNumber = Math.abs(mRandom.nextInt());

            /*
             * the '/' makes sure that even unpatched Linux systems can't get at
             * files by the 8.3 entry.
             */

            slash_pos = randomNumber % 8;
            randomNumber >>= 3;

            /*
             * fill in the first 8 bytes with invalid characters. Note that we
             * need to be careful not to run out of randomness. We use the same
             * extension for all buffers.
             */
            for (i = 0; i < 8; i++) {
            	if (i == slash_pos)
                    retBuffer[i] = '/';
                else {
                    retBuffer[i] =
                            invalidchar[randomNumber % invalidchar.length];
                    randomNumber /= invalidchar.length;
                    if (randomNumber < invalidchar.length)
                        randomNumber = Math.abs(mRandom.nextInt());
                }
            }
            
            for ( i = 0; i < 8; i ++ ) {
                if (retBuffer[i] == 0xe5) {
                    throw new RuntimeException();
                }
            }

            retBuffer[8] = 'i';
            retBuffer[9] = 'f';
            retBuffer[10] = 'l';
        }
        ShortName retName = new ShortName(retBuffer);
        retName.setHasShortNameOnly(hasRealShortName);
        return retName;
    }

}
