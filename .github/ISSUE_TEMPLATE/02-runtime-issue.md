---
name: Runtime Bug/Crash
about: Report a bug or crash at runtime in any supported configuration.
labels: 'bug'
title: '[Runtime Bug/Crash] '

---
<!--
Use this template if a current build has a bug or crashes at runtime.  If your problem happens at build time, please use the issue template `Failure to build`.
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

#### Game Environment
<!--
If the issue is specific to a particular mission, give the name of the campaign and the level of the mission within that campaign. 
If the campaign is not one of the core assets (`Descent 3: Retribution`, `Descent 3: Mercenary`, or any of the other missions provided with the GOG and Steam release), give a download link to the campaign.

If the issue occurs at some particular place in the level, give a description how to reach that point from the beginning of the level. Assume that the maintainer  is not familiar with the optimal route to get from the start point to the affected location.
-->

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
Provide:
- Expected behavior
- Observed behavior
- If possible, describe the frequency of the problem.  Does it happen every time the steps to reproduce are followed?  If it is intermittent, are there any events correlated with the error?
- If the game produced any error messages, include their text verbatim.  If you paraphrase the message, you will likely be asked to reproduce the error and collect a verbatim copy of the text.
- For in-game problems, indicate whether it happens in single player, multiplayer cooperative, or multiplayer competitive. You do not need to check every combination before filing, but please report which combinations you checked and the results you found for those combinations.
- If the game crashed and a crash dump was created, include the dump backtrace.
-->

* Game modes affected:
  * [ ] Single player
  * [ ] Multiplayer competitive
    * [ ] Anarchy
	* [ ] Hyper-Anarchy
	* [ ] Robo-Anarchy
    * [ ] Team Anarchy
    * [ ] Capture the Flag
    * [ ] Bounty
	* [ ] Entropy
	* [ ] Hoard
	* [ ] Monsterball
  * [ ] Multiplayer cooperative

<!--
If the issue is only observed in single player, delete this next group.
-->
* Players affected by issue:
  * [ ] Game host
  * [ ] Game guests