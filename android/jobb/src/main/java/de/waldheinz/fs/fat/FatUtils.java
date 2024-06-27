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

import java.io.IOException;

/**
 * <description>
 * 
 * @author Ewout Prangsma &lt; epr at jnode.org&gt;
 * @author Fabien DUMINY
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public class FatUtils {
    
    /**
     * Gets the offset (in bytes) of the fat with the given index
     * 
     * @param bs
     * @param fatNr (0..)
     * @return long
     * @throws IOException 
     */
    public static long getFatOffset(BootSector bs, int fatNr) {
        long sectSize = bs.getBytesPerSector();
        long sectsPerFat = bs.getSectorsPerFat();
        long resSects = bs.getNrReservedSectors();

        long offset = resSects * sectSize;
        long fatSize = sectsPerFat * sectSize;

        offset += fatNr * fatSize;

        return offset;
    }

    /**
     * Gets the offset (in bytes) of the root directory with the given index
     * 
     * @param bs
     * @return long
     * @throws IOException 
     */
    public static long getRootDirOffset(BootSector bs) {
        long sectSize = bs.getBytesPerSector();
        long sectsPerFat = bs.getSectorsPerFat();
        int fats = bs.getNrFats();

        long offset = getFatOffset(bs, 0);
        
        offset += fats * sectsPerFat * sectSize;

        return offset;
    }

    /**
     * Gets the offset of the data (file) area
     * 
     * @param bs
     * @return long
     * @throws IOException 
     */
    public static long getFilesOffset(BootSector bs) {
        long offset = getRootDirOffset(bs);
        
        offset += bs.getRootDirEntryCount() * 32;

        return offset;
    }
    
    private FatUtils() { /* no instances */ }
    
}
