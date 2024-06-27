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

import java.util.Calendar;

/**
 * This class contains some methods for date and time conversions between Java
 * and the format known from DOS filesystems (e.g. fat)
 * 
 * @author Ewout Prangsma &lt; epr at jnode.org&gt;
 */
final class DosUtils {

    private DosUtils() { /* no instances */ }

    /**
     * Decode a 16-bit encoded DOS date/time into a java date/time.
     * 
     * @param dosDate
     * @param dosTime
     * @return long
     */
    public static long decodeDateTime(int dosDate, int dosTime) {
        final Calendar cal = Calendar.getInstance();
        
        cal.set(Calendar.MILLISECOND, 0);
        cal.set(Calendar.SECOND, (dosTime & 0x1f) * 2);
        cal.set(Calendar.MINUTE, (dosTime >> 5) & 0x3f);
        cal.set(Calendar.HOUR_OF_DAY, dosTime >> 11);

        cal.set(Calendar.DATE, dosDate & 0x1f);
        cal.set(Calendar.MONTH, ((dosDate >> 5) & 0x0f) - 1);
        cal.set(Calendar.YEAR, 1980 + (dosDate >> 9));
        
        return cal.getTimeInMillis();
    }

    /**
     * Encode a java date/time into a 16-bit encoded DOS time
     * 
     * @param javaDateTime
     * @return long
     */
    public static int encodeTime(long javaDateTime) {
        Calendar cal = Calendar.getInstance();
        cal.setTimeInMillis(javaDateTime);
        return 2048 * cal.get(Calendar.HOUR_OF_DAY) + 32 * cal.get(Calendar.MINUTE) +
                cal.get(Calendar.SECOND) / 2;
    }

    /**
     * Encode a java date/time into a 16-bit encoded DOS date
     * 
     * @param javaDateTime
     * @return long
     */
    public static int encodeDate(long javaDateTime) {
        Calendar cal = Calendar.getInstance();
        cal.setTimeInMillis(javaDateTime);
        return 512 * (cal.get(Calendar.YEAR) - 1980) + 32 * (cal.get(Calendar.MONTH) + 1) +
                cal.get(Calendar.DATE);
    }
}
