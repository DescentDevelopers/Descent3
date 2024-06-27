/*
 * Copyright (C) 2012 The Android Open Source Project 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at 
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and 
 * limitations under the License.
 */

package com.android.jobb;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;

public class ObbFile {
    public static final int  OBB_OVERLAY =        (1 << 0);
    public static final int  OBB_SALTED =         (1 << 1);
    
    static final int kFooterTagSize = 8;   /* last two 32-bit integers */

    static final int kFooterMinSize = 33;  /* 32-bit signature version (4 bytes)
                                            * 32-bit package version (4 bytes)
                                            * 32-bit flags (4 bytes)
                                            * 64-bit salt (8 bytes)
                                            * 32-bit package name size (4 bytes)
                                            * >=1-character package name (1 byte)
                                            * 32-bit footer size (4 bytes)
                                            * 32-bit footer marker (4 bytes)
                                            */

    static final int kMaxBufSize = 32768;       /* Maximum file read buffer */

    static final long kSignature  = 0x01059983;  /* ObbFile signature */

    static final int kSigVersion = 1;           /* We only know about signature version 1 */

    /* offsets in version 1 of the header */
    static final int kPackageVersionOffset = 4;
    static final int kFlagsOffset          = 8;
    static final int kSaltOffset           = 12;
    static final int kPackageNameLenOffset = 20;
    static final int kPackageNameOffset    = 24;
    
    long mPackageVersion = -1, mFlags;
    String mPackageName;
    byte[] mSalt = new byte[8];
    
    public ObbFile() {}
    
    public boolean readFrom(String filename)
    {
        File obbFile = new File(filename);
        return readFrom(obbFile);
    }

    public boolean readFrom(File obbFile)
    {
        return parseObbFile(obbFile);
    }

    static public long get4LE(ByteBuffer buf) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        return (buf.getInt() & 0xFFFFFFFFL);
    }
    
    public void setPackageName(String packageName) {
        mPackageName = packageName;
    }
    
    public void setSalt(byte[] salt) {
        if ( salt.length != mSalt.length ) {
            throw new RuntimeException("salt must be " + mSalt.length + " characters in length");
        }
        System.arraycopy(salt, 0, mSalt, 0, mSalt.length);
    }
    
    public void setPackageVersion(long packageVersion) {
        mPackageVersion = packageVersion;
    }
    
    public void setFlags(long flags) {
        mFlags = flags;
    }
        
    public boolean parseObbFile(File obbFile)
    {
        try {
            long fileLength = obbFile.length();
    
            if (fileLength < kFooterMinSize) {
                throw new RuntimeException("file is only " + fileLength + " (less than " + kFooterMinSize + " minimum)");            
            }
    
            RandomAccessFile raf = new RandomAccessFile(obbFile, "r");
            raf.seek(fileLength - kFooterTagSize);
            byte[] footer = new byte[kFooterTagSize];
            raf.readFully(footer);
            ByteBuffer footBuf = ByteBuffer.wrap(footer);
            footBuf.position(4);
            long fileSig = get4LE(footBuf);
            if (fileSig != kSignature) {
                throw new RuntimeException("footer didn't match magic string (expected 0x" + Long.toHexString(kSignature) + ";got 0x" +
                        Long.toHexString(fileSig)+ ")");
            }
            
            footBuf.rewind();
            long footerSize = get4LE(footBuf);
            if (footerSize > fileLength - kFooterTagSize
                        || footerSize > kMaxBufSize) {
                throw new RuntimeException("claimed footer size is too large (0x" + Long.toHexString(footerSize) + "; file size is 0x" +
                        Long.toHexString(fileLength)+ ")");
            }
    
            if (footerSize < (kFooterMinSize - kFooterTagSize)) {
                throw new RuntimeException("claimed footer size is too small (0x" + Long.toHexString(footerSize) + "; minimum size is 0x" +
                        Long.toHexString(kFooterMinSize - kFooterTagSize));
            }
    
            long fileOffset = fileLength - footerSize - kFooterTagSize;
            raf.seek(fileOffset);
            
            footer = new byte[(int)footerSize];
            raf.readFully(footer);
            footBuf = ByteBuffer.wrap(footer);
            
            long sigVersion = get4LE(footBuf);
            if (sigVersion != kSigVersion) {
                throw new RuntimeException("Unsupported ObbFile version " + sigVersion );
            }
            
            footBuf.position(kPackageVersionOffset);
            mPackageVersion = get4LE(footBuf);
            footBuf.position(kFlagsOffset);
            mFlags = get4LE(footBuf);
            
            footBuf.position(kSaltOffset);
            footBuf.get(mSalt);
            footBuf.position(kPackageNameLenOffset);
            long packageNameLen = get4LE(footBuf);
            if (packageNameLen == 0
                    || packageNameLen > (footerSize - kPackageNameOffset)) {
                throw new RuntimeException("bad ObbFile package name length (0x" + Long.toHexString(packageNameLen) +
                        "; 0x" + Long.toHexString(footerSize - kPackageNameOffset) + "possible)");
            }
            byte[] packageNameBuf = new byte[(int)packageNameLen];
            footBuf.position(kPackageNameOffset);
            footBuf.get(packageNameBuf);
    
            mPackageName = new String(packageNameBuf);
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }
    
    public boolean writeTo(String fileName)
    {
        File obbFile = new File(fileName);
        return writeTo(obbFile);
    }
    
    public boolean writeTo(File obbFile) {
        if ( !obbFile.exists() )
            return false;
        
        try {
    
            long fileLength = obbFile.length();
            RandomAccessFile raf = new RandomAccessFile(obbFile, "rw");
            raf.seek(fileLength);
            
            if (null == mPackageName || mPackageVersion == -1) {
                throw new RuntimeException("tried to write uninitialized ObbFile data");
            }
    
            FileChannel fc = raf.getChannel();
            ByteBuffer bbInt = ByteBuffer.allocate(4);
            bbInt.order(ByteOrder.LITTLE_ENDIAN);
            bbInt.putInt(kSigVersion);
            bbInt.rewind();
            fc.write(bbInt);
            
            bbInt.rewind();
            bbInt.putInt((int)mPackageVersion);
            bbInt.rewind();
            fc.write(bbInt);
    
            bbInt.rewind();
            bbInt.putInt((int)mFlags);
            bbInt.rewind();
            fc.write(bbInt);
            
            raf.write(mSalt);
    
            bbInt.rewind();
            bbInt.putInt(mPackageName.length());
            bbInt.rewind();
            fc.write(bbInt);
            
            raf.write(mPackageName.getBytes());
            
            bbInt.rewind();
            bbInt.putInt(mPackageName.length()+kPackageNameOffset);
            bbInt.rewind();
            fc.write(bbInt);
    
            bbInt.rewind();
            bbInt.putInt((int)kSignature);
            bbInt.rewind();
            fc.write(bbInt);
            
            raf.close();    
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }    
}
