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

import de.waldheinz.fs.AbstractFsObject;
import de.waldheinz.fs.BlockDevice;
import java.io.EOFException;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * A chain of clusters as stored in a {@link Fat}.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
final class ClusterChain extends AbstractFsObject {
    protected final Fat fat;
    private final BlockDevice device;
    private final int clusterSize;
    protected final long dataOffset;
    
    private long startCluster;
    
    /**
     * Creates a new {@code ClusterChain} that contains no clusters.
     *
     * @param fat the {@code Fat} that holds the new chain
     * @param readOnly if the chain should be created read-only
     */
    public ClusterChain(Fat fat, boolean readOnly) {
        this(fat, 0, readOnly);
    }
    
    public ClusterChain(Fat fat, long startCluster, boolean readOnly) {
        super(readOnly);
        
        this.fat = fat;
        
        if (startCluster != 0) {
            this.fat.testCluster(startCluster);
            
            if (this.fat.isFreeCluster(startCluster))
                throw new IllegalArgumentException(
                    "cluster " + startCluster + " is free");
        }
        
        this.device = fat.getDevice();
        this.dataOffset = FatUtils.getFilesOffset(fat.getBootSector());
        this.startCluster = startCluster;
        this.clusterSize = fat.getBootSector().getBytesPerCluster();
    }

    public int getClusterSize() {
        return clusterSize;
    }
    
    public Fat getFat() {
        return fat;
    }

    public BlockDevice getDevice() {
        return device;
    }

    /**
     * Returns the first cluster of this chain.
     *
     * @return the chain's first cluster, which may be 0 if this chain does
     *      not contain any clusters
     */
    public long getStartCluster() {
        return startCluster;
    }
    
    /**
     * Calculates the device offset (0-based) for the given cluster and offset
     * within the cluster.
     * 
     * @param cluster
     * @param clusterOffset
     * @return long
     * @throws FileSystemException
     */
    private long getDevOffset(long cluster, int clusterOffset) {
        return dataOffset + clusterOffset +
                ((cluster - Fat.FIRST_CLUSTER) * clusterSize);
    }

    /**
     * Returns the size this {@code ClusterChain} occupies on the device.
     *
     * @return the size this chain occupies on the device in bytes
     */
    public long getLengthOnDisk() {
        if (getStartCluster() == 0) return 0;
        
        return getChainLength() * clusterSize;
    }
    
    /**
     * Sets the length of this {@code ClusterChain} in bytes. Because a
     * {@code ClusterChain} can only contain full clusters, the new size
     * will always be a multiple of the cluster size.
     *
     * @param size the desired number of bytes the can be stored in
     *      this {@code ClusterChain}
     * @return the true number of bytes this {@code ClusterChain} can contain
     * @throws IOException on error setting the new size
     * @see #setChainLength(int) 
     */
    public long setSize(long size) throws IOException {
        final long nrClusters = ((size + clusterSize - 1) / clusterSize);
        if (nrClusters > Integer.MAX_VALUE)
            throw new IOException("too many clusters");

        setChainLength((int) nrClusters);
        
        return clusterSize * nrClusters;
    }

    /**
     * Determines the length of this {@code ClusterChain} in clusters.
     *
     * @return the length of this chain
     */
    public int getChainLength() {
        if (getStartCluster() == 0) return 0;
        
        final long[] chain = getFat().getChain(getStartCluster());
        return chain.length;
    }

