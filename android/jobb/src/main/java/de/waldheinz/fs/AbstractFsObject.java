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
 * A base class that helps to implement the {@code FsObject} interface.
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public class AbstractFsObject implements FsObject {

    /**
     * Holds the read-only state of this object.
     */
    private final boolean readOnly;
    
    /**
     * Remembers if this object still valid.
     */
    private boolean valid;

    /**
     * Creates a new instance of {@code AbstractFsObject} which will be valid
     * and have the specified read-only state.
     *
     * @param readOnly if the new object will be read-only
     */
    protected AbstractFsObject(boolean readOnly) {
        this.valid = true;
        this.readOnly = readOnly;
    }
    
    /** 
     * {@inheritDoc}
     *
     * @return {@inheritDoc}
     * @see #checkValid()
     * @see #invalidate() 
     */
    @Override
    public final boolean isValid() {
        return this.valid;
    }

    /**
     * Marks this object as invalid.
     * 
     * @see #isValid()
     * @see #checkValid()
     */
    protected final void invalidate() {
        this.valid = false;
    }

    /**
     * Convience method to check if this object is still valid and throw an
     * {@code IllegalStateException} if it is not.
     *
     * @throws IllegalStateException if this object was invalidated
     * @since 0.6
     * @see #isValid()
     * @see #invalidate() 
     */
    protected final void checkValid() throws IllegalStateException {
        if (!isValid()) throw new IllegalStateException(
                this + " is not valid");
    }

    /**
     * Convience method to check if this object is writable. An object is
     * writable if it is both, valid and not read-only. 
     *
     * @throws IllegalStateException if this object was invalidated
     * @throws ReadOnlyException if this object was created with the read-only
     *      flag set
     * @since 0.6
     */
    protected final void checkWritable()
            throws IllegalStateException, ReadOnlyException {
        
        checkValid();

        if (isReadOnly()) {
            throw new ReadOnlyException();
        }
    }
    
    @Override
    public final boolean isReadOnly() {
        return this.readOnly;
    }
    
}
