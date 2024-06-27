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
import java.util.Random;

/**
 * <p>
 * Allows to create FAT file systems on {@link BlockDevice}s which follow the
 * "super floppy" standard. This means that the device will be formatted so
 * that it does not contain a partition table. Instead, the entire device holds
 * a single FAT file system.
 * </p><p>
 * This class follows the "builder" pattern, which means it's methods always
 * returns the {@code SuperFloppyFormatter} instance they're called on. This
 * allows to chain the method calls like this:
 * <pre>
 *  BlockDevice dev = new RamDisk(16700000);
 *  FatFileSystem fs = SuperFloppyFormatter.get(dev).
 *          setFatType(FatType.FAT12).format();
 * </pre>
 * 
 * </p>
 *
 * @author Matthias Treydte &lt;matthias.treydte at meetwise.com&gt;
 */
public final class SuperFloppyFormatter {

    /**
     * The media descriptor used (hard disk).
     */
    public final static int MEDIUM_DESCRIPTOR_HD = 0xf8;

    /**
     * The default number of FATs.
     */
    public final static int DEFAULT_FAT_COUNT = 2;

    /**
     * The default number of sectors per track.
     */
    public final static int DEFAULT_SECTORS_PER_TRACK = 32;

    /**
     * The default number of heads.
     * 
     * @since 0.6
     */
    public final static int DEFAULT_HEADS = 64;

    /**
     * The default number of heads.
     * 
     * @deprecated the name of this constant was mistyped
     * @see #DEFAULT_HEADS
     */
    @Deprecated
    public final static int DEFULT_HEADS = DEFAULT_HEADS;

    /**
     * The default OEM name for file systems created by this class.
     */
    public final static String DEFAULT_OEM_NAME = "fat32lib"; //NOI18N
    
    private static final int MAX_DIRECTORY = 512;
    
    private final BlockDevice device;
    
    private String label;
    private String oemName;
    private FatType fatType;
    private int sectorsPerCluster;
    private int reservedSectors;
    private int fatCount;

    /**
     * Creates a new {@code SuperFloppyFormatter} for the specified
     * {@code BlockDevice}.
     *
     * @param device
     * @throws IOException on error accessing the specified {@code device}
     */
    private SuperFloppyFormatter(BlockDevice device) throws IOException {
        this.device = device;
        this.oemName = DEFAULT_OEM_NAME;
        this.fatCount = DEFAULT_FAT_COUNT;
        setFatType(fatTypeFromDevice());
    }
    
    /**
     * Retruns a {@code SuperFloppyFormatter} instance suitable for formatting
     * the specified device.
     *
     * @param dev the device that should be formatted
     * @return the formatter for the device
     * @throws IOException on error creating the formatter
     */
    public static SuperFloppyFormatter get(BlockDevice dev) throws IOException {
        return new SuperFloppyFormatter(dev);
    }
    
    /**
     * Returns the OEM name that will be written to the {@link BootSector}.
     *
     * @return the OEM name of the new file system
     */
    public String getOemName() {
        return oemName;
    }
    
    /**
     * Sets the OEM name of the boot sector.
     *
     * TODO: throw an exception early if name is invalid (too long, ...)
     *
     * @param oemName the new OEM name
     * @return this {@code SuperFloppyFormatter}
     * @see BootSector#setOemName(java.lang.String)
     */
    public SuperFloppyFormatter setOemName(String oemName) {
        this.oemName = oemName;
        return this;
    }
    
    /**
     * Sets the volume label of the file system to create.
     * 
     * TODO: throw an exception early if label is invalid (too long, ...)
     * 
     * @param label the new file system label, may be {@code null}
     * @return this {@code SuperFloppyFormatter}
     * @see FatFileSystem#setVolumeLabel(java.lang.String)
     */
    public SuperFloppyFormatter setVolumeLabel(String label) {
        this.label = label;
        return this;
    }

    /**
     * Returns the volume label that will be given to the new file system.
     *
     * @return the file system label, may be {@code null}
     * @see FatFileSystem#getVolumeLabel() 
     */
    public String getVolumeLabel() {
        return label;
    }

    private void initBootSector(BootSector bs)
            throws IOException {
        
        bs.init();
        bs.setFileSystemTypeLabel(fatType.getLabel());
        bs.setNrReservedSectors(reservedSectors);
        bs.setNrFats(fatCount);
        bs.setSectorsPerCluster(sectorsPerCluster);
        bs.setMediumDescriptor(MEDIUM_DESCRIPTOR_HD);
        bs.setSectorsPerTrack(DEFAULT_SECTORS_PER_TRACK);
        bs.setNrHeads(DEFAULT_HEADS);
        bs.setOemName(oemName);
    }

