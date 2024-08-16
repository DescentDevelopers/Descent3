# Descent 3 Open source usage instructions

**Important note**: This open source distribution of Descent 3 DOES NOT CONTAIN GAME ASSETS. Assets must be acquired separately from an official copy of the game, and copied as describe in the next section.

This is the first release of the Descent 3 open source engine, that should be considered a beta version. If you find a bug that has not been reported before, please open a new ticket it on our [online issue tracker](https://github.com/DescentDevelopers/Descent3/issues).

1. Make sure that you have a copy of Descent 3. You can purchase a copy of Descent 3 from [GOG](https://www.gog.com/game/descent_3_expansion) or [Steam](https://store.steampowered.com/app/273590/Descent_3/).
2. Install Descent 3.
 **Note for Steam users:** If you own Descent 3 on Steam, then it’s recommended that you install the Windows version of the game even if you’re running macOS or Linux, otherwise movies will not work due to [current lack of Ogv support](https://github.com/DescentDevelopers/Descent3/issues/240). You can use either [Steam Play](https://help.steampowered.com/en/faqs/view/08F7-5D56-9654-39AF) or [SteamCMD](https://developer.valvesoftware.com/wiki/SteamCMD#Cross-Platform_Installation) to install the Windows version of the game on macOS or Linux.
3. If your version of Descent 3 is older than v1.4, then [update it to v1.4](http://descent3.com/downloads.php).
4. Find the installation location of Descent 3. Using the Steam client, you can find it from the library page using the `Manage > Browse local files` context menu.
5. Create a new folder named `D3-open-source`.
6. Copy the following files from your installation of Descent 3 to `D3-open-source`:
    - All `.hog` files
    - The `missions` folder
    - _(Optional)_ All `.pld` files
    - _(Optional)_ The `movies` folder
7. Create the `custom/` folder in `D3-open-source`
8. Obtain new Descent 3 engine files:
    - If you want to use pre-built binaries, then download the latest [release](https://github.com/DescentDevelopers/Descent3/releases). For a more cutting-edge experience with the latest features, use the artifacts from the latest automated build. You can find the list of automated builds [here](https://github.com/DescentDevelopers/Descent3/actions/workflows/build.yml?query=branch%3Amain+event%3Apush).
    - If you want to build the engine files yourself, the follow the instructions in [BUILD.md](BUILD.md). Once you build the engine files, they’ll be put in `builds/<platform>/Descent3/<build-type>/`. For example, if you’re using Linux and you create a “Release” build, then the files will be located at `builds/linux/Descent3/Release`.
9. Copy all of the new engine files into `D3-open-source` and overwrite any conflicts.
10. Special notes:
    - D3 Open Source compiles level scripts in their own hogfiles. Make sure you copy and overwrite `d3-{platform}.hog`.
11. Run the game:
    - On Windows, run `D3-open-source\Descent3.exe` from a command-line or double-click on the `Descent3` executable.
    - On Linux, `cd` to `D3-open-source` and run `./Descent3`. Wayland users may need to set environement variable `SDL_VIDEODRIVER=wayland` before launching the game.
    - On MacOS, the `.app` bundle is currently not signed, so your operating system will not let you run it by double-clicking it. To remediate that, open your terminal and `cd` to `D3-open-source`. Run `chmod +x ./Descent3.app/Content/MacOS/Descent3`, then run the game using `./Descent3.app/Content/MacOS/Descent3`

## Troubleshooting

```
Descent 3 Message(Error: Couldn't find the string table.)
```

This error means that game data could not be found. Make sure you copied all game files to the `D3-open-source` folder, and that you're running the game from this same folder.

