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

import de.waldheinz.fs.*;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.zip.GZIPInputStream;

/**
 * A {@link BlockDevice} that lives entirely in heap memory. This is basically
 * a RAM disk. A {@code RamDisk} is always writable.
 *
 * @author Matthias Treydte &lt;waldheinz at gmail.com&gt;
 */
public final class RamDisk implements BlockDevice {
    
    /**
     * The default sector size for {@code RamDisk}s.
     */
    public final static int DEFAULT_SECTOR_SIZE = 512;
    
    private final int sectorSize;
    private final ByteBuffer data;
    private final int size;
    private boolean closed;

    /**
     * Reads a GZIP compressed disk image from the specified input stream and
     * returns a {@code RamDisk} holding the decompressed image.
     *
     * @param in the stream to read the disk image from
     * @return the decompressed {@code RamDisk}
     * @throws IOException on read or decompression error
     */
    public static RamDisk readGzipped(InputStream in) throws IOException {
        final GZIPInputStream zis = new GZIPInputStream(in);
        ByteArrayOutputStream bos = new ByteArrayOutputStream();
        
        final byte[] buffer = new byte[4096];
        
        int read = zis.read(buffer);
        int total = 0;
        
        while (read >= 0) {
            total += read;
            bos.write(buffer, 0, read);
            read = zis.read(buffer);
        }

        if (total < DEFAULT_SECTOR_SIZE) throw new IOException(
                "read only " + total + " bytes"); //NOI18N
                
        final ByteBuffer bb = ByteBuffer.wrap(bos.toByteArray(), 0, total);
        return new RamDisk(bb, DEFAULT_SECTOR_SIZE);
    }
    
    private RamDisk(ByteBuffer buffer, int sectorSize) {
        this.size = buffer.limit();
        this.sectorSize = sectorSize;
        this.data = buffer;
        this.closed = false;
    }

    /**
     * Creates a new instance of {@code RamDisk} of this specified
     * size and using the {@link #DEFAULT_SECTOR_SIZE}.
     *
     * @param size the size of the new block device
     */
    public RamDisk(int size) {
        this(size, DEFAULT_SECTOR_SIZE);
    }

    /**
     * Creates a new instance of {@code RamDisk} of this specified
     * size and sector size
     *
     * @param size the size of the new block device
     * @param sectorSize the sector size of the new block device
     */
    public RamDisk(int size, int sectorSize) {
        if (sectorSize < 1) throw new IllegalArgumentException(
                "invalid sector size"); //NOI18N
        
        this.sectorSize = sectorSize;
        this.size = size;
        this.data = ByteBuffer.allocate(size);
    }
    
    @Override
    public long getSize() {
        checkClosed();
        return this.size;
    }

    @Override
    public void read(long devOffset, ByteBuffer dest) throws IOException {
        checkClosed();
        
        if (devOffset > getSize()){
            final StringBuilder sb = new StringBuilder();
            sb.append("read at ").append(devOffset);
            sb.append(" is off size (").append(getSize()).append(")");
            
            throw new IllegalArgumentException(sb.toString());
        }
        
        data.limit((int) (devOffset + dest.remaining()));
        data.position((int) devOffset);
        
        dest.put(data);
    }

    @Override
    public void write(long devOffset, ByteBuffer src) throws IOException {
        checkClosed();
        
        if (devOffset + src.remaining() > getSize()) throw new
                IllegalArgumentException(
                "offset=" + devOffset +
                ", length=" + src.remaining() +
                ", size=" + getSize());
                
        data.limit((int) (devOffset + src.remaining()));
        data.position((int) devOffset);
        
        
        data.put(src);
    }
    
    /**
     * Returns a slice of the {@code ByteBuffer} that is used by this
     * {@code RamDisk} as it's backing store. The returned buffer will be
     * live (reflecting any changes made through the
     * {@link #write(long, java.nio.ByteBuffer) method}, but read-only.
     *
     * @return a buffer holding the contents of this {@code RamDisk}
     */
    public ByteBuffer getBuffer() {
        return this.data.asReadOnlyBuffer();
    }
    
    @Override
    public void flush() throws IOException {
        checkClosed();
    }
    
    @Override
    public int getSectorSize() {
        checkClosed();
        return this.sectorSize;
    }

    @Override
    public void close() throws IOException {
        this.closed = true;
    }

    @Override
    public boolean isClosed() {
        return this.closed;
    }

    private void checkClosed() {
        if (closed) throw new IllegalStateException("device already closed");
    }

    /**
     * Returns always {@code false}, as a {@code RamDisk} is always writable.
     *
     * @return always {@code false}
     */
    @Override
    public boolean isReadOnly() {
        checkClosed();
        
        return false;
    }
    
}