    /**
     * Initializes the boot sector and file system for the device. The file
     * system created by this method will always be in read-write mode.
     *
     * @return the file system that was created
     * @throws IOException on write error
     */
    public FatFileSystem format() throws IOException {
        final int sectorSize = device.getSectorSize();
        final int totalSectors = (int)(device.getSize() / sectorSize);
        
        final FsInfoSector fsi;
        final BootSector bs;
        if (sectorsPerCluster == 0) throw new AssertionError();
        
        if (fatType == FatType.FAT32) {
            bs = new Fat32BootSector(device);
            initBootSector(bs);
            
            final Fat32BootSector f32bs = (Fat32BootSector) bs;
            
            f32bs.setFsInfoSectorNr(1);
            
            f32bs.setSectorsPerFat(sectorsPerFat(0, totalSectors));
            final Random rnd = new Random(System.currentTimeMillis());
            f32bs.setFileSystemId(rnd.nextInt());
            
            f32bs.setVolumeLabel(label);
            
            /* create FS info sector */
            fsi = FsInfoSector.create(f32bs);
        } else {
            bs = new Fat16BootSector(device);
            initBootSector(bs);
            
            final Fat16BootSector f16bs = (Fat16BootSector) bs;
            
            final int rootDirEntries = rootDirectorySize(
                    device.getSectorSize(), totalSectors);
                    
            f16bs.setRootDirEntryCount(rootDirEntries);
            f16bs.setSectorsPerFat(sectorsPerFat(rootDirEntries, totalSectors));
            if (label != null) f16bs.setVolumeLabel(label);
            fsi = null;
        }

        
//        bs.write();
        
        if (fatType == FatType.FAT32) {
            Fat32BootSector f32bs = (Fat32BootSector) bs;
            /* possibly writes the boot sector copy */
            f32bs.writeCopy(device);
        }
        
        final Fat fat = Fat.create(bs, 0);
        
        final AbstractDirectory rootDirStore;
        if (fatType == FatType.FAT32) {
            rootDirStore = ClusterChainDirectory.createRoot(fat);
            fsi.setFreeClusterCount(fat.getFreeClusterCount());
            fsi.setLastAllocatedCluster(fat.getLastAllocatedCluster());
            fsi.write();
        } else {
            rootDirStore = Fat16RootDirectory.create((Fat16BootSector) bs);
        }
        
        final FatLfnDirectory rootDir =
                new FatLfnDirectory(rootDirStore, fat, false);
        
        rootDir.flush();
        
        for (int i = 0; i < bs.getNrFats(); i++) {
            fat.writeCopy(FatUtils.getFatOffset(bs, i));
        }
        
        bs.write();

        FatFileSystem fs = FatFileSystem.read(device, false);

        if (label != null) {
            fs.setVolumeLabel(label);
        }

        fs.flush();
        return fs;
    }

    private int sectorsPerFat(int rootDirEntries, int totalSectors)
            throws IOException {
        
        final int bps = device.getSectorSize();
        final int rootDirSectors =
                ((rootDirEntries * 32) + (bps - 1)) / bps;
        final long tmp1 =
                totalSectors - (this.reservedSectors + rootDirSectors);
        int tmp2 = (256 * this.sectorsPerCluster) + this.fatCount;

        if (fatType == FatType.FAT32)
            tmp2 /= 2;

        final int result = (int) ((tmp1 + (tmp2 - 1)) / tmp2);
        
        return result;
    }
    
    /**
     * Determines a usable FAT type from the {@link #device} by looking at the
     * {@link BlockDevice#getSize() device size} only.
     *
     * @return the suggested FAT type
     * @throws IOException on error determining the device's size
     */
    private FatType fatTypeFromDevice() throws IOException {
    	return fatTypeFromSize(device.getSize());
    }
    
    /**
     * Determines a usable FAT type from the {@link #device} by looking at the
     * {@link BlockDevice#getSize() device size} only.
     *
     * @return the suggested FAT type
     * @throws IOException on error determining the device's size
     */
    public static FatType fatTypeFromSize(long sizeInBytes) {
        final long sizeInMb = sizeInBytes / (1024 * 1024);
        if (sizeInMb < 5) return FatType.FAT12;
        else if (sizeInMb < 2048) return FatType.FAT16;
        else return FatType.FAT32;    	
    }
    
