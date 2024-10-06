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

## Command-Line Options

The following command-line options are available in Descent 3. You can set command-line options on the Misc. tab of the Setup section of the Descent 3 launcher or by creating a shortcut to `main.exe`. Case is not significant in command-line options, and `-`, `--`, and `+` are all accepted.

<table>
  <tr>
    <th>Option</th>
    <th>Type</th>
    <th>Default</th>
    <th>Platform</th>
    <th>Description</th>
  </tr>
  <tr>
    <th colspan="5">Display Options</th>
  </tr>
  <tr>
    <td><code>-aspect &lt;value&gt;</code></td>
    <td>floating-point number</td>
    <td>1.3333333333333333</td>
    <td>all</td>
    <td>Specifies the screen aspect ratio for non-standard displays, such as wide-screen TVs.</td>
  </tr>
  <tr>
    <td><code>-display &lt;number&gt;</code></td>
    <td>integer</td>
    <td>0</td>
    <td>all</td>
    <td>Run game on the selected display.</td>
  </tr>
  <tr>
    <td><code>-fullscreen</code> or <code>-f</code></td>
    <td>boolean</td>
    <td>On</td>
    <td>all</td>
    <td>Run game in fullscreen mode.</td>
  </tr>
  <tr>
    <td><code>-height &lt;height&gt;</code></td>
    <td>integer</td>
    <td>480 unless you select a different value in the options menu</td>
    <td>all</td>
    <td>Sets the screen resolution to the specified height, if possible.</td>
  </tr>
  <tr>
    <td><code>-himem</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Forces normal operations even when low memory conditions are detected.</td>
  </tr>
  <tr>
    <td><code>-lowmem</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Uses scaled-down textures and lower quality (8-bit) sounds to conserve memory.</td>
  </tr>
  <tr>
    <td><code>-NoRenderWindows</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Causes all windows to be fully transparent. Use this option if your card does not correctly render partially-transparent windows.</td>
  </tr>
  <tr>
    <td><code>-superlowmem</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Uses the <code>-lowmem</code> settings and further scales down textures to conserve memory.</td>
  </tr>
  <tr>
    <td><code>-vsync</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Turns on Vertical Sync. The flag will be enabled in the registry so it will be on when the game is run again.</td>
  </tr>
  <tr>
    <td><code>-width &lt;width&gt;</code></td>
    <td>integer</td>
    <td>640 unless you select a different value in the options menu</td>
    <td>all</td>
    <td>Sets the screen resolution to the specified width, if possible.</td>
  </tr>
  <tr>
    <td><code>-windowed</code> or </code>-w</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Runs the game in a window.</td>
  </tr>
  <tr>
    <td><code>-z32bit</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>WIN</td>
    <td>Enables the 32-bit z-buffer on cards that support it, such as the Matrox G400.</td>
  </tr>

  <tr>
    <th colspan="5">Audio Options</th>
  </tr>
  <tr>
    <td><code>-nomusic</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables music.</td>
  </tr>
  <tr>
    <td><code>-nosound</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables all sound, including music.</td>
  </tr>

  <tr>
    <th colspan="5">Input Options</th>
  </tr>
  <tr>
    <td><code>-alternatejoy</code> or <code>-directinput</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>WIN</td>
    <td>Causes DirectInput to be used for the joystick or other game controller, instead of standard Windows API.</td>
  </tr>
  <tr>
    <td><code>-chpro</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Enables a CH Flightstick Pro or compatible joystick.</td>
  </tr>
  <tr>
    <td><code>-deadzone# &lt;k&gt;</code></td>
    <td><code>#</code> is either the character <code>0</code> or the character <code>1</code>. <code>k</code> is a floating-point number.</td>
    <td>0.2</td>
    <td>all</td>
    <td>Specifies the size of the deadzone for a joystick.</td>
  </tr>
  <tr>
    <td><code>-mlooksens &lt;scale&gt;</code></td>
    <td>floating-point number</td>
    <td>9.102</td>
    <td>all</td>
    <td>Determines how much the player moves when the mouse is moved.</td>
  </tr>
  <tr>
    <td><code>-mouseman</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Enables special handling for the Logitech MouseMan.</td>
  </tr>
  <tr>
    <td><code>-mousesens &lt;scale&gt;</code></td>
    <td>floating-point number</td>
    <td>1.0</td>
    <td>all</td>
    <td>Adjusts the sensitivity of the mouse when not using mouselook mode.</td>
  </tr>
  <tr>
    <td><code>-nomousegrab</code> or <code>-m</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disable mouse capture.</td>
  </tr>
  <tr>
    <th colspan="5">Performance Options</th>
  </tr>
  <tr>
    <td><code>-fastdemo</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Causes a demo to play back at the highest speed your computer is capable of.</td>
  </tr>
  <tr>
    <td><code>-forcelightmaps</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Forces the use of lightmaps, even the Default Detail Level is set to Low in the launcher setup.</td>
  </tr>
  <tr>
    <td><code>-framecap &lt;fps&gt;</code></td>
    <td>integer</td>
    <td>60</td>
    <td>all</td>
    <td>Limits the framerate to the number of frames per second specified.</td>
  </tr>
  <tr>
    <td><code>-highvidmem</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Causes 2 MB Voodoo 1 cards to behave as if they had more memory.</td>
  </tr>
  <tr>
    <td><code>-nocompress</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Turns off S3TC texture compression for cards that support it.</td>
  </tr>
  <tr>
    <td><code>-nolightmaps</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables lightmaps. This will improve performance on low-end video cards.</td>
  </tr>
  <tr>
    <td><code>-nomotionblur</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables motion blur on robots (Pentium III only).</td>
  </tr>
  <tr>
    <td><code>-nomultitexture</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables single-pass multitexturing. This option could fix problems due to buggy drivers on future video cards.</td>
  </tr>
  <tr>
    <td><code>-nooutdoorfog</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables fog on the terrain under Direct3D. This may improve performance on some cards.</td>
  </tr>
  <tr>
    <td><code>-nopentium3</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables detection of the Pentium III processor.</td>
  </tr>
  <tr>
    <td><code>-nosatomega</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables alpha saturation on the omega cannon effect.</td>
  </tr>
  <tr>
    <td><code>-subpixelcorrect</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Adjusts for subpixel correction under Direct3D for cards that do not support it.</td>
  </tr>
  <tr>
    <td><code>-nosparkles</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables powerup sparkles (Pentium III only).</td>
  </tr>

  <tr>
    <th colspan="5">Multiplayer and Network Options</th>
  </tr>
  <tr>
    <td><code>-audiotauntdelay &lt;time&gt;</code></td>
    <td>floating-point number</td>
    <td>5.0</td>
    <td>all</td>
    <td>Sets the time in seconds the a user must wait after sending an audio taunt before he or she is able to send another. This option is only active when starting a server.</td>
  </tr>
  <tr>
    <td><code>-autoexec &lt;file&gt;</code></td>
    <td>path</td>
    <td>&lt;writable-base-directory&gt;/netgames/autoexec.dmfc</td>
    <td>all</td>
    <td>Specifies the full path and file name of the multiplayer config file to be loaded and executed when a multiplayer game is initialized.</td>
  </tr>
  <tr>
    <td><code>-dedicated &lt;config file&gt;</code> or <code>-d &lt;config file&gt;</code></td>
    <td>path</td>
    <td>None</td>
    <td>all</td>
    <td>Starts a dedicated server.</td>
  </tr>
  <tr>
    <td><code>-gamespyport &lt;port&gt;</code></td>
    <td>integer</td>
    <td>20142</td>
    <td>all</td>
    <td>Specifies a port for to listen for GameSpy requests.</td>
  </tr>
  <tr>
    <td><code>-gspyfile &lt;config file&gt;</code></td>
    <td>path</td>
    <td>gamespy.cfg</td>
    <td>all</td>
    <td>Specifies a GameSpy configuration file to use.</td>
  </tr>
  <tr>
    <td><code>-httpproxy &lt;addr&gt;</code> or <code>-httpproxy &lt;addr:port&gt;</code></td>
    <td>string</td>
    <td>None</td>
    <td>all</td>
    <td>Specifies an HTTP proxy server. Descent 3 uses HTTP to auto-download a mission; use this option if your ISP requires a proxy server for HTTP connections. If the first form is used, the port value defaults to 80.</td>
  </tr>
  <tr>
    <td><code>-nooutragelogo</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables the Outrage logo that is normally displayed for five seconds at the start of a multiplayer game.</td>
  </tr>
  <tr>
    <td><code>-nomultibmp</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables sending or receiving in-game custom bitmaps for ships.</td>
  </tr>
  <tr>
    <td><code>-nonetwork</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables all network multiplayer functionality.</td>
  </tr>
  <tr>
    <td><code>-playermessages</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables all weapon-related HUD messages in multiplayer games.</td>
  </tr>
  <tr>
    <td><code>-pxoport &lt;port&gt;</code></td>
    <td>integer</td>
    <td>20142</td>
    <td>all</td>
    <td>Specifies the port that PXO will tell clients to use when contacting a server. The default is 2092.</td>
  </tr>
  <tr>
    <td><code>-useip &lt;IP&gt;</code></td>
    <td>string</td>
    <td>All available network interfaces</td>
    <td>all</td>
    <td>Binds Descent 3 to this IP address. Use this option to tell D3 which IP address to use if your computer has multiple IP addresses.</td>
  </tr>
  <tr>
    <td><code>-useport &lt;port&gt;</code></td>
    <td>integer</td>
    <td>2092</td>
    <td>all</td>
    <td>Specifies the port that TCP/IP and IPX will use.</td>
  </tr>
  <tr>
    <td><code>-usesmoothing</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Enables code to smooth the interpolation for fast-moving objects. This will fix "skipping" problems with fast weapons, such as the Phoenix. This option applies to clients only; using it on the server has no effect.</td>
  </tr>

  <tr>
    <th colspan="5">Other Options</th>
  </tr>
  <tr>
    <td><code>-logfile</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Generates a logfile <code>d3.log</code> if using the Debug build. All <code>mprintf</code> statements output to the logfile.</td>
  </tr>
  <tr>
    <td><code>-loglevel &lt;LEVEL&gt;</code></td>
    <td>string</td>
    <td>INFO (on Release), DEBUG (on Debug)</td>
    <td>all</td>
    <td>Set log level (NONE, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)</td>
  </tr>
  <tr>
    <td><code>-makemovie</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Causes the demo system to save a screenshot of every frame during playback.</td>
  </tr>
  <tr>
    <td><code>-mission &lt;name&gt;</code></td>
    <td>string</td>
    <td>None</td>
    <td>all</td>
    <td>Loads the specified mission file at startup.</td>
  </tr>
  <tr>
    <td><code>-nocrashbox</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Disables the error dialog displayed if Descent3 crashes.</td>
  </tr>
  <tr>
    <td><code>-pilot &lt;name&gt;</code></td>
    <td>string</td>
    <td>None</td>
    <td>all</td>
    <td>Specifies the pilot to use, skipping the pilot selection dialog when the game starts.</td>
  </tr>
  <tr>
    <td><code>-service</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Run game in service mode.</td>
  </tr>
  <tr>
    <td><code>-setdir &lt;path&gt;</code></td>
    <td>path</td>
    <td>.</td>
    <td>all</td>
    <td>Specifies the working directory for Descent 3.</td>
  </tr>
  <tr>
    <td><code>-useexedir</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>all</td>
    <td>Tells Descent 3 to use the directory in which the executable is located as the working directory.</td>
  </tr>
  <tr>
    <td><code>-winconsole</code></td>
    <td>boolean</td>
    <td>Off</td>
    <td>WIN</td>
    <td>Enable windows console (off by default).</td>
  </tr>
</table>