    /**
     * Sets the length of this cluster chain in clusters.
     *
     * @param nrClusters the new number of clusters this chain should contain,
     *      must be {@code >= 0}
     * @throws IOException on error updating the chain length
     * @see #setSize(long) 
     */
    public void setChainLength(int nrClusters) throws IOException {
        if (nrClusters < 0) throw new IllegalArgumentException(
                "negative cluster count"); //NOI18N
                
        if ((this.startCluster == 0) && (nrClusters == 0)) {
            /* nothing to do */
        } else if ((this.startCluster == 0) && (nrClusters > 0)) {
            final long[] chain = fat.allocNew(nrClusters);
            this.startCluster = chain[0];
        } else {
            final long[] chain = fat.getChain(startCluster);
            
            if (nrClusters != chain.length) {
                if (nrClusters > chain.length) {
                    /* grow the chain */
                    int count = nrClusters - chain.length;
                    
                    while (count > 0) {
                        fat.allocAppend(getStartCluster());
                        count--;
                    }
                } else {
                    /* shrink the chain */
                    if (nrClusters > 0) {
                        fat.setEof(chain[nrClusters - 1]);
                        for (int i = nrClusters; i < chain.length; i++) {
                            fat.setFree(chain[i]);
                        }
                    } else {
                        for (int i=0; i < chain.length; i++) {
                            fat.setFree(chain[i]);
                        }
                        
                        this.startCluster = 0;
                    }
                }
            }
        }
    }
    
    public void readData(long offset, ByteBuffer dest)
            throws IOException {

        int len = dest.remaining();

        if ((startCluster == 0 && len > 0)) throw new EOFException();
        
        final long[] chain = getFat().getChain(startCluster);
        final BlockDevice dev = getDevice();

        int chainIdx = (int) (offset / clusterSize);
        if (offset % clusterSize != 0) {
            int clusOfs = (int) (offset % clusterSize);
            int size = Math.min(len,
                    (int) (clusterSize - (offset % clusterSize) - 1));
            dest.limit(dest.position() + size);

            dev.read(getDevOffset(chain[chainIdx], clusOfs), dest);
            
            offset += size;
            len -= size;
            chainIdx++;
        }

        while (len > 0) {
            int size = Math.min(clusterSize, len);
            dest.limit(dest.position() + size);

            dev.read(getDevOffset(chain[chainIdx], 0), dest);

            len -= size;
            chainIdx++;
        }
    }
    
    /**
     * Writes data to this cluster chain, possibly growing the chain so it
     * can store the additional data. When this method returns without throwing
     * an exception, the buffer's {@link ByteBuffer#position() position} will
     * equal it's {@link ByteBuffer#limit() limit}, and the limit will not
     * have changed. This is not guaranteed if writing fails.
     *
     * @param offset the offset where to write the first byte from the buffer
     * @param srcBuf the buffer to write to this {@code ClusterChain}
     * @throws IOException on write error
     */
    public void writeData(long offset, ByteBuffer srcBuf) throws IOException {
        
        int len = srcBuf.remaining();

        if (len == 0) return;

        final long minSize = offset + len;
        if (getLengthOnDisk() < minSize) {
            setSize(minSize);
        }
        
        final long[] chain = fat.getChain(getStartCluster());

        int chainIdx = (int) (offset / clusterSize);
        if (offset % clusterSize != 0) {
            int clusOfs = (int) (offset % clusterSize);
            int size = Math.min(len,
                    (int) (clusterSize - (offset % clusterSize)));
            srcBuf.limit(srcBuf.position() + size);
            
            device.write(getDevOffset(chain[chainIdx], clusOfs), srcBuf);
            
            offset += size;
            len -= size;
            chainIdx++;
        }
        
        while (len > 0) {
            int size = Math.min(clusterSize, len);
            srcBuf.limit(srcBuf.position() + size);

            device.write(getDevOffset(chain[chainIdx], 0), srcBuf);

            len -= size;
            chainIdx++;
        }
        
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) return false;
        if (!(obj instanceof ClusterChain)) return false;
        
        final ClusterChain other = (ClusterChain) obj;
        
        if (this.fat != other.fat &&
                (this.fat == null || !this.fat.equals(other.fat))) {

            return false;
        }
        
        if (this.startCluster != other.startCluster) {
            return false;
        }

        return true;
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 79 * hash +
                (this.fat != null ? this.fat.hashCode() : 0);
        hash = 79 * hash +
                (int) (this.startCluster ^ (this.startCluster >>> 32));
        return hash;
    }
    
}
