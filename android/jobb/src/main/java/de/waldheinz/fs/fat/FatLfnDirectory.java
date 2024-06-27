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

import de.waldheinz.fs.AbstractFsObject;
import de.waldheinz.fs.FsDirectory;
import de.waldheinz.fs.FsDirectoryEntry;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/**
 * The {@link FsDirectory} implementation for FAT file systems. This
 * implementation aims to fully comply to the FAT specification, including
 * the quite complex naming system regarding the long file names (LFNs) and
 * their corresponding 8+3 short file names. This also means that an
 * {@code FatLfnDirectory} is case-preserving but <em>not</em> case-sensitive.
 * 
 * @author gbin
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 * @since 0.6
 */
public final class FatLfnDirectory
        extends AbstractFsObject
        implements FsDirectory {
    
    /**
     * This set is used to check if a file name is already in use in this
     * directory. The FAT specification says that file names must be unique
     * ignoring the case, so this set contains all names converted to
     * lower-case, and all checks must be performed using lower-case strings.
     */
    private final Set<String> usedNames;
    private final Fat fat;
    private final Map<ShortName, FatLfnDirectoryEntry> shortNameIndex;
    private final Map<String, FatLfnDirectoryEntry> longNameIndex;
    private final Map<FatDirectoryEntry, FatFile> entryToFile;
    private final Map<FatDirectoryEntry, FatLfnDirectory> entryToDirectory;
    private Dummy83BufferGenerator dbg;
    
    final AbstractDirectory dir;
    
    FatLfnDirectory(AbstractDirectory dir, Fat fat, boolean readOnly)
            throws IOException {
        
        super(readOnly);
        
        if ((dir == null) || (fat == null)) throw new NullPointerException();
        
        this.fat = fat;
        this.dir = dir;
        
        this.shortNameIndex =
                new LinkedHashMap<ShortName, FatLfnDirectoryEntry>();
                
        this.longNameIndex =
                new LinkedHashMap<String, FatLfnDirectoryEntry>();
                
        this.entryToFile =
                new LinkedHashMap<FatDirectoryEntry, FatFile>();
                
        this.entryToDirectory =
                new LinkedHashMap<FatDirectoryEntry, FatLfnDirectory>();
                
        this.usedNames = new HashSet<String>();
        this.dbg = new Dummy83BufferGenerator();
        
        parseLfn();
    }
    
    FatFile getFile(FatDirectoryEntry entry) throws IOException {
        FatFile file = entryToFile.get(entry);

        if (file == null) {
            file = FatFile.get(fat, entry);
            entryToFile.put(entry, file);
        }
        
        return file;
    }
    
    FatLfnDirectory getDirectory(FatDirectoryEntry entry) throws IOException {
        FatLfnDirectory result = entryToDirectory.get(entry);

        if (result == null) {
            final ClusterChainDirectory storage = read(entry, fat);
            result = new FatLfnDirectory(storage, fat, isReadOnly());
            entryToDirectory.put(entry, result);
        }
        
        return result;
    }
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * According to the FAT file system specification, leading and trailing
     * spaces in the {@code name} are ignored by this method.
     * </p>
     * 
     * @param name {@inheritDoc}
     * @return {@inheritDoc}
     * @throws IOException {@inheritDoc}
     */
    @Override
    public FatLfnDirectoryEntry addFile(String name) throws IOException {
        checkWritable();
        checkUniqueName(name);
        
        name = name.trim();
        final ShortName sn = makeShortName(name, false);
        
        final FatLfnDirectoryEntry entry =
                new FatLfnDirectoryEntry(name, sn, this, false);

        dir.addEntries(entry.compactForm());
        
        shortNameIndex.put(sn, entry);
        longNameIndex.put(name.toLowerCase(), entry);

        getFile(entry.realEntry);
        
        dir.setDirty();
        return entry;
    }
    
    boolean isFreeName(String name) {
        return true;
    }
    
    private void checkUniqueName(String name) throws IOException {
    }
    
    private void freeUniqueName(String name) {
    }
    
    private ShortName makeShortName(String name, boolean isDirectory) throws IOException {
        final ShortName result;

        try {
            result = dbg.generate83BufferNew(name);
        } catch (IllegalArgumentException ex) {
            throw new IOException(
                    "could not generate short name for \"" + name + "\"", ex);
        }        
        return result;
    }
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * According to the FAT file system specification, leading and trailing
     * spaces in the {@code name} are ignored by this method.
     * </p>
     *
     * @param name {@inheritDoc}
     * @return {@inheritDoc}
     * @throws IOException {@inheritDoc}
     */
    @Override
    public FatLfnDirectoryEntry addDirectory(String name) throws IOException {
        checkWritable();
        checkUniqueName(name);
        
        name = name.trim();
        final ShortName sn = makeShortName(name, true);
        final FatDirectoryEntry real = dir.createSub(fat);
        real.setShortName(sn);
        final FatLfnDirectoryEntry e =
                new FatLfnDirectoryEntry(this, real, name);
        
        try {
            dir.addEntries(e.compactForm());
        } catch (IOException ex) {
            final ClusterChain cc =
                    new ClusterChain(fat, real.getStartCluster(), false);
            cc.setChainLength(0);
            dir.removeEntry(real);
            throw ex;
        }
        
        shortNameIndex.put(sn, e);
        longNameIndex.put(name.toLowerCase(), e);

        getDirectory(real);
        
        flush();
        return e;
    }
    
    /**
     * <p>
     * {@inheritDoc}
     * </p><p>
     * According to the FAT file system specification, leading and trailing
     * spaces in the {@code name} are ignored by this method.
     * </p>
     *
     * @param name {@inheritDoc}
     * @return {@inheritDoc}
     */
    @Override
    public FatLfnDirectoryEntry getEntry(String name) {
        name = name.trim().toLowerCase();
        
        final FatLfnDirectoryEntry entry = longNameIndex.get(name);
        
        if (entry == null) {
            if (!ShortName.canConvert(name)) return null;
            return shortNameIndex.get(ShortName.get(name));
        } else {
            return entry;
        }
    }
    
    private void parseLfn() throws IOException {
        int i = 0;
        final int size = dir.getEntryCount();
        
        while (i < size) {
            // jump over empty entries
            while (i < size && dir.getEntry(i) == null) {
                i++;
            }

            if (i >= size) {
                break;
            }

            int offset = i; // beginning of the entry
            // check when we reach a real entry
            while (dir.getEntry(i).isLfnEntry()) {
                i++;
                if (i >= size) {
                    // This is a cutted entry, forgive it
                    break;
                }
            }
            
            if (i >= size) {
                // This is a cutted entry, forgive it
                break;
            }
            
            final FatLfnDirectoryEntry current =
                    FatLfnDirectoryEntry.extract(this, offset, ++i - offset);
            
            if (!current.realEntry.isDeleted() && current.isValid()) {
                checkUniqueName(current.getName());
                
                shortNameIndex.put(current.realEntry.getShortName(), current);
                longNameIndex.put(current.getName().toLowerCase(), current);
            }
        }
    }
    
    private void updateLFN() throws IOException {
        ArrayList<FatDirectoryEntry> dest =
                new ArrayList<FatDirectoryEntry>();

        for (FatLfnDirectoryEntry currentEntry : shortNameIndex.values()) {
            FatDirectoryEntry[] encoded = currentEntry.compactForm();
            dest.addAll(Arrays.asList(encoded));
        }
        
        final int size = dest.size();

        dir.changeSize(size);
        dir.setEntries(dest);
    }

    @Override
    public void flush() throws IOException {
        checkWritable();
        
        for (FatFile f : entryToFile.values()) {
            f.flush();
        }
        
        for (FatLfnDirectory d : entryToDirectory.values()) {
            d.flush();
        }
        
        updateLFN();
        dir.flush();
    }

    @Override
    public Iterator<FsDirectoryEntry> iterator() {
        return new Iterator<FsDirectoryEntry>() {

            final Iterator<FatLfnDirectoryEntry> it =
                    shortNameIndex.values().iterator();

            @Override
            public boolean hasNext() {
                return it.hasNext();
            }

            @Override
            public FsDirectoryEntry next() {
                return it.next();
            }

            /**
             * @see java.util.Iterator#remove()
             */
            @Override
            public void remove() {
                throw new UnsupportedOperationException();
            }
        };
    }

    /**
     * Remove the entry with the given name from this directory.
     * 
     * @param name the name of the entry to remove
     * @throws IOException on error removing the entry
     * @throws IllegalArgumentException on an attempt to remove the dot entries
     */
    @Override
    public void remove(String name)
            throws IOException, IllegalArgumentException {
        
        checkWritable();
        
        final FatLfnDirectoryEntry entry = getEntry(name);
        if (entry == null) return;
        
        unlinkEntry(entry);
        
        final ClusterChain cc = new ClusterChain(
                fat, entry.realEntry.getStartCluster(), false);

        cc.setChainLength(0);
        
        freeUniqueName(name);
        updateLFN();
    }
    
    /**
     * Unlinks the specified entry from this directory without actually
     * deleting it.
     *
     * @param e the entry to be unlinked
     * @see #linkEntry(de.waldheinz.fs.fat.FatLfnDirectoryEntry) 
     */
    void unlinkEntry(FatLfnDirectoryEntry entry) {
        final ShortName sn = entry.realEntry.getShortName();
        
        if (sn.equals(ShortName.DOT) || sn.equals(ShortName.DOT_DOT)) throw
                new IllegalArgumentException(
                    "the dot entries can not be removed");

        final String lowerName = entry.getName().toLowerCase();

        assert (this.longNameIndex.containsKey(lowerName));
        this.longNameIndex.remove(lowerName);
        
        assert (this.shortNameIndex.containsKey(sn));
        this.shortNameIndex.remove(sn);
        
        if (entry.isFile()) {
            this.entryToFile.remove(entry.realEntry);
        } else {
            this.entryToDirectory.remove(entry.realEntry);
        }
    }
    
    /**
     * Links the specified entry to this directory, updating the entrie's
     * short name.
     *
     * @param entry the entry to be linked (added) to this directory
     * @see #unlinkEntry(de.waldheinz.fs.fat.FatLfnDirectoryEntry) 
     */
    void linkEntry(FatLfnDirectoryEntry entry) throws IOException {
        checkUniqueName(entry.getName());
        ShortName name;
        name = this.dbg.generate83BufferNew(entry.getName());
        entry.realEntry.setShortName(name);
        
        this.longNameIndex.put(entry.getName().toLowerCase(), entry);
        this.shortNameIndex.put(entry.realEntry.getShortName(), entry);
        
        updateLFN();
    }
    
    @Override
    public String toString() {
        return getClass().getSimpleName() +
                " [size=" + shortNameIndex.size() + //NOI18N
                ", dir=" + dir + "]"; //NOI18N
    }
    
    private static ClusterChainDirectory read(FatDirectoryEntry entry, Fat fat)
            throws IOException {

        if (!entry.isDirectory()) throw
                new IllegalArgumentException(entry + " is no directory");

        final ClusterChain chain = new ClusterChain(
                fat, entry.getStartCluster(),
                entry.isReadonlyFlag());

        final ClusterChainDirectory result =
                new ClusterChainDirectory(chain, false);

        result.read();
        return result;
    }
    
}
