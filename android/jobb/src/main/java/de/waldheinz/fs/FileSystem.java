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

/**
 * The interface common to all file system implementations.
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public interface FileSystem {
    
    /**
     * Gets the root entry of this filesystem. This is usually a directory, but
     * this is not required.
     * 
     * @return the file system's root entry
     * @throws IOException on read error
     */
    public FsDirectory getRoot() throws IOException;

    /**
     * Returns if this {@code FileSystem} is in read-only mode.
     *
     * @return if this {@code FileSystem} is read-only
     */
    public boolean isReadOnly();

    /**
     * Close this file system. After a close, all invocations of methods of
     * this file system or objects created by this file system will throw an
     * {@link IllegalStateException}.
     * 
     * @throws IOException on error closing the file system
     */
    public void close() throws IOException;
    
    /**
     * Returns {@code true} if this file system is closed. If the file system
     * is closed, no more operations may be performed on it.
     * 
     * @return if this file system is closed
     */
    public boolean isClosed();

    /**
     * The total size of this file system.
     *
     * @return if -1 this feature is unsupported
     * @throws IOException if an I/O error occurs
     */
    public long getTotalSpace() throws IOException;

    /**
     * The free space of this file system.
     *
     * @return if -1 this feature is unsupported
     * @throws IOException if an I/O error occurs
     */
    public long getFreeSpace() throws IOException;

    /**
     * The usable space of this file system.
     *
     * @return if -1 this feature is unsupported
     * @throws IOException if an I/O error occurs
     */
    public long getUsableSpace() throws IOException;

    /**
     * Flushes any modified file system structures to the underlying storage.
     *
     * @throws IOException
     */
    public void flush() throws IOException;
}
