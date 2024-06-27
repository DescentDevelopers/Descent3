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

import de.waldheinz.fs.BlockDevice;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * The root directory of a FAT12/16 partition.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
final class Fat16RootDirectory extends AbstractDirectory {
    private final BlockDevice device;
    private final long deviceOffset;

    private Fat16RootDirectory(Fat16BootSector bs, boolean readOnly) {
        super(bs.getRootDirEntryCount(), readOnly, true);

        if (bs.getRootDirEntryCount() <= 0) throw new IllegalArgumentException(
                "root directory size is " + bs.getRootDirEntryCount());
        
        this.deviceOffset = FatUtils.getRootDirOffset(bs);
        this.device = bs.getDevice();
    }
    
    /**
     * Reads a {@code Fat16RootDirectory} as indicated by the specified
     * {@code Fat16BootSector}.
     *
     * @param bs the boot sector that describes the root directory to read
     * @param readOnly if the directory shold be created read-only
     * @return the directory that was read
     * @throws IOException on read error
     */
    public static Fat16RootDirectory read(
            Fat16BootSector bs, boolean readOnly) throws IOException {
        
        final Fat16RootDirectory result = new Fat16RootDirectory(bs, readOnly);
        result.read();
        return result;
    }

    /**
     * Creates a new {@code Fat16RootDirectory} as indicated by the specified
     * {@code Fat16BootSector}. The directory will always be created in
     * read-write mode.
     *
     * @param bs the boot sector that describes the root directory to create
     * @return the directory that was created
     * @throws IOException on write error
     */
    public static Fat16RootDirectory create(
            Fat16BootSector bs) throws IOException {
        
        final Fat16RootDirectory result = new Fat16RootDirectory(bs, false);
        result.flush();
        return result;
    }
    
    @Override
    protected void read(ByteBuffer data) throws IOException {
        this.device.read(deviceOffset, data);
    }

    @Override
    protected void write(ByteBuffer data) throws IOException {
        this.device.write(deviceOffset, data);
    }

    /**
     * By convention always returns 0, as the FAT12/16 root directory is not
     * stored in a cluster chain.
     *
     * @return always 0
     */
    @Override
    protected long getStorageCluster() {
        return 0;
    }

    /**
     * As a FAT12/16 root directory can not change it's size, this method
     * throws a {@code DirectoryFullException} if the requested size is
     * larger than {@link #getCapacity()} and does nothing else.
     *
     * @param entryCount {@inheritDoc}
     */
    @Override
    protected void changeSize(int entryCount) throws DirectoryFullException {
        if (getCapacity() < entryCount) {
            throw new DirectoryFullException(getCapacity(), entryCount);
        }
    }
    
}
