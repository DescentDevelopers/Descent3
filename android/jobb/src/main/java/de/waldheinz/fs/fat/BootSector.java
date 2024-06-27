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

import de.waldheinz.fs.BlockDevice;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * The boot sector.
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public abstract class BootSector extends Sector {

    /**
     * Offset to the byte specifying the number of FATs.
     *
     * @see #getNrFats()
     * @see #setNrFats(int) 
     */
    public static final int FAT_COUNT_OFFSET = 16;
    public static final int RESERVED_SECTORS_OFFSET = 14;
    
    public static final int TOTAL_SECTORS_16_OFFSET = 19;
    public static final int TOTAL_SECTORS_32_OFFSET = 32;

    /**
     * The length of the file system type string.
     *
     * @see #getFileSystemType()
     */
    public static final int FILE_SYSTEM_TYPE_LENGTH = 8;

    /**
     * The offset to the sectors per cluster value stored in a boot sector.
     * 
     * @see #getSectorsPerCluster()
     * @see #setSectorsPerCluster(int)
     */
    public static final int SECTORS_PER_CLUSTER_OFFSET = 0x0d;
    
    public static final int EXTENDED_BOOT_SIGNATURE = 0x29;

    /**
     * The size of a boot sector in bytes.
     */
    public final static int SIZE = 512;
    
    protected BootSector(BlockDevice device) {
        super(device, 0, SIZE);
        markDirty();
    }
    
    public static BootSector read(BlockDevice device) throws IOException {
        final ByteBuffer bb = ByteBuffer.allocate(512);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        device.read(0, bb);
        
        if ((bb.get(510) & 0xff) != 0x55 ||
                (bb.get(511) & 0xff) != 0xaa) throw new IOException(
                "missing boot sector signature");
                
        final byte sectorsPerCluster = bb.get(SECTORS_PER_CLUSTER_OFFSET);

        if (sectorsPerCluster <= 0) throw new IOException(
                "suspicious sectors per cluster count " + sectorsPerCluster);
                
        final int rootDirEntries = bb.getShort(
                Fat16BootSector.ROOT_DIR_ENTRIES_OFFSET);
        final int rootDirSectors = ((rootDirEntries * 32) +
                (device.getSectorSize() - 1)) / device.getSectorSize();

        final int total16 =
                bb.getShort(TOTAL_SECTORS_16_OFFSET) & 0xffff;
        final long total32 =
                bb.getInt(TOTAL_SECTORS_32_OFFSET) & 0xffffffffl;
        
        final long totalSectors = total16 == 0 ? total32 : total16;
        
        final int fatSz16 =
                bb.getShort(Fat16BootSector.SECTORS_PER_FAT_OFFSET)  & 0xffff;
        final long fatSz32 =
                bb.getInt(Fat32BootSector.SECTORS_PER_FAT_OFFSET) & 0xffffffffl;
                
        final long fatSz = fatSz16 == 0 ? fatSz32 : fatSz16;
        final int reservedSectors = bb.getShort(RESERVED_SECTORS_OFFSET);
        final int fatCount = bb.get(FAT_COUNT_OFFSET);
        final long dataSectors = totalSectors - (reservedSectors +
                (fatCount * fatSz) + rootDirSectors);
                
        final long clusterCount = dataSectors / sectorsPerCluster;
        
        final BootSector result =
                (clusterCount > Fat16BootSector.MAX_FAT16_CLUSTERS) ?
            new Fat32BootSector(device) : new Fat16BootSector(device);
            
        result.read();
        return result;
    }
    
    public abstract FatType getFatType();
    
    /**
     * Gets the number of sectors per FAT.
     * 
     * @return the sectors per FAT
     */
    public abstract long getSectorsPerFat();
    
    /**
     * Sets the number of sectors/fat
     * 
     * @param v  the new number of sectors per fat
     */
    public abstract void setSectorsPerFat(long v);

    public abstract void setSectorCount(long count);

    public abstract int getRootDirEntryCount();
    
    public abstract long getSectorCount();

    /**
     * Returns the offset to the file system type label, as this differs
     * between FAT12/16 and FAT32.
     *
     * @return the offset to the file system type label
     */
    public abstract int getFileSystemTypeLabelOffset();
    
    public abstract int getExtendedBootSignatureOffset();
    
    public void init() throws IOException {
        setBytesPerSector(getDevice().getSectorSize());
        setSectorCount(getDevice().getSize() / getDevice().getSectorSize());
        set8(getExtendedBootSignatureOffset(), EXTENDED_BOOT_SIGNATURE);

        /* magic bytes needed by some windows versions to recognize a boot
         * sector. these are x86 jump instructions which lead into
         * nirvana when executed, but we're currently unable to produce really
         * bootable images anyway. So... */
        set8(0x00, 0xeb);
        set8(0x01, 0x3c);
        set8(0x02, 0x90);
        
        /* the boot sector signature */
        set8(0x1fe, 0x55);
        set8(0x1ff, 0xaa);
    }
    
    /**
     * Returns the file system type label string.
     *
     * @return the file system type string
     * @see #setFileSystemTypeLabel(java.lang.String)
     * @see #getFileSystemTypeLabelOffset() 
     * @see #FILE_SYSTEM_TYPE_LENGTH
     */
    public String getFileSystemTypeLabel() {
        final StringBuilder sb = new StringBuilder(FILE_SYSTEM_TYPE_LENGTH);

        for (int i=0; i < FILE_SYSTEM_TYPE_LENGTH; i++) {
            sb.append ((char) get8(getFileSystemTypeLabelOffset() + i));
        }

        return sb.toString();
    }

    /**
     * 
     *
     * @param fsType the
     * @throws IllegalArgumentException if the length of the specified string
     *      does not equal {@link #FILE_SYSTEM_TYPE_LENGTH}
     */
    public void setFileSystemTypeLabel(String fsType)
            throws IllegalArgumentException {

        if (fsType.length() != FILE_SYSTEM_TYPE_LENGTH) {
            throw new IllegalArgumentException();
        }

        for (int i=0; i < FILE_SYSTEM_TYPE_LENGTH; i++) {
            set8(getFileSystemTypeLabelOffset() + i, fsType.charAt(i));
        }
    }

    /**
     * Returns the number of clusters that are really needed to cover the
     * data-caontaining portion of the file system.
     *
     * @return the number of clusters usable for user data
     * @see #getDataSize() 
     */
    public final long getDataClusterCount() {
        return getDataSize() / getBytesPerCluster();
    }

    /**
     * Returns the size of the data-containing portion of the file system.
     *
     * @return the number of bytes usable for storing user data
     */
    private long getDataSize() {
        return (getSectorCount() * getBytesPerSector()) -
                FatUtils.getFilesOffset(this);
    }

    /**
     * Gets the OEM name
     * 
     * @return String
     */
    public String getOemName() {
        StringBuilder b = new StringBuilder(8);
        
        for (int i = 0; i < 8; i++) {
            int v = get8(0x3 + i);
            if (v == 0) break;
            b.append((char) v);
        }
        
        return b.toString();
    }


    /**
     * Sets the OEM name, must be at most 8 characters long.
     *
     * @param name the new OEM name
     */
    public void setOemName(String name) {
        if (name.length() > 8) throw new IllegalArgumentException(
                "only 8 characters are allowed");

        for (int i = 0; i < 8; i++) {
            char ch;
            if (i < name.length()) {
                ch = name.charAt(i);
            } else {
                ch = (char) 0;
            }

            set8(0x3 + i, ch);
        }
    }
    
    /**
     * Gets the number of bytes/sector
     * 
     * @return int
     */
    public int getBytesPerSector() {
        return get16(0x0b);
    }

    /**
     * Sets the number of bytes/sector
     * 
     * @param v the new value for bytes per sector
     */
    public void setBytesPerSector(int v) {
        if (v == getBytesPerSector()) return;

        switch (v) {
            case 512: case 1024: case 2048: case 4096:
                set16(0x0b, v);
                break;
                
            default:
                throw new IllegalArgumentException();
        }
    }

    private static boolean isPowerOfTwo(int n) {
        return ((n!=0) && (n&(n-1))==0);
    }

    /**
     * Returns the number of bytes per cluster, which is calculated from the
     * {@link #getSectorsPerCluster() sectors per cluster} and the
     * {@link #getBytesPerSector() bytes per sector}.
     *
     * @return the number of bytes per cluster
     */
    public int getBytesPerCluster() {
        return this.getSectorsPerCluster() * this.getBytesPerSector();
    }

    /**
     * Gets the number of sectors/cluster
     * 
     * @return int
     */
    public int getSectorsPerCluster() {
        return get8(SECTORS_PER_CLUSTER_OFFSET);
    }

    /**
     * Sets the number of sectors/cluster
     *
     * @param v the new number of sectors per cluster
     */
    public void setSectorsPerCluster(int v) {
        if (v == getSectorsPerCluster()) return;
        if (!isPowerOfTwo(v)) throw new IllegalArgumentException(
                "value must be a power of two");
        
        set8(SECTORS_PER_CLUSTER_OFFSET, v);
    }
    
    /**
     * Gets the number of reserved (for bootrecord) sectors
     * 
     * @return int
     */
    public int getNrReservedSectors() {
        return get16(RESERVED_SECTORS_OFFSET);
    }

    /**
     * Sets the number of reserved (for bootrecord) sectors
     * 
     * @param v the new number of reserved sectors
     */
    public void setNrReservedSectors(int v) {
        if (v == getNrReservedSectors()) return;
        if (v < 1) throw new IllegalArgumentException(
                "there must be >= 1 reserved sectors");
        set16(RESERVED_SECTORS_OFFSET, v);
    }

    /**
     * Gets the number of fats
     * 
     * @return int
     */
    public final int getNrFats() {
        return get8(FAT_COUNT_OFFSET);
    }

    /**
     * Sets the number of fats
     *
     * @param v the new number of fats
     */
    public final void setNrFats(int v) {
        if (v == getNrFats()) return;
        
        set8(FAT_COUNT_OFFSET, v);
    }
    
    /**
     * Gets the number of logical sectors
     * 
     * @return int
     */
    protected int getNrLogicalSectors() {
        return get16(TOTAL_SECTORS_16_OFFSET);
    }
    
    /**
     * Sets the number of logical sectors
     * 
     * @param v the new number of logical sectors
     */
    protected void setNrLogicalSectors(int v) {
        if (v == getNrLogicalSectors()) return;
        
        set16(TOTAL_SECTORS_16_OFFSET, v);
    }
    
    protected void setNrTotalSectors(long v) {
        set32(TOTAL_SECTORS_32_OFFSET, v);
    }
    
    protected long getNrTotalSectors() {
        return get32(TOTAL_SECTORS_32_OFFSET);
    }
    
    /**
     * Gets the medium descriptor byte
     * 
     * @return int
     */
    public int getMediumDescriptor() {
        return get8(0x15);
    }

    /**
     * Sets the medium descriptor byte
     * 
     * @param v the new medium descriptor
     */
    public void setMediumDescriptor(int v) {
        set8(0x15, v);
    }
    
    /**
     * Gets the number of sectors/track
     * 
     * @return int
     */
    public int getSectorsPerTrack() {
        return get16(0x18);
    }

    /**
     * Sets the number of sectors/track
     *
     * @param v the new number of sectors per track
     */
    public void setSectorsPerTrack(int v) {
        if (v == getSectorsPerTrack()) return;
        
        set16(0x18, v);
    }

    /**
     * Gets the number of heads
     * 
     * @return int
     */
    public int getNrHeads() {
        return get16(0x1a);
    }

    /**
     * Sets the number of heads
     * 
     * @param v the new number of heads
     */
    public void setNrHeads(int v) {
        if (v == getNrHeads()) return;
        
        set16(0x1a, v);
    }

    /**
     * Gets the number of hidden sectors
     * 
     * @return int
     */
    public long getNrHiddenSectors() {
        return get32(0x1c);
    }

    /**
     * Sets the number of hidden sectors
     *
     * @param v the new number of hidden sectors
     */
    public void setNrHiddenSectors(long v) {
        if (v == getNrHiddenSectors()) return;
        
        set32(0x1c, v);
    }
    
    @Override
    public String toString() {
        StringBuilder res = new StringBuilder(1024);
        res.append("Bootsector :\n");
        res.append("oemName=");
        res.append(getOemName());
        res.append('\n');
        res.append("medium descriptor = ");
        res.append(getMediumDescriptor());
        res.append('\n');
        res.append("Nr heads = ");
        res.append(getNrHeads());
        res.append('\n');
        res.append("Sectors per track = ");
        res.append(getSectorsPerTrack());
        res.append('\n');
        res.append("Sector per cluster = ");
        res.append(getSectorsPerCluster());
        res.append('\n');
        res.append("byte per sector = ");
        res.append(getBytesPerSector());
        res.append('\n');
        res.append("Nr fats = ");
        res.append(getNrFats());
        res.append('\n');
        res.append("Nr hidden sectors = ");
        res.append(getNrHiddenSectors());
        res.append('\n');
        res.append("Nr logical sectors = ");
        res.append(getNrLogicalSectors());
        res.append('\n');
        res.append("Nr reserved sector = ");
        res.append(getNrReservedSectors());
        res.append('\n');
        
        return res.toString();
    }
    
}
