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
import java.util.Arrays;

/**
 * 
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public final class Fat {

    /**
     * The first cluster that really holds user data in a FAT.
     */
    public final static int FIRST_CLUSTER = 2;
    
    private final long[] entries;
    private final FatType fatType;
    private final int sectorCount;
    private final int sectorSize;
    private final BlockDevice device;
    private final BootSector bs;
    private final long offset;
    private final int lastClusterIndex;
    
    private int lastAllocatedCluster;

    /**
     * Reads a {@code Fat} as specified by a {@code BootSector}.
     *
     * @param bs the boot sector specifying the {@code Fat} layout
     * @param fatNr the number of the {@code Fat} to read
     * @return the {@code Fat} that was read
     * @throws IOException on read error
     * @throws IllegalArgumentException if {@code fatNr} is greater than
     *      {@link BootSector#getNrFats()}
     */
    public static Fat read(BootSector bs, int fatNr)
            throws IOException, IllegalArgumentException {
        
        if (fatNr > bs.getNrFats()) {
            throw new IllegalArgumentException(
                    "boot sector says there are only " + bs.getNrFats() +
                    " FATs when reading FAT #" + fatNr);
        }
        
        final long fatOffset = FatUtils.getFatOffset(bs, fatNr);
        final Fat result = new Fat(bs, fatOffset);
        result.read();
        return result;
    }
    
    /**
     * Creates a new {@code Fat} as specified by a {@code BootSector}.
     *
     * @param bs the boot sector specifying the {@code Fat} layout
     * @param fatNr the number of the {@code Fat} to create
     * @return the {@code Fat} that was created
     * @throws IOException on write error
     * @throws IllegalArgumentException if {@code fatNr} is greater than
     *      {@link BootSector#getNrFats()}
     */
    public static Fat create(BootSector bs, int fatNr)
            throws IOException, IllegalArgumentException {
        
        if (fatNr > bs.getNrFats()) {
            throw new IllegalArgumentException(
                    "boot sector says there are only " + bs.getNrFats() +
                    " FATs when creating FAT #" + fatNr);
        }
        
        final long fatOffset = FatUtils.getFatOffset(bs, fatNr);
        final Fat result = new Fat(bs, fatOffset);

        if (bs.getDataClusterCount() > result.entries.length)
            throw new IOException("FAT too small for device");
            
        result.init(bs.getMediumDescriptor());
        result.write();
        return result;
    }
    
    private Fat(BootSector bs, long offset) throws IOException {
        this.bs = bs;
        this.fatType = bs.getFatType();
        if (bs.getSectorsPerFat() > Integer.MAX_VALUE)
            throw new IllegalArgumentException("FAT too large");

        if (bs.getSectorsPerFat() <= 0) throw new IOException(
                "boot sector says there are " + bs.getSectorsPerFat() +
                " sectors per FAT");

        if (bs.getBytesPerSector() <= 0) throw new IOException(
                "boot sector says there are " + bs.getBytesPerSector() +
                " bytes per sector");

        this.sectorCount = (int) bs.getSectorsPerFat();
        this.sectorSize = bs.getBytesPerSector();
        this.device = bs.getDevice();
        this.offset = offset;
        this.lastAllocatedCluster = FIRST_CLUSTER;
        
        if (bs.getDataClusterCount() > Integer.MAX_VALUE) throw
                new IOException("too many data clusters");
        
        if (bs.getDataClusterCount() == 0) throw
                new IOException("no data clusters");
        
        this.lastClusterIndex = (int) bs.getDataClusterCount() + FIRST_CLUSTER;

        entries = new long[(int) ((sectorCount * sectorSize) /
                fatType.getEntrySize())];
                
        if (lastClusterIndex > entries.length) throw new IOException(
            "file system has " + lastClusterIndex +
            "clusters but only " + entries.length + " FAT entries");
    }
    
    public FatType getFatType() {
        return fatType;
    }
    
    /**
     * Returns the {@code BootSector} that specifies this {@code Fat}.
     *
     * @return this {@code Fat}'s {@code BootSector}
     */
    public BootSector getBootSector() {
        return this.bs;
    }

    /**
     * Returns the {@code BlockDevice} where this {@code Fat} is stored.
     *
     * @return the device holding this FAT
     */
    public BlockDevice getDevice() {
        return device;
    }
    
    private void init(int mediumDescriptor) {
        entries[0] = 
                (mediumDescriptor & 0xFF) |
                (0xFFFFF00L & fatType.getBitMask());
        entries[1] = fatType.getEofMarker();
    }
    
    /**
     * Read the contents of this FAT from the given device at the given offset.
     * 
     * @param offset the byte offset where to read the FAT from the device
     * @throws IOException on read error
     */
    private void read() throws IOException {
        final byte[] data = new byte[sectorCount * sectorSize];
        device.read(offset, ByteBuffer.wrap(data));

        for (int i = 0; i < entries.length; i++)
            entries[i] = fatType.readEntry(data, i);
    }
    
    public void write() throws IOException {
        this.writeCopy(offset);
    }
    
    /**
     * Write the contents of this FAT to the given device at the given offset.
     * 
     * @param offset the device offset where to write the FAT copy
     * @throws IOException on write error
     */
    public void writeCopy(long offset) throws IOException {
        final byte[] data = new byte[sectorCount * sectorSize];
        
        for (int index = 0; index < entries.length; index++) {
            fatType.writeEntry(data, index, entries[index]);
        }
        
        device.write(offset, ByteBuffer.wrap(data));
    }
    
    /**
     * Gets the medium descriptor byte
     * 
     * @return int
     */
    public int getMediumDescriptor() {
        return (int) (entries[0] & 0xFF);
    }
    
    /**
     * Gets the entry at a given offset
     * 
     * @param index
     * @return long
     */
    public long getEntry(int index) {
        return entries[index];
    }

    /**
     * Returns the last free cluster that was accessed in this FAT.
     *
     * @return the last seen free cluster
     */
    public int getLastFreeCluster() {
        return this.lastAllocatedCluster;
    }
    
    public long[] getChain(long startCluster) {
        testCluster(startCluster);
        // Count the chain first
        int count = 1;
        long cluster = startCluster;
        while (!isEofCluster(entries[(int) cluster])) {
            count++;
            cluster = entries[(int) cluster];
        }
        // Now create the chain
        long[] chain = new long[count];
        chain[0] = startCluster;
        cluster = startCluster;
        int i = 0;
        while (!isEofCluster(entries[(int) cluster])) {
            cluster = entries[(int) cluster];
            chain[++i] = cluster;
        }
        return chain;
    }

    /**
     * Gets the cluster after the given cluster
     * 
     * @param cluster
     * @return long The next cluster number or -1 which means eof.
     */
    public long getNextCluster(long cluster) {
        testCluster(cluster);
        long entry = entries[(int) cluster];
        if (isEofCluster(entry)) {
            return -1;
        } else {
            return entry;
        }
    }

    /**
     * Allocate a cluster for a new file
     * 
     * @return long the number of the newly allocated cluster
     * @throws IOException if there are no free clusters
     */
    public long allocNew() throws IOException {

        int i;
        int entryIndex = -1;

        for (i = lastAllocatedCluster; i < lastClusterIndex; i++) {
            if (isFreeCluster(i)) {
                entryIndex = i;
                break;
            }
        }
        
        if (entryIndex < 0) {
            for (i = FIRST_CLUSTER; i < lastAllocatedCluster; i++) {
                if (isFreeCluster(i)) {
                    entryIndex = i;
                    break;
                }
            }
        }
        
        if (entryIndex < 0) {
            throw new IOException(
                    "FAT Full (" + (lastClusterIndex - FIRST_CLUSTER)
                    + ", " + i + ")"); //NOI18N
        }
        
        entries[entryIndex] = fatType.getEofMarker();
        lastAllocatedCluster = entryIndex % lastClusterIndex;
        if (lastAllocatedCluster < FIRST_CLUSTER)
            lastAllocatedCluster = FIRST_CLUSTER;
        
        return entryIndex;
    }
    
    /**
     * Returns the number of clusters that are currently not in use by this FAT.
     * This estimate does only account for clusters that are really available in
     * the data portion of the file system, not for clusters that might only
     * theoretically be stored in the {@code Fat}.
     *
     * @return the free cluster count
     * @see FsInfoSector#setFreeClusterCount(long)
     * @see FsInfoSector#getFreeClusterCount()
     * @see BootSector#getDataClusterCount() 
     */
    public int getFreeClusterCount() {
        int result = 0;

        for (int i=FIRST_CLUSTER; i < lastClusterIndex; i++) {
            if (isFreeCluster(i)) result++;
        }

        return result;
    }

    /**
     * Returns the cluster number that was last allocated in this fat.
     *
     * @return
     */
    public int getLastAllocatedCluster() {
        return this.lastAllocatedCluster;
    }
    
    /**
     * Allocate a series of clusters for a new file.
     * 
     * @param nrClusters when number of clusters to allocate
     * @return long
     * @throws IOException if there are no free clusters
     */
    public long[] allocNew(int nrClusters) throws IOException {
        final long rc[] = new long[nrClusters];
        
        rc[0] = allocNew();
        for (int i = 1; i < nrClusters; i++) {
            rc[i] = allocAppend(rc[i - 1]);
        }
        
        return rc;
    }
    
    /**
     * Allocate a cluster to append to a new file
     * 
     * @param cluster a cluster from a chain where the new cluster should be
     *      appended
     * @return long the newly allocated and appended cluster number
     * @throws IOException if there are no free clusters
     */
    public long allocAppend(long cluster)
            throws IOException {
        
        testCluster(cluster);
        
        while (!isEofCluster(entries[(int) cluster])) {
            cluster = entries[(int) cluster];
        }
        
        long newCluster = allocNew();
        entries[(int) cluster] = newCluster;

        return newCluster;
    }

    public void setEof(long cluster) {
        testCluster(cluster);
        entries[(int) cluster] = fatType.getEofMarker();
    }

    public void setFree(long cluster) {
        testCluster(cluster);
        entries[(int) cluster] = 0;
    }
    
    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof Fat)) return false;
        
        final Fat other = (Fat) obj;
        if (this.fatType != other.fatType) return false;
        if (this.sectorCount != other.sectorCount) return false;
        if (this.sectorSize != other.sectorSize) return false;
        if (this.lastClusterIndex != other.lastClusterIndex) return false;
        if (!Arrays.equals(this.entries, other.entries)) return false;
        if (this.getMediumDescriptor() != other.getMediumDescriptor())
            return false;

        return true;
    }
    
    @Override
    public int hashCode() {
        int hash = 7;
        hash = 23 * hash + Arrays.hashCode(this.entries);
        hash = 23 * hash + this.fatType.hashCode();
        hash = 23 * hash + this.sectorCount;
        hash = 23 * hash + this.sectorSize;
        hash = 23 * hash + this.lastClusterIndex;
        return hash;
    }
    
    /**
     * Is the given entry a free cluster?
     *
     * @param entry
     * @return boolean
     */
    protected boolean isFreeCluster(long entry) {
        if (entry > Integer.MAX_VALUE) throw new IllegalArgumentException();
        return (entries[(int) entry] == 0);
    }
    
    /**
     * Is the given entry a reserved cluster?
     *
     * @param entry
     * @return boolean
     */
    protected boolean isReservedCluster(long entry) {
        return fatType.isReservedCluster(entry);
    }

    /**
     * Is the given entry an EOF marker
     *
     * @param entry
     * @return boolean
     */
    protected boolean isEofCluster(long entry) {
        return fatType.isEofCluster(entry);
    }
    
    protected void testCluster(long cluster) throws IllegalArgumentException {
        if ((cluster < FIRST_CLUSTER) || (cluster >= entries.length)) {
            throw new IllegalArgumentException(
                    "invalid cluster value " + cluster);
        }
    }
    
    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder();

        sb.append(this.getClass().getSimpleName());
        sb.append("[type=");
        sb.append(fatType);
        sb.append(", mediumDescriptor=0x");
        sb.append(Integer.toHexString(getMediumDescriptor()));
        sb.append(", sectorCount=");
        sb.append(sectorCount);
        sb.append(", sectorSize=");
        sb.append(sectorSize);
        sb.append(", freeClusters=");
        sb.append(getFreeClusterCount());
        sb.append("]");
        
        return sb.toString();
    }
    
}
