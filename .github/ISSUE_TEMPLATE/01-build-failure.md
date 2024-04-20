---
name: Build Failure
about: Report a failure to build current code in any supported configuration.
labels: 'build-failure'
title: '[Build Failure] '

---
<!--
Use this template if the code in main fails to build for you.  If your problem happens at runtime, please use the issue template `Runtime Crash` or the issue template `Runtime Bug`, as appropriate.

Please use a descriptive title. Include in the title the _first_ error message. Attach your buildlog if possible.
-->

### Build Version
<!--
If you fetched the source from Git, state the Git commit you used, preferably as the full 40-digit commit hash.  Please do **not** say "HEAD", "current", or similar relative references.  
The meaning of relative references can change as contributors publish new code.  The 40-digit commit hash will not change.
-->

#### Operating System Environment
<!--
State what host platform (Microsoft Windows, Mac OS X, or Linux) you tried.  If you tried multiple, list all of them.
-->
* [ ] Microsoft Windows (32-bit)
* [ ] Microsoft Windows (64-bit)
* [ ] Mac OS X
<!--
* For Linux, give the name of the distribution. Add versions as needed.
-->
* Ubuntu
  * [ ] Ubuntu 16.04 LTS (Xenial Xerus)
  * [ ] Ubuntu 18.04 LTS (Bionic Beaver)
  * [ ] Ubuntu 18.10 (Cosmic Cuttlefish)
  * [ ] Ubuntu 19.04 (Disco Dingo)
  * [ ] Ubuntu 19.10 (Eoan Ermine)
  * [ ] Ubuntu 20.04 LTS (Focal Fossa)
  * [ ] Ubuntu 22.04 LTS (Jammy Jellyfish)

#### CPU Environment
<!--
Indicate which CPU families were targeted.  Some bugs are only visible on certain architectures, since other architectures hide the consequences of the mistake.
If unsure, omit this section.  Generally, if you are on an architecture that requires special consideration, you will know your architecture.
-->
* [ ] x86 (32-bit Intel/AMD)
* [ ] x86\_64 (64-bit Intel/AMD)
* [ ] ARM (32-bit)
* [ ] ARM64 (64-bit; sometimes called AArch64)

### Description
<!--
Describe the issue here.
-->

#### Regression Status
<!--
What is the oldest Git commit known to present the problem?  What is the newest Git commit known not to present the problem?
-->

### Steps to Reproduce
<!--
If you performed build steps outside of the build steps listed in the README.md file, please list them here.
-->
