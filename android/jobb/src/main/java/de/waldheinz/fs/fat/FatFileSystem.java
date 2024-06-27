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

import de.waldheinz.fs.AbstractFileSystem;
import de.waldheinz.fs.BlockDevice;
import java.io.IOException;
import de.waldheinz.fs.ReadOnlyException;

/**
 * <p>
 * Implements the {@code FileSystem} interface for the FAT family of file
 * systems. This class always uses the "long file name" specification when
 * writing directory entries.
 * </p><p>
 * For creating (aka "formatting") FAT file systems please refer to the
 * {@link SuperFloppyFormatter} class.
 * </p>
 *
 * @author Ewout Prangsma &lt;epr at jnode.org&gt;
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public final class FatFileSystem extends AbstractFileSystem {
    
    private final Fat fat;
    private final FsInfoSector fsiSector;
    private final BootSector bs;
    private final FatLfnDirectory rootDir;
    private final AbstractDirectory rootDirStore;
    private final FatType fatType;
    private final long filesOffset;

    FatFileSystem(BlockDevice api, boolean readOnly) throws IOException {

        this(api, readOnly, false);
    }
    
    /**
     * Constructor for FatFileSystem in specified readOnly mode
     * 
     * @param device the {@code BlockDevice} holding the file system
     * @param readOnly if this FS should be read-lonly
     * @param ignoreFatDifferences
     * @throws IOException on read error
     */
    private FatFileSystem(BlockDevice device, boolean readOnly,
            boolean ignoreFatDifferences)
            throws IOException {
        
        super(readOnly);
        
        this.bs = BootSector.read(device);
        
        if (bs.getNrFats() <= 0) throw new IOException(
                "boot sector says there are no FATs");
        
        this.filesOffset = FatUtils.getFilesOffset(bs);
        this.fatType = bs.getFatType();
        this.fat = Fat.read(bs, 0);

        if (!ignoreFatDifferences) {
            for (int i=1; i < bs.getNrFats(); i++) {
                final Fat tmpFat = Fat.read(bs, i);
                if (!fat.equals(tmpFat)) {
                    throw new IOException("FAT " + i + " differs from FAT 0");
                }
            }
        }
        
        if (fatType == FatType.FAT32) {
            final Fat32BootSector f32bs = (Fat32BootSector) bs;
            ClusterChain rootDirFile = new ClusterChain(fat,
                    f32bs.getRootDirFirstCluster(), isReadOnly());
            this.rootDirStore = ClusterChainDirectory.readRoot(rootDirFile);
            this.fsiSector = FsInfoSector.read(f32bs);
            
            if (fsiSector.getFreeClusterCount() != fat.getFreeClusterCount()) {
                throw new IOException("free cluster count mismatch - fat: " +
                        fat.getFreeClusterCount() + " - fsinfo: " +
                        fsiSector.getFreeClusterCount());
            }
        } else {
            this.rootDirStore =
                    Fat16RootDirectory.read((Fat16BootSector) bs,readOnly);
            this.fsiSector = null;
        }

        this.rootDir = new FatLfnDirectory(rootDirStore, fat, isReadOnly());
            
    }

    /**
     * Reads the file system structure from the specified {@code BlockDevice}
     * and returns a fresh {@code FatFileSystem} instance to read or modify
     * it.
     *
     * @param device the {@code BlockDevice} holding the file system
     * @param readOnly if the {@code FatFileSystem} should be in read-only mode
     * @return the {@code FatFileSystem} instance for the device
     * @throws IOException on read error or if the file system structure could
     *      not be parsed
     */
    public static FatFileSystem read(BlockDevice device, boolean readOnly)
            throws IOException {
        
        return new FatFileSystem(device, readOnly);
    }

    long getFilesOffset() {
        checkClosed();
        
        return filesOffset;
    }

    /**
     * Returns the size of the FAT entries of this {@code FatFileSystem}.
     *
     * @return the exact type of the FAT used by this file system
     */
    public FatType getFatType() {
        checkClosed();

        return this.fatType;
    }

    /**
     * Returns the volume label of this file system.
     *
     * @return the volume label
     */
    public String getVolumeLabel() {
        checkClosed();
        
        final String fromDir = rootDirStore.getLabel();
        
        if (fromDir == null && fatType != FatType.FAT32) {
            return ((Fat16BootSector)bs).getVolumeLabel();
        } else {
            return fromDir;
        }
    }
    
    /**
     * Sets the volume label for this file system.
     *
     * @param label the new volume label, may be {@code null}
     * @throws ReadOnlyException if the file system is read-only
     * @throws IOException on write error
     */
    public void setVolumeLabel(String label)
            throws ReadOnlyException, IOException {
        
        checkClosed();
        checkReadOnly();

        rootDirStore.setLabel(label);
        
        if (fatType != FatType.FAT32) {
            ((Fat16BootSector)bs).setVolumeLabel(label);
        }
    }

    AbstractDirectory getRootDirStore() {
        checkClosed();
        
        return rootDirStore;
    }
    
    /**
     * Flush all changed structures to the device.
     * 
     * @throws IOException on write error
     */
    @Override
    public void flush() throws IOException {
        checkClosed();
        
        if (bs.isDirty()) {
            bs.write();
        }
        
        for (int i = 0; i < bs.getNrFats(); i++) {
            fat.writeCopy(FatUtils.getFatOffset(bs, i));
        }
        
        rootDir.flush();
        
        if (fsiSector != null) {
            fsiSector.setFreeClusterCount(fat.getFreeClusterCount());
            fsiSector.setLastAllocatedCluster(fat.getLastAllocatedCluster());
            fsiSector.write();
        }
    }
    
    @Override
    public FatLfnDirectory getRoot() {
        checkClosed();
        
        return rootDir;
    }
    
    /**
     * Returns the fat.
     * 
     * @return Fat
     */
    public Fat getFat() {
        return fat;
    }

    /**
     * Returns the bootsector.
     * 
     * @return BootSector
     */
    public BootSector getBootSector() {
        checkClosed();
        
        return bs;
    }

    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * This method shows the free space in terms of available clusters.
     * </p>
     * 
     * @return always -1
     */
    @Override
    public long getFreeSpace() {
    	return this.fat.getFreeClusterCount() * this.bs.getBytesPerCluster();
    }

    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * This method is currently not implemented for {@code FatFileSystem} and
     * always returns -1.
     * </p>
     *
     * @return always -1
     */
    @Override
    public long getTotalSpace() {
    	return this.bs.getDataClusterCount() * this.bs.getBytesPerCluster();    	
    }

    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * This method is currently not implemented for {@code FatFileSystem} and
     * always returns -1.
     * </p>
     *
     * @return always -1
     */
    @Override
    public long getUsableSpace() {
        // TODO implement me
        return -1;
    }
}
