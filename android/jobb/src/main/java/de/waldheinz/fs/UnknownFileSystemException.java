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

package de.waldheinz.fs;

import java.io.IOException;

/**
 * Indicates that it was not possible to determine the type of the file
 * system being used on a block device.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public final class UnknownFileSystemException extends IOException {
    private final static long serialVersionUID = 1;
    
    private final BlockDevice device;
    
    /**
     * Creates a new instance of {@code UnknownFileSystemException}.
     * 
     * @param device the {@code BlockDevice} whose file system could not
     *      be determined
     */
    public UnknownFileSystemException(BlockDevice device) {
        super("can not determin file system type"); //NOI18N
        this.device = device;
    }

    /**
     * Returns the {@code BlockDevice} whose file system could not be
     * determined.
     *
     * @return the {@code BlockDevice} with an unknown file system
     */
    public BlockDevice getDevice() {
        return this.device;
    }
}
