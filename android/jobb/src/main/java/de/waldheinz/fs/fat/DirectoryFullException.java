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

import java.io.IOException;

/**
 * Gets thrown when either
 * <ul>
 * <li>a {@link Fat16RootDirectory} becomes full or</li>
 * <li>a {@link ClusterChainDirectory} grows beyond it's
 *      {@link ClusterChainDirectory#MAX_SIZE maximum size}
 * </ul>
 * 
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public final class DirectoryFullException extends IOException {
    private final static long serialVersionUID = 2;
    private final int currentCapacity;
    private final int requestedCapacity;
    
    DirectoryFullException(int currentCapacity, int requestedCapacity) {
        this("directory is full", currentCapacity, requestedCapacity);
    }
    
    DirectoryFullException(String message,
            int currentCapacity, int requestedCapacity) {

        super(message);

        this.currentCapacity = currentCapacity;
        this.requestedCapacity = requestedCapacity;
    }
    
    /**
     * Returns the current capacity of the directory.
     *
     * @return the current capacity
     */
    public int getCurrentCapacity() {
        return currentCapacity;
    }

    /**
     * Returns the capacity the directory tried to grow, which did not succeed.
     *
     * @return the requested capacity
     */
    public int getRequestedCapacity() {
        return requestedCapacity;
    }
    
}
