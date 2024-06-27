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
 * The FAT32 File System Information Sector.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @see http://en.wikipedia.org/wiki/File_Allocation_Table#FS_Information_Sector
 */
final class FsInfoSector extends Sector {

    /**
     * The offset to the free cluster count value in the FS info sector.
     */
    public static final int FREE_CLUSTERS_OFFSET = 0x1e8;

    /**
     * The offset to the "last allocated cluster" value in this sector.
     */
    public static final int LAST_ALLOCATED_OFFSET = 0x1ec;

    /**
     * The offset to the signature of this sector.
     */
    public static final int SIGNATURE_OFFSET = 0x1fe;

    private FsInfoSector(BlockDevice device, long offset) {
        super(device, offset, BootSector.SIZE);
    }

    /**
     * Reads a {@code FsInfoSector} as specified by the given
     * {@code Fat32BootSector}.
     *
     * @param bs the boot sector that specifies where the FS info sector is
     *      stored
     * @return the FS info sector that was read
     * @throws IOException on read error
     * @see Fat32BootSector#getFsInfoSectorNr() 
     */
    public static FsInfoSector read(Fat32BootSector bs) throws IOException {
        final FsInfoSector result =
                new FsInfoSector(bs.getDevice(), offset(bs));
        
        result.read();
        result.verify();
        return result;
    }

    /**
     * Creates an new {@code FsInfoSector} where the specified
     * {@code Fat32BootSector} indicates it should be.
     *
     * @param bs the boot sector specifying the FS info sector storage
     * @return the FS info sector instance that was created
     * @throws IOException on write error
     * @see Fat32BootSector#getFsInfoSectorNr() 
     */
    public static FsInfoSector create(Fat32BootSector bs) throws IOException {
        final int offset = offset(bs);

        if (offset == 0) throw new IOException(
                "creating a FS info sector at offset 0 is strange");
        
        final FsInfoSector result =
                new FsInfoSector(bs.getDevice(), offset(bs));
        
        result.init();
        result.write();
        return result;
    }

    private static int offset(Fat32BootSector bs) {
        return bs.getFsInfoSectorNr() * bs.getBytesPerSector();
    }

    /**
     * Sets the number of free clusters on the file system stored at
     * {@link #FREE_CLUSTERS_OFFSET}.
     *
     * @param value the new free cluster count
     * @see Fat#getFreeClusterCount()
     */
    public void setFreeClusterCount(long value) {
        if (getFreeClusterCount() == value) return;
        
        set32(FREE_CLUSTERS_OFFSET, value);
    }
    
    /**
     * Returns the number of free clusters on the file system as sepcified by
     * the 32-bit value at {@link #FREE_CLUSTERS_OFFSET}.
     *
     * @return the number of free clusters
     * @see Fat#getFreeClusterCount() 
     */
    public long getFreeClusterCount() {
        return get32(FREE_CLUSTERS_OFFSET);
    }

    /**
     * Sets the last allocated cluster that was used in the {@link Fat}.
     *
     * @param value the FAT's last allocated cluster number
     * @see Fat#getLastAllocatedCluster() 
     */
    public void setLastAllocatedCluster(long value) {
        if (getLastAllocatedCluster() == value) return;
        
        super.set32(LAST_ALLOCATED_OFFSET, value);
    }

    /**
     * Returns the last allocated cluster number of the {@link Fat} of the
     * file system this FS info sector is part of.
     *
     * @return the last allocated cluster number
     * @see Fat#getLastAllocatedCluster() 
     */
    public long getLastAllocatedCluster() {
        return super.get32(LAST_ALLOCATED_OFFSET);
    }

    private void init() {
        buffer.position(0x00);
        buffer.put((byte) 0x52);
        buffer.put((byte) 0x52);
        buffer.put((byte) 0x61);
        buffer.put((byte) 0x41);
        
        /* 480 reserved bytes */

        buffer.position(0x1e4);
        buffer.put((byte) 0x72);
        buffer.put((byte) 0x72);
        buffer.put((byte) 0x41);
        buffer.put((byte) 0x61);
        
        setFreeClusterCount(-1);
        setLastAllocatedCluster(Fat.FIRST_CLUSTER);

        buffer.position(SIGNATURE_OFFSET);
        buffer.put((byte) 0x55);
        buffer.put((byte) 0xaa);
        
        markDirty();
    }

    private void verify() throws IOException {
        if (!(get8(SIGNATURE_OFFSET) == 0x55) ||
                !(get8(SIGNATURE_OFFSET + 1) == 0xaa)) {

            throw new IOException("invalid FS info sector signature");
        }
    }

    @Override
    public String toString() {
        return FsInfoSector.class.getSimpleName() +
                " [freeClusterCount=" + getFreeClusterCount() + //NOI18N
                ", lastAllocatedCluster=" + getLastAllocatedCluster() + //NOI18N
                ", offset=" + getOffset() + //NOI18N
                ", dirty=" + isDirty() + //NOI18N
                "]"; //NOI18N
    }
    
}
