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

/**
 * This exception is thrown when an attempt is made to write to a read-only
 * {@link BlockDevice}, {@link FileSystem} or other file system object. This is
 * an unchecked exception, as it should always be possible to query the object
 * about it's read-only state using it's {@code isReadOnly()} method.
 * 
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @see FileSystem#isReadOnly()
 * @see BlockDevice#isReadOnly() 
 */
public final class ReadOnlyException extends RuntimeException {

    private final static long serialVersionUID = 1;
    
    /**
     * Creates a new instance of {@code ReadOnlyException}.
     *
     */
    public ReadOnlyException() {
        super("read-only");
    }
}
