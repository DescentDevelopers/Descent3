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
 
package de.waldheinz.fs;

import java.io.IOException;
import java.util.Comparator;

/**
 * Represents one entry in a {@link FsDirectory}.
 * 
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public interface FsDirectoryEntry extends FsObject {

    /**
     * Compares directory entries alphabetically, with all directories coming
     * before all files.
     */
    public final static Comparator<FsDirectoryEntry> DIRECTORY_ENTRY_COMPARATOR =
            new Comparator<FsDirectoryEntry>() {

        @Override
        public int compare(FsDirectoryEntry e1, FsDirectoryEntry e2) {
            if (e2.isDirectory() == e1.isDirectory()) {
                /* compare names */
                return e1.getName().compareTo(e2.getName());
            } else {
                if (e2.isDirectory()) return 1;
                else return -1;
            }
        }
    };
    
    /**
     * Gets the name of this entry.
     *
     * @return this entrys name
     */
    public String getName();
    
    /**
     * Gets the last modification time of this entry.
     *
     * @return the last modification time of the entry as milliseconds
     *      since 1970, or {@code 0} if this filesystem does not support
     *      getting the last modification time
     * @throws IOException if an error occurs retrieving the time stamp
     */
    public long getLastModified() throws IOException;

    /**
     * Returns the time when this entry was created as ms since 1970.
     *
     * @return the creation time, or 0 if this feature is not supported
     * @throws IOException on error retrieving the time stamp
     */
    public long getCreated() throws IOException;

    /**
     * Returns the time when this entry was last accessed as ms since 1970.
     *
     * @return the last access time, or 0 if this feature is not supported
     * @throws IOException on error retrieving the last access time
     */
    public long getLastAccessed() throws IOException;
    
    /**
     * Is this entry refering to a file?
     * 
     * @return if this entry refers to a file
     */
    public boolean isFile();

    /**
     * Is this entry refering to a (sub-)directory?
     *
     * @return if this entry refers to a directory
     */
    public boolean isDirectory();

    /**
     * Sets the name of this entry.
     * 
     * @param newName the new name of this entry
     * @throws IOException on error setting the new name
     */
    public void setName(String newName) throws IOException;

    /**
     * Sets the last modification time of this entry.
     * 
     * @param lastModified the new last modification time of this entry
     * @throws IOException on write error
     */
    public void setLastModified(long lastModified) throws IOException;

    /**
     * Gets the file this entry refers to. This method can only be called if
     * {@code isFile} returns {@code true}.
     * 
     * @return the file described by this entry
     * @throws IOException on error accessing the file
     * @throws UnsupportedOperationException if this entry is a directory
     */
    public FsFile getFile()
            throws IOException, UnsupportedOperationException;
    
    /**
     * Gets the directory this entry refers to. This method can only be called
     * if <code>isDirectory</code> returns true.
     * 
     * @return The directory described by this entry
     * @throws IOException on read error
     * @throws UnsupportedOperationException if this entry is a file
     */
    public FsDirectory getDirectory()
            throws IOException, UnsupportedOperationException;
    
    /**
     * Indicate if the entry has been modified in memory (ie need to be saved)
     * 
     * @return true if the entry needs to be saved
     */
    public boolean isDirty();
}
