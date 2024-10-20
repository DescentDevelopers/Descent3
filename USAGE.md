# Descent 3 Open source usage instructions

**Important note**: This open source distribution of Descent 3 DOES NOT CONTAIN
GAME ASSETS. Assets must be acquired separately from an official copy of the
game, and copied as described in the next section.

This is the first release of the Descent 3 open source engine, that should be
considered a beta version. If you find a bug that has not been reported before,
please open a new ticket it on our [online issue tracker](https://github.com/DescentDevelopers/Descent3/issues).

1. Make sure that you have a copy of Descent 3. You can purchase a copy of
Descent 3 from [GOG](https://www.gog.com/game/descent_3_expansion) or
[Steam](https://store.steampowered.com/app/273590/Descent_3/).

2. Install Descent 3.
**Note for Steam users:** If you own Descent 3 on Steam, then it’s recommended
that you install the Windows version of the game even if you’re running macOS
or Linux, otherwise movies will not work due to
[current lack of OGV support](https://github.com/DescentDevelopers/Descent3/issues/240).
You can use either [Steam Play](https://help.steampowered.com/en/faqs/view/08F7-5D56-9654-39AF)
or [SteamCMD](https://developer.valvesoftware.com/wiki/SteamCMD#Cross-Platform_Installation)
to install the Windows version of the game on macOS or Linux.

3. If your version of Descent 3 is older than v1.4, then
[update it to v1.4](http://descent3.com/downloads.php).

4. Find the installation location of Descent 3. Using the Steam client, you can
find it from the library page using the `Manage > Browse local files`
context menu.

5. Create a new folder named `D3-open-source`.

6. Copy the following files from your installation of Descent 3 to `D3-open-source`:
    - All `.hog` files
    - The `missions` folder
    - _(Optional)_ All `.pld` files
    - _(Optional)_ The `movies` folder

7. Create the `custom/` folder in `D3-open-source`

8. Obtain new Descent 3 engine files:
    - If you want to use pre-built binaries, then download the latest
      [release](https://github.com/DescentDevelopers/Descent3/releases). For a
      more cutting-edge experience with the latest features, use the artifacts
      from the latest automated build. You can find the list of automated
      builds [here](https://github.com/DescentDevelopers/Descent3/actions/workflows/build.yml?query=branch%3Amain+event%3Apush).
    - If you want to build the engine files yourself, the follow the
      instructions in [BUILD.md](BUILD.md). Once you build the engine files,
      they’ll be put in `builds/<platform>/build/<build-type>/`. For example, if
      you’re using Linux and you create a “Release” build, then the files will
      be located at `builds/linux/build/Release`.

9. Copy all the new engine files into `D3-open-source` and overwrite any
conflicts.

10. Special notes:
    - D3 Open Source compiles level scripts in their own hogfiles. Make sure
      you copy and overwrite `d3-{platform}.hog`.

11. Run the game:
    - On Windows, run `D3-open-source\Descent3.exe` from a command-line or
      double-click on the `Descent3` executable.
    - On Linux, `cd` to `D3-open-source` and run `./Descent3`. Wayland users
      may need to set environment variable `SDL_VIDEODRIVER=wayland` before
      launching the game.
    - On macOS, the `.app` bundle is currently not signed, so your operating
      system will not let you run it by double-clicking it. To remediate that,
      open your terminal and `cd` to `D3-open-source`. Run
      `xattr -c ./Descent3.app`, `xattr -c ./netgames/*`,
      `chmod +x ./Descent3.app/Contents/MacOS/Descent3`, and then run the game
      using `./Descent3.app/Contents/MacOS/Descent3`

## Troubleshooting

```
Descent 3 Message(Error: Couldn't find the string table.)
```

This error means that game data could not be found. Make sure you copied all
game files to the `D3-open-source` folder, and that you're running the game
from this same folder.

## Base directories

A base directory is a directory that Descent 3 expects game files to be in. When you run Descent 3, it will try to access many different files. Most of those files need to be stored in a base directory. There are two different types of files that are stored in base directories:

- Read-write files are files that can change while you play Descent 3. Examples: `<you name>.plt` and files in the `savegame/` directory.
- Read-only files are files that do not change while you play Descent 3. Examples: `d3.hog` and files in the `movies/` directory.

Descent 3 has two types of base directories:

- The writable base directory can contain both read-write files and read-only files. There is only one writeable base directory. By default, the writable base directory gets set to the current working directory.
- The read-only base directories can only contain read-only files. There can be any number of read-only base directories. By default, Descent 3 uses zero read-only base directories.

You can set the writable base directory and the list of read-only base directories using the `-setdir`, `-useexedir` and `-additionaldir` command-line options (see [the next section](#command-line-options)).

When Descent 3 tries to find a read-only file, then it will look through the list of base directories in this order:

- the last read-only base directory that was specified on the command-line,
- the second-to-last read-only base directory that was specified on the command-line,
- the third-to-last read-only base directory that was specified on the command-line,
- …
- the first read-only base directory that was specified on the command-line and, finally,
- the writable base directory.

Files that are in base directories that are higher on that list will override files that are in base directories that are lower on that list. For example, lets say that you run Descent 3 like this:

```
Descent3 -setdir /home/user/my-writable-base-directory -additionaldir /home/user/my-read-only-base-directory
```

Let’s also say that both `my-writable-base-directory` and `my-read-only-base-directory` contain a file named `d3.hog`. In this example, Descent 3 will load `/home/user/my-read-only-base-directory/d3.hog` because read-only directories have a higher precedence than the writable base directory. Descent 3 will ignore `/home/user/my-writable-base-directory/d3.hog`.

## Command-Line Options

The following command-line options are available in Descent 3. You can set command-line options on the Misc. tab of the Setup section of the Descent 3 launcher or by creating a shortcut to `Descent3.exe`. Case is not significant in command-line options, and `-`, `--`, and `+` are all accepted.

### Display Options

- `-aspect <value>`

    **Type:** floating-point number

    **Default:** 1.3333333333333333

    **Platform:** all

    **Description:** Specifies the screen aspect ratio for non-standard displays, such as wide-screen TVs.

- `-display <number>`

    **Type:** integer

    **Default:** 0

    **Platforms:** all

    **Description:** Run game on the selected display.

- `-fullscreen` or `-f`

    **Type:** boolean

    **Default:** On

    **Platform:** all

    **Description:** Run game in fullscreen mode.

- `-height <height>`

    **Type:** integer

    **Default:** 480 unless you select a different value in the options menu

    **Platform:** all

    **Description:** Sets the screen resolution to the specified height, if possible.

- `-himem`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Forces normal operations even when low memory conditions are detected.

- `-lowmem`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Uses scaled-down textures and lower quality (8-bit) sounds to conserve memory.

- `-NoRenderWindows`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Causes all windows to be fully transparent. Use this option if your card does not correctly render partially-transparent windows.

- `-superlowmem`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Uses the `-lowmem` settings and further scales down textures to conserve memory.

- `-vsync`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Turns on Vertical Sync. The flag will be enabled in the registry so it will be on when the game is run again.

- `-width <width>`

    **Type:** integer

    **Default:** 640 unless you select a different value in the options menu

    **Platform:** all

    **Description:** Sets the screen resolution to the specified width, if possible.

- `-windowed` or `-w`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Runs the game in a window.

### Audio Options

- `-nomusic`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables music.

- `-nosound`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables all sound, including music.

### Input Options

- `-deadzone# <k>`

    **Type:** `#` is either the character `0` or the character `1`. `k` is a floating-point number.

    **Default:** 0.2

    **Platform:** all

    **Description:** Specifies the size of the deadzone for a joystick.

- `-mlooksens <scale>`

    **Type:** floating-point number

    **Default:** 9.102

    **Platform:** all

    **Description:** Determines how much the player moves when the mouse is moved.

- `-mousesens <scale>`

    **Type:** floating-point number

    **Default:** 1.0

    **Platform:** all

    **Description:** Adjusts the sensitivity of the mouse when not using mouselook mode.

- `-nomousegrab` or `-m`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disable mouse capture.

### Performance Options

- `-fastdemo`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Causes a demo to play back at the highest speed your computer is capable of.

- `-forcelightmaps`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Forces the use of lightmaps, even the Default Detail Level is set to Low in the launcher setup.

- `-framecap <fps>`

    **Type:** integer

    **Default:** 60

    **Platform:** all

    **Description:** Limits the framerate to the number of frames per second specified.

- `-nomotionblur`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables motion blur on robots (Pentium III only).

- `-nosatomega`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables alpha saturation on the omega cannon effect.

- `-nosparkles`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables powerup sparkles (Pentium III only).

### Multiplayer and Network Options

- `-audiotauntdelay <time>`

    **Type:** floating-point number

    **Default:** 5.0

    **Platform:** all

    **Description:** Sets the time in seconds the a user must wait after sending an audio taunt before he or she is able to send another. This option is only active when starting a server.

- `-autoexec <file>`

    **Type:** path

    **Default:** `<writable-base-directory>/netgames/autoexec.dmfc`

    **Platform:** all

    **Description:** Specifies the full path and file name of the multiplayer config file to be loaded and executed when a multiplayer game is initialized.

- `-dedicated <config file>` or `-d <config file>`

    **Type:** path

    **Default:** None

    **Platform:** all

    **Description:** Starts a dedicated server.

- `-gamespyport <port>`

    **Type:** integer

    **Default:** 20142

    **Platform:** all

    **Description:** Specifies a port for to listen for GameSpy requests.

- `-gspyfile <config file>`

    **Type:** path

    **Default:** gamespy.cfg

    **Platform:** all

    **Description:** Specifies a GameSpy configuration file to use.

- `-httpproxy <addr>` or `-httpproxy <addr:port>`

    **Type:** string

    **Default:** None

    **Platform:** all

    **Description:** Specifies an HTTP proxy server. Descent 3 uses HTTP to auto-download a mission; use this option if your ISP requires a proxy server for HTTP connections. If the first form is used, the port value defaults to 80.

- `-nooutragelogo`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables the Outrage logo that is normally displayed for five seconds at the start of a multiplayer game.

- `-nomultibmp`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables sending or receiving in-game custom bitmaps for ships.

- `-nonetwork`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables all network multiplayer functionality.

- `-playermessages`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Disables all weapon-related HUD messages in multiplayer games.

- `-pxoport <port>`

    **Type:** integer

    **Default:** 20142

    **Platform:** all

    **Description:** Specifies the port that PXO will tell clients to use when contacting a server. The default is 2092.

- `-useip <IP>`

    **Type:** string

    **Default:** All available network interfaces

    **Platform:** all

    **Description:** Binds Descent 3 to this IP address. Use this option to tell D3 which IP address to use if your computer has multiple IP addresses.

- `-useport <port>`

    **Type:** integer

    **Default:** 2092

    **Platform:** all

    **Description:** Specifies the port that TCP/IP and IPX will use.

- `-usesmoothing`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Enables code to smooth the interpolation for fast-moving objects. This will fix "skipping" problems with fast weapons, such as the Phoenix. This option applies to clients only; using it on the server has no effect.

### Base Directory Options

- `-additionaldir <path>`

    **Type:** path

    **Default:** None

    **Platform:** all

    **Description:** Adds a directory to the list of read-only base directories. This options can be used multiple times to add multiple directories to the list.

- `-setdir <path>`

    **Type:** path

    **Default:** .

    **Platform:** all

    **Description:** Sets the writable base directory.

- `-useexedir`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Tells Descent 3 to use the directory in which the executable is located as the writable base directory.

### Other Options

- `-logfile`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Generates a logfile `d3.log` if using the Debug build. All `mprintf` statements output to the logfile.

- `-loglevel <LEVEL>`

    **Type:** string

    **Default:** INFO (on Release), DEBUG (on Debug)

    **Platform:** all

    **Description:** Set log level (NONE, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)

- `-makemovie`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Causes the demo system to save a screenshot of every frame during playback.

- `-mission <name>`

    **Type:** string

    **Default:** None

    **Platform:** all

    **Description:** Loads the specified mission file at startup.

- `-pilot <name>`

    **Type:** string

    **Default:** None

    **Platform:** all

    **Description:** Specifies the pilot to use, skipping the pilot selection dialog when the game starts.

- `-service`

    **Type:** boolean

    **Default:** Off

    **Platform:** all

    **Description:** Run game in service mode.

- `-winconsole`

    **Type:** boolean

    **Default:** Off

    **Platform:** WIN

    **Description:** Enable windows console (off by default).
