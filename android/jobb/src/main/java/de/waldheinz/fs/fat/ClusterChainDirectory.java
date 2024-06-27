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
import java.nio.ByteBuffer;

/**
 * A directory that is stored in a cluster chain.
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
class ClusterChainDirectory extends AbstractDirectory {

    /**
     * According to the FAT specification, this is the maximum size a FAT
     * directory may occupy on disk. The {@code ClusterChainDirectory} takes
     * care not to grow beyond this limit.
     *
     * @see #changeSize(int) 
     */
    public final static int MAX_SIZE = 65536 * 32;

    /**
     * The {@code ClusterChain} that stores this directory. Package-visible
     * for testing.
     */
    final ClusterChain chain;
    
    protected ClusterChainDirectory(ClusterChain chain, boolean isRoot) {
        
        super((int)(chain.getLengthOnDisk() / FatDirectoryEntry.SIZE),
                chain.isReadOnly(), isRoot);
        
        this.chain = chain;   
    }
    
    public static ClusterChainDirectory readRoot(
            ClusterChain chain) throws IOException {
        
        final ClusterChainDirectory result =
                new ClusterChainDirectory(chain, true);
        
        result.read();
        return result;
    }
    
    public static ClusterChainDirectory createRoot(Fat fat) throws IOException {

        if (fat.getFatType() != FatType.FAT32) {
            throw new IllegalArgumentException(
                    "only FAT32 stores root directory in a cluster chain");
        }

        final Fat32BootSector bs = (Fat32BootSector) fat.getBootSector();
        final ClusterChain cc = new ClusterChain(fat, false);
        cc.setChainLength(1);
        
        bs.setRootDirFirstCluster(cc.getStartCluster());
        
        final ClusterChainDirectory result =
                new ClusterChainDirectory(cc, true);
        
        result.flush();
        return result;
    }
    
    @Override
    protected final void read(ByteBuffer data) throws IOException {
        this.chain.readData(0, data);
    }

    @Override
    protected final void write(ByteBuffer data) throws IOException {
        final int toWrite = data.remaining();
        chain.writeData(0, data);
        final long trueSize = chain.getLengthOnDisk();
        
        /* TODO: check if the code below is really needed */
        if (trueSize > toWrite) {
            final int rest = (int) (trueSize - toWrite);
            final ByteBuffer fill = ByteBuffer.allocate(rest);
            chain.writeData(toWrite, fill);
        }
    }

    /**
     * Returns the first cluster of the chain that stores this directory for
     * non-root instances or 0 if this is the root directory.
     *
     * @return the first storage cluster of this directory
     * @see #isRoot() 
     */
    @Override
    protected final long getStorageCluster() {
        return isRoot() ? 0 : chain.getStartCluster();
    }
    
    public final void delete() throws IOException {
        chain.setChainLength(0);
    }
    
    @Override
    protected final void changeSize(int entryCount)
            throws IOException, IllegalArgumentException {

        assert (entryCount >= 0);

        final int size = entryCount * FatDirectoryEntry.SIZE;

        if (size > MAX_SIZE) throw new DirectoryFullException(
                "directory would grow beyond " + MAX_SIZE + " bytes",
                getCapacity(), entryCount);
        
        sizeChanged(chain.setSize(Math.max(size, chain.getClusterSize())));
    }
    
}
