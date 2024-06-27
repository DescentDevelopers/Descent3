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
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * This is the abstract base class for all directory implementations.
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
abstract class AbstractDirectory {

    /**
     * The maximum length of the volume label.
     *
     * @see #setLabel(java.lang.String) 
     */
    public static final int MAX_LABEL_LENGTH = 11;
    
    private final List<FatDirectoryEntry> entries;
    private final boolean readOnly;
    private final boolean isRoot;
    
    private boolean dirty;
    private int capacity;
    private String volumeLabel;

    /**
     * Creates a new instance of {@code AbstractDirectory}.
     *
     * @param capacity the initial capacity of the new instance
     * @param readOnly if the instance should be read-only
     * @param isRoot if the new {@code AbstractDirectory} represents a root
     *      directory
     */
    protected AbstractDirectory(
            int capacity, boolean readOnly, boolean isRoot) {
        
        this.entries = new ArrayList<FatDirectoryEntry>();
        this.capacity = capacity;
        this.readOnly = readOnly;
        this.isRoot = isRoot;
    }

    /**
     * Gets called when the {@code AbstractDirectory} must read it's content
     * off the backing storage. This method must always fill the buffer's
     * remaining space with the bytes making up this directory, beginning with
     * the first byte.
     *
     * @param data the {@code ByteBuffer} to fill
     * @throws IOException on read error
     */
    protected abstract void read(ByteBuffer data) throws IOException;

    /**
     * Gets called when the {@code AbstractDirectory} wants to write it's
     * contents to the backing storage. This method is expected to write the
     * buffer's remaining data to the storage, beginning with the first byte.
     *
     * @param data the {@code ByteBuffer} to write
     * @throws IOException on write error
     */
    protected abstract void write(ByteBuffer data) throws IOException;

    /**
     * Returns the number of the cluster where this directory is stored. This
     * is important when creating the ".." entry in a sub-directory, as this
     * entry must poing to the storage cluster of it's parent.
     *
     * @return this directory's storage cluster
     */
    protected abstract long getStorageCluster();

    /**
     * Gets called by the {@code AbstractDirectory} when it has determined that
     * it should resize because the number of entries has changed.
     *
     * @param entryCount the new number of entries this directory needs to store
     * @throws IOException on write error
     * @throws DirectoryFullException if the FAT12/16 root directory is full
     * @see #sizeChanged(long)
     * @see #checkEntryCount(int) 
     */
    protected abstract void changeSize(int entryCount)
            throws DirectoryFullException, IOException;
            
    /**
     * Replaces all entries in this directory.
     *
     * @param newEntries the new directory entries
     */
    public void setEntries(List<FatDirectoryEntry> newEntries) {
        if (newEntries.size() > capacity)
            throw new IllegalArgumentException("too many entries");
        
        this.entries.clear();
        this.entries.addAll(newEntries);
    }
    
    /**
     * 
     *
     * @param newSize the new storage space for the directory in bytes
     * @see #changeSize(int) 
     */
    protected final void sizeChanged(long newSize) throws IOException {
        final long newCount = newSize / FatDirectoryEntry.SIZE;
        if (newCount > Integer.MAX_VALUE)
            throw new IOException("directory too large");
        
        this.capacity = (int) newCount;
    }

    public final FatDirectoryEntry getEntry(int idx) {
        return this.entries.get(idx);
    }
    
    /**
     * Returns the current capacity of this {@code AbstractDirectory}.
     *
     * @return the number of entries this directory can hold in its current
     *      storage space
     * @see #changeSize(int)
     */
    public final int getCapacity() {
        return this.capacity;
    }

    /**
     * The number of entries that are currently stored in this
     * {@code AbstractDirectory}.
     *
     * @return the current number of directory entries
     */
    public final int getEntryCount() {
        return this.entries.size();
    }
    
    public boolean isReadOnly() {
        return readOnly;
    }

    public final boolean isRoot() {
        return this.isRoot;
    }
    
    /**
     * Gets the number of directory entries in this directory. This is the
     * number of "real" entries in this directory, possibly plus one if a
     * volume label is set.
     * 
     * @return the number of entries in this directory
     */
    public int getSize() {
        return entries.size() + ((this.volumeLabel != null) ? 1 : 0);
    }
    
    /**
     * Mark this directory as dirty.
     */
    protected final void setDirty() {
        this.dirty = true;
    }

    /**
     * Checks if this {@code AbstractDirectory} is a root directory.
     *
     * @throws UnsupportedOperationException if this is not a root directory
     * @see #isRoot() 
     */
    private void checkRoot() throws UnsupportedOperationException {
        if (!isRoot()) {
            throw new UnsupportedOperationException(
                    "only supported on root directories");
        }
    }
    
    /**
     * Mark this directory as not dirty.
     */
    private void resetDirty() {
        this.dirty = false;
    }
    
