/*
 * Copyright (C) 2009,2010 Matthias Treydte <mt@waldheinz.de>
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

import de.waldheinz.fs.BlockDevice;
import java.io.IOException;

/**
 * Contains the FAT32 specific parts of the boot sector.
 *
 * @author Matthias Treydte &lt;matthias.treydte at meetwise.com&gt;
 */
final class Fat32BootSector extends BootSector {

    /**
     * The offset to the entry specifying the first cluster of the FAT32
     * root directory.
     */
    public final static int ROOT_DIR_FIRST_CLUSTER_OFFSET = 0x2c;

    /**
     * The offset to the 4 bytes specifying the sectors per FAT value.
     */
    public static final int SECTORS_PER_FAT_OFFSET = 0x24;

    /**
     * Offset to the file system type label.
     */
    public static final int FILE_SYSTEM_TYPE_OFFSET = 0x52;
    
    public static final int VERSION_OFFSET = 0x2a;
    public static final int VERSION = 0;

    public static final int FS_INFO_SECTOR_OFFSET = 0x30;
    public static final int BOOT_SECTOR_COPY_OFFSET = 0x32;
    public static final int EXTENDED_BOOT_SIGNATURE_OFFSET = 0x42;
    
    /*
     * TODO: make this constructor private
     */
    public Fat32BootSector(BlockDevice device) throws IOException {
        super(device);
    }
    
    @Override
    public void init() throws IOException {
        super.init();

        set16(VERSION_OFFSET, VERSION);

        setBootSectorCopySector(6); /* as suggested by M$ */
    }

    /**
     * Returns the first cluster in the FAT that contains the root directory.
     *
     * @return the root directory's first cluster
     */
    public long getRootDirFirstCluster() {
        return get32(ROOT_DIR_FIRST_CLUSTER_OFFSET);
    }

    /**
     * Sets the first cluster of the root directory.
     *
     * @param value the root directory's first cluster
     */
    public void setRootDirFirstCluster(long value) {
        if (getRootDirFirstCluster() == value) return;
        
        set32(ROOT_DIR_FIRST_CLUSTER_OFFSET, value);
    }

    /**
     * Sets the sectur number that contains a copy of the boot sector.
     *
     * @param sectNr the sector that contains a boot sector copy
     */
    public void setBootSectorCopySector(int sectNr) {
        if (getBootSectorCopySector() == sectNr) return;
        if (sectNr < 0) throw new IllegalArgumentException(
                "boot sector copy sector must be >= 0");
        
        set16(BOOT_SECTOR_COPY_OFFSET, sectNr);
    }
    
    /**
     * Returns the sector that contains a copy of the boot sector, or 0 if
     * there is no copy.
     *
     * @return the sector number of the boot sector copy
     */
    public int getBootSectorCopySector() {
        return get16(BOOT_SECTOR_COPY_OFFSET);
    }

    /**
     * Sets the 11-byte volume label stored at offset 0x47.
     *
     * @param label the new volume label, may be {@code null}
     */
    public void setVolumeLabel(String label) {
        for (int i=0; i < 11; i++) {
            final byte c =
                    (label == null) ? 0 :
                    (label.length() > i) ? (byte) label.charAt(i) : 0x20;

            set8(0x47 + i, c);
        }
    }

    public int getFsInfoSectorNr() {
        return get16(FS_INFO_SECTOR_OFFSET);
    }

    public void setFsInfoSectorNr(int offset) {
        if (getFsInfoSectorNr() == offset) return;

        set16(FS_INFO_SECTOR_OFFSET, offset);
    }
    
    @Override
    public void setSectorsPerFat(long v) {
        if (getSectorsPerFat() == v) return;
        
        set32(SECTORS_PER_FAT_OFFSET, v);
    }
    
    @Override
    public long getSectorsPerFat() {
        return get32(SECTORS_PER_FAT_OFFSET);
    }

    @Override
    public FatType getFatType() {
        return FatType.FAT32;
    }

    @Override
    public void setSectorCount(long count) {
        super.setNrTotalSectors(count);
    }

    @Override
    public long getSectorCount() {
        return super.getNrTotalSectors();
    }

    /**
     * This is always 0 for FAT32.
     *
     * @return always 0
     */
    @Override
    public int getRootDirEntryCount() {
        return 0;
    }
    
    public void setFileSystemId(int id) {
        super.set32(0x43, id);
    }

    public int getFileSystemId() {
        return (int) super.get32(0x43);
    }

    /**
     * Writes a copy of this boot sector to the specified device, if a copy
     * is requested.
     *
     * @param device the device to write the boot sector copy to
     * @throws IOException on write error
     * @see #getBootSectorCopySector() 
     */
    public void writeCopy(BlockDevice device) throws IOException {
        if (getBootSectorCopySector() > 0) {
            final long offset = getBootSectorCopySector() * SIZE;
            buffer.rewind();
            buffer.limit(buffer.capacity());
            device.write(offset, buffer);
        }
    }

    @Override
    public int getFileSystemTypeLabelOffset() {
        return FILE_SYSTEM_TYPE_OFFSET;
    }

    @Override
    public int getExtendedBootSignatureOffset() {
        return EXTENDED_BOOT_SIGNATURE_OFFSET;
    }
}
