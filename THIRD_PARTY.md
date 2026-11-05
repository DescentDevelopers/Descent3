# Third party components

This file contains information about third party components that are required
for building Descent 3.

## libacm

libacm - library for InterPlay ACM Audio format. https://github.com/markokr/libacm

* third_party/libacm/decode.c
* third_party/libacm/libacm.h

The libacm core code is licensed under minimal BSD/ISC license.

```
Copyright (c) 2004-2010, Marko Kreen

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

## libmve

libmve - Interplay MVE implementation from D2X project. https://github.com/btb/d2x

[@c030c453](https://github.com/btb/d2x/tree/c030c4531ad19f1658ea9635ff4ee6861e1d15e0)

* libmve/decoder8.c
* libmve/decoder16.c
* libmve/decoders.h
* libmve/mve_audio.c
* libmve/mve_audio.h
* libmve/mvelib.c
* libmve/mvelib.h
* libmve/mveplay.c

The libmve code is licensed under GPL-3 license.

```
Copyright (C) 2002-2024 D2X Project

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

## plog

Portable, simple and extensible C++ logging library.

* third_party/plog/*

The plog code is licensed under MIT license.

```
Copyright (c) 2022 Sergey Podobry

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## stb

Single-file public domain (or MIT licensed) libraries for C/C++. https://github.com/nothings/stb

* third_party/stb/stb_image_write.h

The stb code is licensed under MIT license.

```
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## tiny386

i386 CPU and x87 FPU emulator used by the x86 sandbox (x86sandbox/). https://github.com/hchunhui/tiny386

[@7fd4d054](https://github.com/hchunhui/tiny386/tree/7fd4d05400779ddb3e2e29b164f07dde7fdf6f17)

* third_party/tiny386/fpu.c
* third_party/tiny386/fpu.h
* third_party/tiny386/i386.c
* third_party/tiny386/i386.h
* third_party/tiny386/i386ins.def
* third_party/tiny386/simd.inc
* third_party/tiny386/LICENSE

i386.c has one local change, kept as third_party/tiny386/patches/0001-count-rep-iterations-as-steps.patch:
each iteration of a REP string instruction counts as a step, so it counts against the x86 sandbox's
instruction budget, and a REP instruction that runs out of steps stops and resumes later. It has been
submitted upstream as https://github.com/hchunhui/tiny386/pull/64.

The tiny386 code is licensed under the BSD 3-Clause license.

```
Copyright (c) 2024-2025, Chunhui He
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

  * Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

## wasm2c runtime (WABT)

Runtime support for C code generated by wasm2c, used by the x86 sandbox. https://github.com/WebAssembly/wabt

[@bda86b81](https://github.com/WebAssembly/wabt/tree/bda86b81e5b8dfcf75a4a5c1f7ad99c54254223f) (after the 1.0.42 release; it has the table allocator fix for CVE-2026-90648, which the 1.0.42 release does not)

* third_party/wasm2c/wasm-rt.h
* third_party/wasm2c/wasm-rt-impl.c
* third_party/wasm2c/wasm-rt-impl.h
* third_party/wasm2c/wasm-rt-impl-tableops.inc
* third_party/wasm2c/wasm-rt-mem-impl.c
* third_party/wasm2c/wasm-rt-mem-impl-helper.inc
* third_party/wasm2c/wasm-rt-exceptions.h
* third_party/wasm2c/wasm-rt-exceptions-impl.c
* third_party/wasm2c/LICENSE

The C code in x86sandbox/generated/ is produced by wasm2c from the same release.

The wasm2c runtime is licensed under the Apache License, Version 2.0. The full license text is in third_party/wasm2c/LICENSE.