    /**
     * Flush the contents of this directory to the persistent storage
     */
    public void flush() throws IOException {
        
        final ByteBuffer data = ByteBuffer.allocate(
                getCapacity() * FatDirectoryEntry.SIZE);
        
        for (int i=0; i < entries.size(); i++) {
            final FatDirectoryEntry entry = entries.get(i);
            
            if (entry != null) {
                entry.write(data);
            }
        }
        
        /* TODO: the label could be placed directly the dot entries */
        
        if (this.volumeLabel != null) {
            final FatDirectoryEntry labelEntry =
                    FatDirectoryEntry.createVolumeLabel(volumeLabel);

            labelEntry.write(data);
        }
        
        if (data.hasRemaining()) {
            FatDirectoryEntry.writeNullEntry(data);
        }

        data.flip();
        
        write(data);
        resetDirty();
    }
    
    protected final void read() throws IOException {
        final ByteBuffer data = ByteBuffer.allocate(
                getCapacity() * FatDirectoryEntry.SIZE);
                
        read(data);
        data.flip();
        
        for (int i=0; i < getCapacity(); i++) {
            final FatDirectoryEntry e =
                    FatDirectoryEntry.read(data, isReadOnly());
            
            if (e == null) break;
            
            if (e.isVolumeLabel()) {
                if (!this.isRoot) throw new IOException(
                        "volume label in non-root directory");
                
                this.volumeLabel = e.getVolumeLabel();
            } else {
                entries.add(e);
            }
        }
    }
    
    public void addEntry(FatDirectoryEntry e) throws IOException {
        assert (e != null);
        
        if (getSize() == getCapacity()) {
            changeSize(getCapacity() + 1);
        }

        entries.add(e);
    }
    
    public void addEntries(FatDirectoryEntry[] entries)
            throws IOException {
        
        if (getSize() + entries.length > getCapacity()) {
            changeSize(getSize() + entries.length);
        }

        this.entries.addAll(Arrays.asList(entries));
    }
    
    public void removeEntry(FatDirectoryEntry entry) throws IOException {
        assert (entry != null);
        
        this.entries.remove(entry);
        changeSize(getSize());
    }

    /**
     * Returns the volume label that is stored in this directory. Reading the
     * volume label is only supported for the root directory.
     *
     * @return the volume label stored in this directory, or {@code null}
     * @throws UnsupportedOperationException if this is not a root directory
     * @see #isRoot() 
     */
    public String getLabel() throws UnsupportedOperationException {
        checkRoot();
        
        return volumeLabel;
    }

    public FatDirectoryEntry createSub(Fat fat) throws IOException {
        final ClusterChain chain = new ClusterChain(fat, false);
        chain.setChainLength(1);

        final FatDirectoryEntry entry = FatDirectoryEntry.create(true);
        entry.setStartCluster(chain.getStartCluster());
        
        final ClusterChainDirectory dir =
                new ClusterChainDirectory(chain, false);

        /* add "." entry */

        final FatDirectoryEntry dot = FatDirectoryEntry.create(true);
        dot.setShortName(ShortName.DOT);
        dot.setStartCluster(dir.getStorageCluster());
        copyDateTimeFields(entry, dot);
        dir.addEntry(dot);

        /* add ".." entry */

        final FatDirectoryEntry dotDot = FatDirectoryEntry.create(true);
        dotDot.setShortName(ShortName.DOT_DOT);
        dotDot.setStartCluster(getStorageCluster());
        copyDateTimeFields(entry, dotDot);
        dir.addEntry(dotDot);

        dir.flush();

        return entry;
    }
    
    private static void copyDateTimeFields(
            FatDirectoryEntry src, FatDirectoryEntry dst) {
        
        dst.setCreated(src.getCreated());
        dst.setLastAccessed(src.getLastAccessed());
        dst.setLastModified(src.getLastModified());
    }

    /**
     * Sets the volume label that is stored in this directory. Setting the
     * volume label is supported on the root directory only.
     *
     * @param label the new volume label
     * @throws IllegalArgumentException if the label is too long
     * @throws UnsupportedOperationException if this is not a root directory
     * @see #isRoot() 
     */
    public void setLabel(String label) throws IllegalArgumentException,
            UnsupportedOperationException, IOException {

        checkRoot();

        if (label.length() > MAX_LABEL_LENGTH) throw new
                IllegalArgumentException("label too long");

        if (this.volumeLabel != null) {
            if (label == null) {
                changeSize(getSize() - 1);
                this.volumeLabel = null;
            } else {
                ShortName.checkValidChars(label.toCharArray());
                this.volumeLabel = label;
            }
        } else {
            if (label != null) {
                changeSize(getSize() + 1);
                ShortName.checkValidChars(label.toCharArray());
                this.volumeLabel = label;
            }
        }

        this.dirty = true;
    }
    
}
