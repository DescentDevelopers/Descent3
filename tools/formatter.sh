#these are the relevant file extensions I know of off the top of my head they should be supported by clang-forma
#copyright(c) Martin Johan Smouter 2024
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE
for file in $(find ./ -depth -name *\.\c -o -name *\.\cpp -o -name -o -name *\.\h -o -name *\.\hpp -o -name *\.\js -o -name *\.\json -o -name *\.\java -o -name *\.\cs);
do
{
  clang-format-14 --style=LLVM $file > $file;
  clang-format-15 --style=LLVM $file > $file;
	clang-format-16 --style=LLVM $file > $file;
	clang-format-17 --style=LLVM $file > $file;
	clang-format-18 --style=LLVM $file > $file;
	clang-format-19 --style=LLVM $file > $file;
	clang-format-20 --style=LLVM $file > $file;
	clang-format-21 --style=LLVM $file > $file;
	echo $file;
}
done;
