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

/**
 * This interface is the base interface for objects that are part of a
 * {@link FileSystem}.
 * 
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public interface FsObject {
    
    /**
     * Checks if this {@code FsObject} is still valid.
     * 
     * An object is not valid anymore if it has been removed from the
     * filesystem. All invocations on methods (except this method and the
     * methods inherited from {@link java.lang.Object}) of
     * invalid objects must throw an {@link IllegalStateException}.
     * 
     * @return if this {@code FsObject} is still valid
     */
    public boolean isValid();
    
    /**
     * Checks if this {@code FsObject} is read-only. Any attempt to modify a
     * read-only {@code FsObject} must result in a {@link ReadOnlyException}
     * being thrown, and the modification must not be performed.
     *
     * @return if this {@code FsObject} is read-only
     * @since 0.6
     */
    public boolean isReadOnly();
    
}