    public static int clusterSizeFromSize(long sizeInBytes, int sectorSize){
    	switch(fatTypeFromSize(sizeInBytes)) {
        case FAT12:
            return sectorsPerCluster12(sizeInBytes, sectorSize);
        case FAT16:
            return sectorsPerCluster16FromSize(sizeInBytes, sectorSize);
        case FAT32:
            return sectorsPerCluster32FromSize(sizeInBytes, sectorSize);
            
        default:
            throw new AssertionError();
    	}
    }
    
    /**
     * Returns the exact type of FAT the will be created by this formatter.
     *
     * @return the FAT type
     */
    public FatType getFatType() {
        return this.fatType;
    }

    /**
     * Sets the type of FAT that will be created by this
     * {@code SuperFloppyFormatter}.
     *
     * @param fatType the desired {@code FatType}
     * @return this {@code SuperFloppyFormatter}
     * @throws IOException on error setting the {@code fatType}
     * @throws IllegalArgumentException if {@code fatType} does not support the
     *      size of the device
     */
    public SuperFloppyFormatter setFatType(FatType fatType)
            throws IOException, IllegalArgumentException {
        
        if (fatType == null) throw new NullPointerException();

        switch (fatType) {
            case FAT12: case FAT16:
                this.reservedSectors = 1;
                break;
                
            case FAT32:
                this.reservedSectors = 32;
        }
        
        this.sectorsPerCluster = defaultSectorsPerCluster(fatType);
        this.fatType = fatType;
        
        return this;
    }
    
    private static int rootDirectorySize(int bps, int nbTotalSectors) {
        final int totalSize = bps * nbTotalSectors;
        if (totalSize >= MAX_DIRECTORY * 5 * 32) {
            return MAX_DIRECTORY;
        } else {
            return totalSize / (5 * 32);
        }
    }
    
    static private int MAX_FAT32_CLUSTERS = 0x0FFFFFF5;
    
    static private int sectorsPerCluster32FromSize(long size, int sectorSize) {
        final long sectors = size / sectorSize;
        
        if (sectors <= 66600) throw new IllegalArgumentException(
                "disk too small for FAT32");

        return
                sectors > 67108864 ? 64 :
                sectors > 33554432 ? 32 :
                sectors > 16777216 ? 16 :
                sectors >   532480 ?  8 : 1;
    }
    
    private int sectorsPerCluster32() throws IOException {
        if (this.reservedSectors != 32) throw new IllegalStateException(
                "number of reserved sectors must be 32");
        
        if (this.fatCount != 2) throw new IllegalStateException(
                "number of FATs must be 2");

        final long sectors = device.getSize() / device.getSectorSize();

        if (sectors <= 66600) throw new IllegalArgumentException(
                "disk too small for FAT32");

        return sectorsPerCluster32FromSize(device.getSize(), device.getSectorSize());
    }

    static private int MAX_FAT16_CLUSTERS = 65524;
    
    static private int sectorsPerCluster16FromSize(long size, int sectorSize) {
        final long sectors = size / sectorSize;
        
        if (sectors <= 8400) throw new IllegalArgumentException(
                "disk too small for FAT16");

        if (sectors > 4194304) throw new IllegalArgumentException(
                "disk too large for FAT16");

          return
          sectors > 2097152 ? 64 :
          sectors > 1048576 ? 32 :
          sectors >  524288 ? 16 :
          sectors >  262144 ?  8 :
          sectors >   32680 ?  4 : 2;
    }
    
    private int sectorsPerCluster16() throws IOException {
        if (this.reservedSectors != 1) throw new IllegalStateException(
                "number of reserved sectors must be 1");

        if (this.fatCount != 2) throw new IllegalStateException(
                "number of FATs must be 2");
        
    	long size = device.getSize();
        int sectorSize = device.getSectorSize();
        return sectorsPerCluster16FromSize(size, sectorSize);        
    }
    
    private int defaultSectorsPerCluster(FatType fatType) throws IOException {
    	long size = device.getSize();
        int sectorSize = device.getSectorSize();
    	
        switch (fatType) {
            case FAT12:
                return sectorsPerCluster12(size, sectorSize);

            case FAT16:
                return sectorsPerCluster16();

            case FAT32:
                return sectorsPerCluster32();
                
            default:
                throw new AssertionError();
        }
    }

    static private int sectorsPerCluster12(long size, int sectorSize) {
        int result = 1;
        
        final long sectors = size / sectorSize;

        while (sectors / result > Fat16BootSector.MAX_FAT12_CLUSTERS) {
            result *= 2;
            if (result * size > 4096) throw new
                    IllegalArgumentException("disk too large for FAT12");
        }
        
        return result;
    }
    
}
