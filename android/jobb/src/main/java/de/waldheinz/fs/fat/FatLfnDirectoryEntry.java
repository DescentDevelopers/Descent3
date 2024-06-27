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

import de.waldheinz.fs.AbstractFsObject;
import de.waldheinz.fs.FsDirectoryEntry;
import de.waldheinz.fs.ReadOnlyException;
import java.io.IOException;

/**
 * Represents an entry in a {@link FatLfnDirectory}. Besides implementing the
 * {@link FsDirectoryEntry} interface for FAT file systems, it allows access
 * to the {@link #setArchiveFlag(boolean) archive},
 * {@link #setHiddenFlag(boolean) hidden},
 * {@link #setReadOnlyFlag(boolean) read-only} and
 * {@link #setSystemFlag(boolean) system} flags specifed for the FAT file
 * system.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public final class FatLfnDirectoryEntry
        extends AbstractFsObject
        implements FsDirectoryEntry {
    
    final FatDirectoryEntry realEntry;
    
    private FatLfnDirectory parent;
    private String fileName;
    
    FatLfnDirectoryEntry(String name, ShortName sn,
            FatLfnDirectory parent, boolean directory) {
        
        super(false);
        
        this.parent = parent;
        this.fileName = name;
        
        final long now = System.currentTimeMillis();
        this.realEntry = FatDirectoryEntry.create(directory);
        this.realEntry.setShortName(sn);
        this.realEntry.setCreated(now);
        this.realEntry.setLastAccessed(now);
    }
    
    FatLfnDirectoryEntry(FatLfnDirectory parent,
            FatDirectoryEntry realEntry, String fileName) {
        
        super(parent.isReadOnly());
        
        this.parent = parent;
        this.realEntry = realEntry;
        this.fileName = fileName;
    }
    
    static FatLfnDirectoryEntry extract(
            FatLfnDirectory dir, int offset, int len) {
            
        final FatDirectoryEntry realEntry = dir.dir.getEntry(offset + len - 1);
        final String fileName;
        
        if (len == 1) {
            /* this is just an old plain 8.3 entry */
            fileName = realEntry.getShortName().asSimpleString();
        } else {
            /* stored in reverse order */
            final StringBuilder name = new StringBuilder(13 * (len - 1));
            
            for (int i = len - 2; i >= 0; i--) {
                FatDirectoryEntry entry = dir.dir.getEntry(i + offset);
                name.append(entry.getLfnPart());
            }
            
            fileName = name.toString().trim();
        }
        
         return new FatLfnDirectoryEntry(dir, realEntry, fileName);
    }
    
    /**
     * Returns if this directory entry has the FAT "hidden" flag set.
     *
     * @return if this is a hidden directory entry
     * @see #setHiddenFlag(boolean)
     */
    public boolean isHiddenFlag() {
        return this.realEntry.isHiddenFlag();
    }
    
    /**
     * Sets the "hidden" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param hidden if this entry should have the hidden flag set
     * @throws ReadOnlyException if this entry is read-only
     * @see #isHiddenFlag()
     */
    public void setHiddenFlag(boolean hidden) throws ReadOnlyException {
        checkWritable();
        
        this.realEntry.setHiddenFlag(hidden);
    }
    
    /**
     * Returns if this directory entry has the FAT "system" flag set.
     *
     * @return if this is a "system" directory entry
     * @see #setSystemFlag(boolean)
     */
    public boolean isSystemFlag() {
        return this.realEntry.isSystemFlag();
    }
    
    /**
     * Sets the "system" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param systemEntry if this entry should have the system flag set
     * @throws ReadOnlyException if this entry is read-only
     * @see #isSystemFlag()
     */
    public void setSystemFlag(boolean systemEntry) throws ReadOnlyException {
        checkWritable();
        
        this.realEntry.setSystemFlag(systemEntry);
    }

    /**
     * Returns if this directory entry has the FAT "read-only" flag set. This
     * entry may still modified if {@link #isReadOnly()} returns {@code true}.
     *
     * @return if this entry has the read-only flag set
     * @see #setReadOnlyFlag(boolean) 
     */
    public boolean isReadOnlyFlag() {
        return this.realEntry.isReadonlyFlag();
    }

    /**
     * Sets the "read only" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value. This method only modifies the read-only flag as
     * specified by the FAT file system, which is essentially ignored by the
     * fat32-lib. The true indicator if it is possible to alter this 
     *
     * @param readOnly if this entry should be flagged as read only
     * @throws ReadOnlyException if this entry is read-only as given by
     *      {@link #isReadOnly()} method
     * @see #isReadOnlyFlag() 
     */
    public void setReadOnlyFlag(boolean readOnly) throws ReadOnlyException {
        checkWritable();
        
        this.realEntry.setReadonlyFlag(readOnly);
    }

    /**
     * Returns if this directory entry has the FAT "archive" flag set.
     * 
     * @return if this entry has the archive flag set
     */
    public boolean isArchiveFlag() {
        return this.realEntry.isArchiveFlag();
    }

    /**
     * Sets the "archive" flag on this {@code FatLfnDirectoryEntry} to the
     * specified value.
     *
     * @param archive if this entry should have the archive flag set
     * @throws ReadOnlyException if this entry is
     *      {@link #isReadOnly() read-only}
     */
    public void setArchiveFlag(boolean archive) throws ReadOnlyException {
        checkWritable();

        this.realEntry.setArchiveFlag(archive);
    }
    
    private int totalEntrySize() {
        int result = (fileName.length() / 13) + 1;

        if ((fileName.length() % 13) != 0) {
            result++;
        }
        
        return result;
    }

    FatDirectoryEntry[] compactForm() {
        if (this.realEntry.getShortName().equals(ShortName.DOT) ||
                this.realEntry.getShortName().equals(ShortName.DOT_DOT) ||
                this.realEntry.hasShortNameOnly) {
            /* the dot entries must not have a LFN */
            return new FatDirectoryEntry[]{this.realEntry};
        }
    
        final int totalEntrySize = totalEntrySize();

        final FatDirectoryEntry[] entries =
                new FatDirectoryEntry[totalEntrySize];

        final byte checkSum = this.realEntry.getShortName().checkSum();
        int j = 0;
        
        for (int i = totalEntrySize - 2; i > 0; i--) {
            entries[i] = createPart(fileName.substring(j * 13, j * 13 + 13),
                    j + 1, checkSum, false);
            j++;
        }

        entries[0] = createPart(fileName.substring(j * 13),
                j + 1, checkSum, true);
        
        entries[totalEntrySize - 1] = this.realEntry;
        
        return entries;
    }

    @Override
    public String getName() {
        checkValid();
        
        return fileName;
    }
    
    @Override
    public void setName(String newName) throws IOException {
        checkWritable();
        
        if (!this.parent.isFreeName(newName)) {
            throw new IOException(
                    "the name \"" + newName + "\" is already in use");
        }
        
        this.parent.unlinkEntry(this);
        this.fileName = newName;
        this.parent.linkEntry(this);
    }
    
    /**
     * Moves this entry to a new directory under the specified name.
     *
     * @param target the direcrory where this entry should be moved to
     * @param newName the new name under which this entry will be accessible
     *      in the target directory
     * @throws IOException on error moving this entry
     * @throws ReadOnlyException if this directory is read-only
     */
    public void moveTo(FatLfnDirectory target, String newName)
            throws IOException, ReadOnlyException {

        checkWritable();

        if (!target.isFreeName(newName)) {
            throw new IOException(
                    "the name \"" + newName + "\" is already in use");
        }
        
        this.parent.unlinkEntry(this);
        this.parent = target;
        this.fileName = newName;
        this.parent.linkEntry(this);
    }
    
    @Override
    public void setLastModified(long lastModified) {
        checkWritable();
        realEntry.setLastModified(lastModified);
    }
    
    @Override
    public FatFile getFile() throws IOException {
        return parent.getFile(realEntry);
    }
    
    @Override
    public FatLfnDirectory getDirectory() throws IOException {
        return parent.getDirectory(realEntry);
    }
    
    @Override
    public String toString() {
        return "LFN = " + fileName + " / SFN = " + realEntry.getShortName();
    }
    
    private static FatDirectoryEntry createPart(String subName,
            int ordinal, byte checkSum, boolean isLast) {
            
        final char[] unicodechar = new char[13];
        subName.getChars(0, subName.length(), unicodechar, 0);

        for (int i=subName.length(); i < 13; i++) {
            if (i==subName.length()) {
                unicodechar[i] = 0x0000;
            } else {
                unicodechar[i] = 0xffff;
            }
        }

        final byte[] rawData = new byte[FatDirectoryEntry.SIZE];
        
        if (isLast) {
            LittleEndian.setInt8(rawData, 0, ordinal + (1 << 6));
        } else {
            LittleEndian.setInt8(rawData, 0, ordinal);
        }
        
        LittleEndian.setInt16(rawData, 1, unicodechar[0]);
        LittleEndian.setInt16(rawData, 3, unicodechar[1]);
        LittleEndian.setInt16(rawData, 5, unicodechar[2]);
        LittleEndian.setInt16(rawData, 7, unicodechar[3]);
        LittleEndian.setInt16(rawData, 9, unicodechar[4]);
        LittleEndian.setInt8(rawData, 11, 0x0f); // this is the hidden
                                                    // attribute tag for
        // lfn
        LittleEndian.setInt8(rawData, 12, 0); // reserved
        LittleEndian.setInt8(rawData, 13, checkSum); // checksum
        LittleEndian.setInt16(rawData, 14, unicodechar[5]);
        LittleEndian.setInt16(rawData, 16, unicodechar[6]);
        LittleEndian.setInt16(rawData, 18, unicodechar[7]);
        LittleEndian.setInt16(rawData, 20, unicodechar[8]);
        LittleEndian.setInt16(rawData, 22, unicodechar[9]);
        LittleEndian.setInt16(rawData, 24, unicodechar[10]);
        LittleEndian.setInt16(rawData, 26, 0); // sector... unused
        LittleEndian.setInt16(rawData, 28, unicodechar[11]);
        LittleEndian.setInt16(rawData, 30, unicodechar[12]);
        
        return new FatDirectoryEntry(rawData, false);
    }

    @Override
    public long getLastModified() throws IOException {
        return realEntry.getLastModified();
    }

    @Override
    public long getCreated() throws IOException {
        return realEntry.getCreated();
    }

    @Override
    public long getLastAccessed() throws IOException {
        return realEntry.getLastAccessed();
    }

    @Override
    public boolean isFile() {
        return realEntry.isFile();
    }

    @Override
    public boolean isDirectory() {
        return realEntry.isDirectory();
    }

    @Override
    public boolean isDirty() {
        return realEntry.isDirty();
    }
    
}
