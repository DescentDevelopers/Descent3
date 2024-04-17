# Descent 3

## Update 04/16/2024

I'm so happy to see the amount of interest and participation here! 

Please join the Descent Developer Discord, there's an active community there already. 

[https://discord.gg/GNy5CUQ](https://discord.gg/GNy5CUQ)
~~Please join our brand new Discord server with this link [https://discord.gg/xxxx](https://discord.gg/xxx)~~


You can expect some big commits coming soon. We'll be merging in some code that other developers did in parallel and/or after this code was archived.

## Direction and Decisions

1. We will support c++17 going forward
2. We are using clang in LLVM mode for code formatting. Please run clang-format before submitting a PR
   

## Original Release
This is the latest version of the Descent 3 source code. This includes the '1.5' patch that Jeff Slutter and Kevin Bentley wrote several years ago. At the time, it worked for Windows, Linux, and Mac. 

Some proprietary sound and video libraries from Interplay have been stripped out (the ACM and MVE format). I have that code if someone wants to help make a converter so the old cutscenes work. It'll take some effort to stub out that code so it compiles. 

The first thing I want to do is get everything compiling again, and ideally some CI/CD actions. After that, the code needs to be cleaned up some, to remove old version control comments, etc. A lot of this code was written by a really great team, but keep in mind we were much younger and less experienced back then.

If you're interested in helping maintain it, please send me a message. Otherwise, I'm happy to take pull requests.

This is the last update I put out there showing different architectures playing along. Yikes, that was a long time ago, sorry we never released a 1.5 patch. Some logistics got in the way!

[![Descent3 1.5 Patch Development update](https://img.youtube.com/vi/oasEAoPHk7I/0.jpg)](https://www.youtube.com/watch?v=oasEAoPHk7I)

Thanks to Jeff Slutter, who did most of the work modernizing the code from the 90's. I'm looking forward to seeing what the community does with it!
