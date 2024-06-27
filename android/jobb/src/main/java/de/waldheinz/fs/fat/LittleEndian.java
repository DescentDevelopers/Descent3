/*
 * Copyright (C) 2003-2009 JNode.org
 *               2009,2010 Matthias Treydte <mt@waldheinz.de>
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


/**
 * Little endian (LSB first) conversion methods.
 *
 * @author Ewout Prangsma &lt;epr at users.sourceforge.net&gt;
 */
final class LittleEndian {

    private LittleEndian() { /* no instances */ }
    
    /**
     * Gets an 8-bit unsigned integer from the given byte array at
     * the given offset.
     *
     * @param src the byte offset where to read the value from
     * @param offset the byte array to extract the value from
     * @return the integer that was read
     */
    public static int getUInt8(byte[] src, int offset) {
        return src[offset] & 0xFF;
    }

    /**
     * Gets a 16-bit unsigned integer from the given byte array at the given offset.
     *
     * @param src
     * @param offset
     */
    public static int getUInt16(byte[] src, int offset) {
        final int v0 = src[offset + 0] & 0xFF;
        final int v1 = src[offset + 1] & 0xFF;
        return ((v1 << 8) | v0);
    }

    /**
     * Gets a 32-bit unsigned integer from the given byte array at the given offset.
     *
     * @param src
     * @param offset
     */
    public static long getUInt32(byte[] src, int offset) {
        final long v0 = src[offset + 0] & 0xFF;
        final long v1 = src[offset + 1] & 0xFF;
        final long v2 = src[offset + 2] & 0xFF;
        final long v3 = src[offset + 3] & 0xFF;
        return ((v3 << 24) | (v2 << 16) | (v1 << 8) | v0);
    }

    /**
     * Sets an 8-bit integer in the given byte array at the given offset.
     */
    public static void setInt8(byte[] dst, int offset, int value) {
        dst[offset] = (byte) value;
    }

    /**
     * Sets a 16-bit integer in the given byte array at the given offset.
     */
    public static void setInt16(byte[] dst, int offset, int value) {
        dst[offset + 0] = (byte) (value & 0xFF);
        dst[offset + 1] = (byte) ((value >>> 8) & 0xFF);
    }

    /**
     * Sets a 32-bit integer in the given byte array at the given offset.
     */
    public static void setInt32(byte[] dst, int offset, long value)
            throws IllegalArgumentException {
        
        if (value > Integer.MAX_VALUE) {
            throw new IllegalArgumentException(
                    value + " can not be represented in a 32bit dword");
        }
        
        dst[offset + 0] = (byte) (value & 0xFF);
        dst[offset + 1] = (byte) ((value >>> 8) & 0xFF);
        dst[offset + 2] = (byte) ((value >>> 16) & 0xFF);
        dst[offset + 3] = (byte) ((value >>> 24) & 0xFF);
    }
    
}
