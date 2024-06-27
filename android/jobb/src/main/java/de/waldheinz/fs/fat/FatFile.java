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
import java.io.IOException;
import de.waldheinz.fs.FsFile;
import de.waldheinz.fs.ReadOnlyException;
import java.io.EOFException;
import java.nio.ByteBuffer;

/**
 * The in-memory representation of a single file (chain of clusters) on a
 * FAT file system.
 * 
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public final class FatFile extends AbstractFsObject implements FsFile {
    private final FatDirectoryEntry entry;
    private final ClusterChain chain;
    
    private FatFile(FatDirectoryEntry myEntry, ClusterChain chain) {
        super(myEntry.isReadOnly());
        
        this.entry = myEntry;
        this.chain = chain;
    }
    
    static FatFile get(Fat fat, FatDirectoryEntry entry)
            throws IOException {
        
        if (entry.isDirectory())
            throw new IllegalArgumentException(entry + " is a directory");
            
        final ClusterChain cc = new ClusterChain(
                fat, entry.getStartCluster(), entry.isReadonlyFlag());
                
        if (entry.getLength() > cc.getLengthOnDisk()) throw new IOException(
                "entry is larger than associated cluster chain");
                
        return new FatFile(entry, cc);
    }
    
    /**
     * Returns the length of this file in bytes. This is the length that
     * is stored in the directory entry that is associated with this file.
     * 
     * @return long the length that is recorded for this file
     */
    @Override
    public long getLength() {
        checkValid();
        
        return entry.getLength();
    }
    
    /**
     * Sets the size (in bytes) of this file. Because
     * {@link #write(long, java.nio.ByteBuffer) writing} to the file will grow
     * it automatically if needed, this method is mainly usefull for truncating
     * a file. 
     *
     * @param length the new length of the file in bytes
     * @throws ReadOnlyException if this file is read-only
     * @throws IOException on error updating the file size
     */
    @Override
    public void setLength(long length) throws ReadOnlyException, IOException {
        checkWritable();
        
        if (getLength() == length) return;
        
        updateTimeStamps(true);
        chain.setSize(length);
        
        this.entry.setStartCluster(chain.getStartCluster());
        this.entry.setLength(length);
    }
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * Unless this file is {@link #isReadOnly() read-ony}, this method also
     * updates the "last accessed" field in the directory entry that is
     * associated with this file.
     * </p>
     * 
     * @param offset {@inheritDoc}
     * @param dest {@inheritDoc}
     * @see FatDirectoryEntry#setLastAccessed(long)
     */
    @Override
    public void read(long offset, ByteBuffer dest) throws IOException {
        checkValid();
        
        final int len = dest.remaining();
        
        if (len == 0) return;
        
        if (offset + len > getLength()) {
            throw new EOFException();
        }
        
        if (!isReadOnly()) {
            updateTimeStamps(false);
        }
        
        chain.readData(offset, dest);
    }

    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * If the data to be written extends beyond the current
     * {@link #getLength() length} of this file, an attempt is made to
     * {@link #setLength(long) grow} the file so that the data will fit.
     * Additionally, this method updates the "last accessed" and "last modified"
     * fields on the directory entry that is associated with this file.
     * </p>
     *
     * @param offset {@inheritDoc}
     * @param srcBuf {@inheritDoc}
     */
    @Override
    public void write(long offset, ByteBuffer srcBuf)
            throws ReadOnlyException, IOException {

        checkWritable();

        updateTimeStamps(true);
        
        final long lastByte = offset + srcBuf.remaining();

        if (lastByte > getLength()) {
            setLength(lastByte);
        }
        
        chain.writeData(offset, srcBuf);
    }
    
    private void updateTimeStamps(boolean write) {
        final long now = System.currentTimeMillis();
        entry.setLastAccessed(now);
        
        if (write) {
            entry.setLastModified(now);
        }
    }

    /**
     * Has no effect besides possibly throwing an {@code ReadOnlyException}. To
     * make sure that all data is written out to disk use the
     * {@link FatFileSystem#flush()} method.
     *
     * @throws ReadOnlyException if this {@code FatFile} is read-only
     */
    @Override
    public void flush() throws ReadOnlyException {
        checkWritable();
        
        /* nothing else to do */
    }
    
    /**
     * Returns the {@code ClusterChain} that holds the contents of
     * this {@code FatFile}.
     *
     * @return the file's {@code ClusterChain}
     */
    ClusterChain getChain() {
        checkValid();
        
        return chain;
    }
    
    /**
     * Returns a human-readable string representation of this {@code FatFile},
     * mainly for debugging purposes.
     *
     * @return a string describing this {@code FatFile}
     */
    @Override
    public String toString() {
        return getClass().getSimpleName() + " [length=" + getLength() +
                ", first cluster=" + chain.getStartCluster() + "]";
    }
    
}
