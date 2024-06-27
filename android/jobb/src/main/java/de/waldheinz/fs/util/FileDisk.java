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

package de.waldheinz.fs.util;

import de.waldheinz.fs.BlockDevice;
import de.waldheinz.fs.ReadOnlyException;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * This is a {@code BlockDevice} that uses a {@link File} as it's backing store.
 *
 * @author Matthias Treydte &lt;matthias.treydte at meetwise.com&gt;
 */
public final class FileDisk implements BlockDevice {

    /**
     * The number of bytes per sector for all {@code FileDisk} instances.
     */
    public final static int BYTES_PER_SECTOR = 512;

    private final RandomAccessFile raf;
    private final FileChannel fc;
    private final boolean readOnly;
    private boolean closed;

    /**
     * Creates a new instance of {@code FileDisk} for the specified
     * {@code File}.
     *
     * @param file the file that holds the disk contents
     * @param readOnly if the file should be opened in read-only mode, which
     *      will result in a read-only {@code FileDisk} instance
     * @throws FileNotFoundException if the specified file does not exist
     * @see #isReadOnly() 
     */
    public FileDisk(File file, boolean readOnly) throws FileNotFoundException {
        if (!file.exists()) throw new FileNotFoundException();

        this.readOnly = readOnly;
        this.closed = false;
        final String modeString = readOnly ? "r" : "rw"; //NOI18N
        this.raf = new RandomAccessFile(file, modeString);
        this.fc = raf.getChannel();
    }

    public FileDisk(RandomAccessFile raf, FileChannel fc, boolean readOnly) {
        this.closed = false;
        this.raf = raf;
        this.fc = fc;
        this.readOnly = readOnly;
    }    
    
    private FileDisk(RandomAccessFile raf, boolean readOnly) {
        this.closed = false;
        this.raf = raf;
        this.fc = raf.getChannel();
        this.readOnly = readOnly;
    }

    /**
     * Creates a new {@code FileDisk} of the specified size. The
     * {@code FileDisk} returned by this method will be writable.
     *
     * @param file the file to hold the {@code FileDisk} contents
     * @param size the size of the new {@code FileDisk}
     * @return the created {@code FileDisk} instance
     * @throws IOException on error creating the {@code FileDisk}
     */
    public static FileDisk create(File file, long size) throws IOException {
        try {
            final RandomAccessFile raf =
                    new RandomAccessFile(file, "rw"); //NOI18N
            raf.setLength(size);
            
            return new FileDisk(raf, false);
        } catch (FileNotFoundException ex) {
            throw new IOException(ex);
        }
    }
    
    @Override
    public long getSize() throws IOException {
        checkClosed();
        
        return raf.length();
    }

    @Override
    public void read(long devOffset, ByteBuffer dest) throws IOException {
        checkClosed();

        int toRead = dest.remaining();
        if ((devOffset + toRead) > getSize()) throw new IOException(
                "reading past end of device");

        while (toRead > 0) {
            final int read = fc.read(dest, devOffset);
            if (read < 0) throw new IOException();
            toRead -= read;
            devOffset += read;
        }
    }

    @Override
    public void write(long devOffset, ByteBuffer src) throws IOException {
        checkClosed();

        if (this.readOnly) throw new ReadOnlyException();
        
        int toWrite = src.remaining();

        if ((devOffset + toWrite) > getSize()) throw new IOException(
                "writing past end of file");

        while (toWrite > 0) {
            final int written = fc.write(src, devOffset);
            if (written < 0) throw new IOException();
            toWrite -= written;
            devOffset += written;
        }
    }

    @Override
    public void flush() throws IOException {
        checkClosed();
    }

    @Override
    public int getSectorSize() {
        checkClosed();
        
        return BYTES_PER_SECTOR;
    }

    @Override
    public void close() throws IOException {
        if (isClosed()) return;

        this.closed = true;
        this.fc.close();
        this.raf.close();
    }
    
    @Override
    public boolean isClosed() {
        return this.closed;
    }

    private void checkClosed() {
        if (closed) throw new IllegalStateException("device already closed");
    }

    @Override
    public boolean isReadOnly() {
        checkClosed();
        
        return this.readOnly;
    }

}
