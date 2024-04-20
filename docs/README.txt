

                          RELEASE NOTES FOR    
                            DESCENT (TM) 3 
                             Version 1.5.0

                  ___________________________________

                     ! DO NOT DISTRIBUTE THIS GAME !
                    DESCENT 3 IS A COMMERCIAL PRODUCT

                       See Section 13 for details
                  ___________________________________


                         ______________________

                           TABLE of CONTENTS
                         ______________________


                         1.  Introduction
                         2.  Version Changes
                         3.  System Requirements
                         4.  Manual Addenda
                         5.  Keyboard Controls
                         6.  Multiplayer Game Types
                         7.  Command-line Options
                         8.  Compatibility
                         9.  Maximizing Performance
                         10. Troubleshooting
                         11. Customer Service
                         12. Reporting Bugs
                         13. Legal Information


                          ______________________

                             1. INTRODUCTION
                          ______________________


This file contains information about Descent 3 that is supplemental
to what's in the manual.  It includes information on version changes,
troubleshooting, and reporting bugs.  It also contains information
about changes to the game made after the manual went to press.
 

                          ______________________

                            2. VERSION CHANGES
                          ______________________


1.0.0  - Initial release

1.1.0  - Win32 update; Linux dedicated server update

1.2.0  - Win32 update; Linux dedicated server update

1.2.5  - Initial Mac release

1.3.0  - Descent 3: Mercenary; Win32 update; Linux dedicated server update

1.3.1  - Mac Update

1.3.2  - Mac Update

1.3.3  - Mac Update

1.4.0  - Win32 Update; Linux dedicated server update; Mac Update; Initial
         Linux version

1.4.0a - Linux Update

1.4.1  - Mac Update

1.5.0  - Win32, Mac, Linux, & Linux dedicated server update


Special thanks to Jason Leighton, Duane Johnson, and Kevin Bentley for
their work on the 1.4 patch and to Kevin Bentley for his work on the
1.5 patch.

Please check out the Descent 3 FAQ which is included in the 1.4 and
later releases.


Fixes for Version 1.5.0:
    - Fixed a checksum problem introduced in 1.4 that prevented some
      levels from being played across platforms (for example, Windows
      and Linux).
    - Force feedback now works under Windows 2000.
    - Fixed crash starting a multiplayer game when the Black Pyro is
      the only allowed ship and the player is currently set to use
      another ship.
    - Can no longer start a multiplayer game with no allowed ships.
    - It's no longer possible to crash the dedicated server by typing
      a very long command line in telnet mode.
    - Disabled cheats that are based on changing the ship weapon values
      in the data files.
    - Mod files are now stored on www.descent3.com (instead of 
      www.pxo.net).
    - Removed support for HEAT.NET, as they're no longer in business.
    - Pilot command-line arg (-pilot or +name) is now only checked once,
      not every time the main menu comes up.  This fixes the problem of
      the game switching back to the pilot specified on the command 
      line after the user has selected a different one.
    - When the player's HUD name level is lowered by a game server,
      the client remembers the higher level and will revert to that
      level when a server allows.
    - Fixed a problem with the GuideBot menu not responding to key
      presses after firing with the mouse.
    - Saved games now properly load custom textures.
    - Demos now properly load custom textures.
    - Primary weapons no longer behave oddly when attempting to fire
      secondaries when out of secondaries.
    - Fixed a rendering crash that could be caused (among other ways,
      probably) by a lot of chaff being detonated at once.
    - Fixed some (maybe all) multiplayer ghost ship problem.
    - Omega cannon energy usage is now not framerate dependent.
    - Fixed crashes/slowdowns when using the omega cannon.
    - Fixed a problem with the rearview camera when viewing demos
      of multiplayer games.
    - Fixed a ghost ship problem with demos after a player respawned
      in a multiplayer game.
    - Certain objects, such as the surface cannon in level 10, no
      longer interact with observers in multiplayer games.
    - The stats file now contains a listing of team members in
      Team Anarchy, Mosterball, and Entropy.
    - The results screen for team anarchy now shows the team scores.
    - Don't show extra teams on team anarchy & monsterball results
      screens.
    - Removed the Mission Status item from the HUD config menu, since
      there is no Mission Status HUD indicator.
    - Removed the missile camera center window option, since it didn't
      really work (& wasn't supposed to be there in the first place).
      If you really want the center view, however, you can set 
      MissileView to 1 in the registry.  Note that going into the
      config menu will reset it, though.
    - Fixed problems when video bit depth set to 32.
    - Clients now can hear the server player taking omega (& other)
      damage.
    - Fixed problems with omega cannon causing wrong amounts of damage
      and damage to the person firing the weapon.
    - Allowed room for longer team names on results screens.
    - Ping & packet loss data no longer displayed on HUD in
      peer-to-peer games.
    - It's no longer possible to pick up a powerup in multiplayer while
      leaving observer mode while moving over the powerup.
    - Added confirmation HUD messages when sending private messages.
    - Fixed several problems unloading overlay data.
    - Made -mission work as described; that is, it now causes the 
      specified mission to start automatically when the game is started.
    - Colored stipes now display on player ships in demo playback of
      multiplayer games.
    - HUD gauges now work in demo playback.
    - Clarified the description in readme.txt of the -mission command-line
      option. Refer to Section 7, Command-line Options, for the new
      description.
    - Fixed a couple problems with the level auto-download system.
    - In demos of multiplayer games, player ships no longer switch 
      between modesl for different ships.


New features for Version 1.5.0:
    - Increased by 20 the maximum number of player ships.
    - Increased by 300 the maximum number of textures.
    - Increased by 100 the maximum number of objects.
    - Increased by 200 the maximum number of polygon models.
    - Added "-nosatomega" command-line option to disable alpha
      effects on omega cannon.  Try this if you have speed or crashing
      problems with the omega under 1.5.
    - Added -nocrashbox command-line option to disable the error dialog
      displayed if the game crashes.
    - Improvements for some players using NAT routers.  If you're having
      trouble playing D3 using a NAT, enable port/host forwarding on
      the NAT server, then specify your port with the the -useport and              -pxoport command-line options.
    - Added "-datacheck" command-line option.  If you get a message
      on the loading screen that your mission has data errors, use
      this option and see the file "datacheck.out" for a description
      of the data errors in your mission.
    - In multiplayer, a HUD message is shown when a player's ship
      is changed because the selected ship is not allowed.

Known issues in Version 1.5.0:
    - Because of limitations in the precision of Descent 3's timer,
      high framerates are not displayed accurately on the HUD.  


Known issues in Version 1.4.0:
    - Under Windows 2000, mouse buttons may not work properly.  If
      you experience this problem, try selecting the checkbox for
      "Enable Logitech Mouseman Mode" on the Controls tab of the Setup
      dialog.
    - The scores list in the dedicated server will show only one
      digit for the number of kills, deaths, and suicides.  This will
      not be fixed.
    - The network loss display shown to clients is not always 
      strictly accurate.  See "Network loss statistics" in Section 4, 
      Manual Addenda.


New features for Version 1.4.0:
    - Added IntelliVIBE support for VRF's Tactile Feedback System. See
      http://www.vrf.com for more info.
    - Added a limited auto-download option feature for game mods.  See
      "Auto-downloaded game mods" in Section 4, Manual Addenda.
    - Increased MAX_SHIPS from 5 to 10 for allow more add-on ships.
    - Made several small speed improvements.


Fixes for Version 1.4.0:
    - Fixed a parsing bug in the dedicated server configuration file
      that would crash on the MultiSettingsFile keyword.
    - Fixed a problem that caused old demos to sometimes crash or
      display incorrect objects. 
    - Fixed a bug that could cause crashes when playing from a saved 
      game from an earlier version.  Note however that if a saved game 
      from verion 1.2 or before was loaded in 1.3 and then re-saved, 
      that saved game is probably corrupt and will not be playable.
    - Fixed a UI bug that caused the Terrain Detail slider to sometimes
      show incorrect values. 
    - Fixed an effects problem that ocurred in low-memory conditions.
    - Fixed memory overwrite with long ship logo file names.
    - Fixed problem that caused unneeded sounds to not be deleted.
    - Typing ":" or "=" tokens in the dedicated server console no
      longer crashes the server.
    - The game info dialog on the game list screens now shows the
      correct difficultly level for the game.
    - Fixed memory overwrite bug in DMFC.
    - The SHIPBAN command in multiplayer settings files now works with
      the Black Pyro.  
    - In the PXO game list, fixed a crash when sorting by ping.
    - In the PXO client, the game list is now cleared when joining a 
      private lobby.
    - Improved multiplayer performance by increasing the size of the
      network receive buffer.
    - Fixed a sound system crash when playing audio taunts in the 
      pilot menu before playing the game.
    - Fixed a bug that allowed the GuideBot to be killed in Mercenary
      level 5.
    - Added error checking in several places.
    - Fixed several memory leaks, including a big one in the dedicated
      server that would cause it to run out of memory after many level 
      resets.
    - Fixed a bug in the lightmap system.
    - Miscellaneous Mac fixes.
    - Fixed a bug with out-of-bounds weapon indices.
    - Check for too-long player callsigns.
    - Fixed a string table problem in non-North American versions
      that caused netgame crashes.
    - Fixed a bug that caused multiple versions of audio taunts and
      ship logos to be sent from a client to a server.  
    - To fix performance problems, secondary weapon firing 
      no longer uses reliable packets.
    - Fixed phantom Napalm, Omega, and Vauss bugs.
    - Fixed a bug in the deadzone that caused the value to be decreased 
      each time you the config dialog was used.
    - Added the -usesmoothing command-line option to address skipping
      problems with fast ships, such as the Phoenix.  This option
      affects clients only; using it on the server has no effect.
    - Fixed a crash in Level 10.
    - Fixed keyboard ramping problems introduced in version 1.3.
    - Fixed the homing lock warning so it no longer happens when
      a weapon does not have a line of sight to the player.
    - Fixed a problem that caused a server to crash when a player
      with a corrupted logo file joined the game.
    - Added the -mousesens command-line option to adjust the mouse
      sensitivity when not using mouselook mode.  See the description 
      in Section 7, Command-line Options. 
    - Fixed a crash caused by using a long team name.
    - Fixed a sporadic sound crash bug.
    - Fixed a Direct3D bug that may have (but probably didn't) cause 
      a crash.
    - Fixed a screen clear problem on the ATI Rage Fury Maxx using
      dual-chip mode under OpenGL.
    - Added code to thwart a multiplayer cheat in which players could
      leave the mine and fire at other players with impunity.
    - Added code to prevent the player ship from going above the
      ceiling height.
    - Messages sent from the dedicated server console can now be 
      be sent to individuals and team members in the same way as
      F8 messages sent by a player.
    - Fixed a problem that caused the Linux dedicated server to send
      corrupted stats to Parallax Online.
    - Fixed a Linux dedicated server crash for add-on table files that
      incorrectly specified full path names for data files.


New features for Version 1.3.0:

    - Miscellaneous additions for Descent 3: Mercenary.
    - Significant memory savings, which should help performance.
    - If text for a particular langaguge is not found, the English
      text is used instead.
    - In the file dialog, double-clicking on a directory will open
      that folder.
    - Added the -mlooksens command-line option to change the mouse
      sensitivity in mouselook mode.  Usage: -mlooksens <scale>.
      Scale determines how much the player moves when the mouse is
      moved.  The default value is 9.1; smaller values give more
      precision.
    - Added the -mission command-line option.  This causes the 
      specified mission file to be loaded at program start, and makes 
      that mission the default when starting a new game.  
      Usage: -mission <name>.
    - Added the ability for objects to cast "negative" light. See
      the DMFC documentation for more info.
    - Added new mission file keyword "SHIP" to override default ship.
    - Added support for add-on data that's not mission-specific.


Fixes for Version 1.3.0:

    - Fixed randomness in ambient sound patterns.
    - Miscellaneous fixes for DMFC. See the DMFC readme.
    - Fixed a couple small bugs for add-on table files.
    - Fixed a byte-ordering problem with PXO, introduced in 
      version 1.2, that caused stats to be lost.  


New features for Version 1.2.0:

    - Reduced network traffic in multiplayer games by not sending 
      position packets for dedicated servers, dead players, and
      observers.
    - Additional Pentium III enhancements: motion blur on robots and 
      powerup sparkles.
    - Under Linux, service mode (invoked with -service) now works like 
      a true service.
    - Secondary weapon fire packets are now sent relialbly.
    - Added support for EAX 2.0.
    - Changed the controller (joystick) code to use by default the 
      Windows API.  Version 1.0.0 used the Windows API, but 1.1.0 
      used DirectInput.  To use DirectInput under 1.2.0, use the 
      "-directinput" command-line option (which does the same thing
      as the "-alternatejoy" option.)  DirectInput should be used
      with joysticks that don't work with the Windows API, such as
      the Wingman Interceptor and other multiple-hat sticks.


Fixes for Version 1.2.0:

    - Fixed a minor AI path-following bug.
    - Add-on table files now work correctly when pages in the add-on
      are dependent on other add-on pages.
    - Fixed an energy center problem in Entropy games.
    - Fixed a problem in CTF games picking up the flag in small rooms.
    - F7 no longer crashes when pressed immediately upon joining a 
      netgame.
    - Fixed z-buffer rendering problems under OpenGL.
    - Fixed an OpenGL problem caused by setting the magnification 
      filter to an invalid setting.
    - DMFC is now notified of player damage before the player is 
      killed.
    - Fixed a menu problem that showed no ship logo selected when an 
      OAF-format logo was selected.
    - Stats are now written to PXO when a server quits.
    - Fixed syncing problems in multiplayer when a guided missile
      is released. 
    - Changed the Winsock packet buffer to be 16KB.
    - Removed the joystick sample-rate cap mistakenly added in 
      version 1.1.0.


New features for Version 1.1.0:

    - Added the -bumped command-line option to enable hardware
      bump-mapping on cards that support it, such as the Matrox G400.
    - Added the -NoRenderWindows command-line option.  Some cards
      do not correctly render partially-transparent windows.  If 
      windows do not work correctly on your card, use this switch
      to cause all windows to be fully transparent.
    - The Pilot Info function on the PXO chat screen now shows the
      numerical ranking required to achieve the next named rank.
    - Increased the maximum joystick sensitivity.
    - In co-op, spewed powerups no longer time out.
    - Added the -deadzone0 & -deadzone1 command-line options to
      specify the size of the deadzone for a joystick.  See the 
      description in Section 7, Command-line Options.
    - Overhauled the PXO ranking system.  One major effect is that
      players should no longer lose rank even when beating someone
      of lower or equal rank.
    - Reduced the delay between shots for the Fusion Cannon.
    - Added the -z32bit command-line option for cards that support
      a 32-bit z-buffer, such as the Matrox G400.
    - Added enhancements for Pentium III systems. 
    - Added the "outlinem" cheat code to turn on outline mode (for 
      level designers).
    - Added "renderstat" cheat code to see rendering statistics (for 
      level designers).
    - A sound is now played when a player disconnects or leaves the 
      game.
    - Added delete buttons to the ship configuration dialog to delete 
      logos and audio taunts from within the game.
    - The user's pilot is now highlighted on the F7 stats screen.
    - Kills, deaths, and suicides are no longer reported to PXO for 
      Co-op games.
    - Added several new features to the DMFC SDK; see the SDK Readme
      file for more information.
    - Added the "$scores" server command for Entropy and Team Anarchy
      games.
    - Added a token system for taunt macros.  See the description in 
      Section 4, Manual Addenda.
    - The typing icon is now not displayed to other players when 
      entering a team message using Ctrl-F8.
    - Added the -autoexec command-line parameter.  See the description 
      in Section 7, Command-line Options.
    - Added Remote administration of a server via a client.  See the 
      description in Section 4, Manual Addenda.
    - Added the "$wait" server command.  See the description in 
      Section 4, Manual Addenda.
    - Added the "logout" server command to terminate a telnet 
      connection.
    - In multiplayer games with a level time limit, a countdown timer is 
      displayed starting 10 seconds before the level ends.
    - The status of the small rear view is now saved in the pilot file.
    - Added better support for the Madcatz Panther XL.  Configuration is
      specified in the file PANTHER.CTL.
    - Added better support for the SpaceOrb 360.  Configuration 
      is specified in the file SPACEORB.CTL.
    - This version will work on systems running non-English versions of 
      Windows.
    - Added a dedicated server command for Hoard to set the minimum
      number of hoard orbs needed to score. Usage: "$mincount <count>", 
      where 'count' is between 1 and 12.
    - Room wind is now loaded from the level file (for compatibility with
      D3Edit).
    - Improved Aureal 2.0 support.
    - Added code to unzip auto-downloaded mission files.  For details,
      see Section 4, Manual Addenda.
    - Added the -nooutragelogo command-line parameter.  See the 
      description in Section 7, Command-line Options.
    - The framerate is now capped at 50 by default.  To remove this cap
      use the command-line option "-framecap 999".
    - Added the -httpproxy command-line parameter.  See the description
      in Section 7, Command-line Options.


Fixes for Version 1.1.0:

    - Corrected the legal text in Section 13 of the Readme file.
    - Fixed the section numbering in the Readme file.
    - Fixed z-buffer rendering problems under Direct3D on cards 
      that don't support w-buffering.
    - When running a small install, restoring a saved game no longer 
      requires inserting CD 1 if the level is stored on CD 2.
    - Music now works properly in the automap.
    - In multiplayer games with a time limit, the timer now does not
      start until after the "Waiting for Players" dialog has been 
      closed.
    - When typing a private or team-only message that's longer than 
      one line, the continuation line is now only sent to the 
      recipient(s) of the first line.
    - Fixed glitches when the multiplayer game list screens updated.
    - The auto-download system for missions now works.
    - When going to a new level in a multiplayer game, the reticle is
      now reset correctly.
    - Removed the Matrox M3D from the list of supported video cards in
      the Readme file.
    - Fixed crashes on servers left running for a long time.     
    - In multiplayer games, time spent in observer mode now doesn't 
      count as time alive.  
    - Fixed several crashes and other bugs in the demo system. 
    - Fixed an occasional crash when clicking on the "Return to Chat"
      button on the PXO game list screen.
    - The program NETTEST.EXE, mentioned in the manual, is now included.
    - The GuideBot now does a better job of staying out of the way of
      player shots.
    - The GuideBot should not get stuck as often.
    - Improved anti-cheating code in the server.
    - Fixed parsing of the command line on dedicated servers to allow 
      certain punctuation marks.
    - Fixed "$scores" command for CTF and Monsterball to list the team 
      scores also.
    - Fixed the post-level results screens for CTF, Monsterball, Team 
      Anarchy, and Entropy to display connected players before 
      disconnected players.
    - Fixed F7 stats screen for CTF, Monsterball, Team Anarchy, and 
      Entropy to display connected players before disconnected players.
    - Fixed the multiplayer post-level results screen to prevent text 
      from going off the bottom of the screen.
    - Fixed a memory bug in the connection DLLs that could cause a 
      crash.
    - Fixed a memory bug in the UI system that could cause a crash.
    - The D3 temporary directory (custom/cache) can now only be used by 
      one process at a time.  When running multiple copies of Descent 3 
      on the same machine, use the -tempdir command-line option to 
      specify an alternative directory.
    - Fixed a Hoard bug that sometimes gave the player points when 
      killed in the goal.
    - The key used to invoke the automap now will also exit the automap.
    - Mouse control no longer varies with framerate.  
    - The game now works correctly on systems with multiple CD-ROM 
      drives.
    - Fixed some odd behavior when the afterburner was mapped to more 
      than one button or key on the same device.
    - Changed a multiplayer kill messages that wasn't gender neutral.
    - Changed the GuideBot's "Exit Ship" menu item to "Release 
      GuideBot".
    - Keyboard sensitivity is now set correctly from the selected
      pilot file when starting the game.
    - In multiplayer, the typing indicator icon now draws correctly 
      under Direct3D and OpenGL.
    - Fixed a bunch of memory leaks.
    - Fixed the manage system to support multiple add-on table files. 
    - In DMFC, the timer interval function was called twice per frame.
    - Reduced the Winsock packet buffer to the larger of 2KB or the 
      minumum for the installed network card.

Fixes for Version 1.1.0, Linux dedicated server:

    - Fixed server shutdown when a client quits out of a telnet 
      administration session.
    - Fixed crash due to typing certain keys, such as a colon.
    - Improved keyboard handling for non-SVGALIB keyboard processing.
    - Removed nearly all library dependencies.  Libraries are now 
      dynamically loaded when needed.
    - Fixed excessive redraw with 'raw' console support by not using
      the ncurses library.
    - Fixed backspace problems with 'raw' keyboard support.


                        __________________________

                          3. SYSTEM REQUIREMENTS
                        __________________________


The following minimum system configuration is required to run 
Descent 3:

    A 200MHz Pentium processor
    Windows 95, 98, or NT
    32MB RAM
    3D accelerator video card with 4 MB of texture RAM
    4x CD-ROM drive
    210 MB hard disk space
    DirectX 6 certified sound card
    If you have Windows 95 or Windows 98, DirectX 6.1 software
    If you have Windows NT 4.0, Service Pack 3 or greater

The following hardware is recommend for a good Descent 3 experience:  

    A Pentium II 300MHz processor
    64MB RAM
    500 MB hard disk space

Of course, the faster the CPU and the more RAM your computer has, 
the better performance you'll get in Descent 3.  We've tried to make
Descent 3 work on a wide range of machines, so don't be suprised if
you can't use the highest detail settings if you don't have the very
latest hardware.


                        _______________________

                           4. MANUAL ADDENDA
                        _______________________


The following are supplemental items and changes to the Descent 3 
manual.

CHANGES & CORRECTIONS

Weapon Precedence:

  We've changed the way weapon precedence selection works.  With the
  new system, you first click on the item you want to move, and then
  click on the spot where you want the item.  The item will be moved
  to that spot, and the other items will slide down to make room for
  it.

Mercenary mission:

  The single-player Mercenary mission does not support cooperative
  play.


SUPPLEMENTAL ITEMS

Note about multiple missions:

  When starting a new game after a clean install or after creating
  a new pilot, Descent 3 will automatically start the training
  mission, and then proceed into the first level of the Retribution
  mission.  To select another mission, you need to use Escape to abort
  the training mission, and then select New Game again.

  When launching Descent 3 via the Mercenary shortcut, the training
  mission will not be automatically selected, and you'll be able to
  start any of the available missions. 
   

PDF Manual:

  A PDF version of the Descent 3 manual is provided on CD 1. The
  file is D3Manual.pdf and is located in the root directory of CD 1.
  You can view this file with Adobe Acrobat Reader which can be 
  downloaded from Adobe's web site at:
     http://www.adobe.com/prodindex/acrobat/readstep.html

Brightness Control:

  Some video cards do not allow applications such as Descent 3 to
  adjust the card's brightness (gamma) setting.  If you try to adjust
  the brightness in D3 and get no response, try adjusting the setting
  in the Windows Control Panel (under Display).

Guide-Bot:

  Shift + the number keys (1 - 9, 0) will command the Guide-Bot to
  perform specific actions.  See a the list in Section 5, Keyboard 
  Control.

FindFast:

  Microsoft Office installs a program called FindFast that makes
  it possible for Office applications to open files more quickly.
  This application, FindFast, runs in the background on your
  machine indexing the contents of your hard drive.   

  If FindFast is running while Descent 3 being played, the game's
  performance can be significantly reduced.  For this reason, the
  Descent 3 launcher will detect the presence of FindFast when
  the game is started and give you the option of disabling it. 
  We STRONGLY recommend that you disable FindFast when starting 
  Descent 3.

  If you terminate FindFast it may take your computer slightly 
  longer to open files, but everything else will work normally.  
  FindFast will be re-enabled the next time you start your computer.

Shift Keys:

  Windows 95 & NT do not differentiate between the left and right
  shift keys when informing Descent 3 that one of these keys
  has be released.  If you press both shift keys at the same
  time, Descent 3 will think that one key is still held down
  even after both have been released.  Because if this, you may 
  want to be careful when assigning the shift keys to functions  
  in Descent 3.

HEAT.NET:

  Descent 3 features direct support for HEAT.NET.  To play a game 
  of Descent 3 on HEAT.NET, select HEAT.NET from the Multiplayer
  menu in Descent 3, or use the Start Menu shortcut "Play Descent 3 
  Online at HEAT.NET".

Multiplayer:

  CTRL-F8 can be used to send messages to your teammates in a team
  multiplayer game.  This works just like the F8, but only your
  teammates will see the message.  In a non-team game, everyone will
  see the message.  When you receive a team message, the sender's
  name will be in brackets (e.g., "[Crucible]: look out!").

  When using F8 to send a message, you can send a private message to a
  specific player by starting the message with the player's pilot
  name, followed by a colon.  For example, typing "spike: look out!"
  would send the message "look out!" to the player Spike.  You only
  need to type as many characters of the player's name as are
  necessary to differentiate it from other player names.  When you
  receive a private message, the sender's name will be in angle
  brackets (e.g., "<Crucible>: look out!").

Level 13 ("CED Expediator Dreadnaught"):

  Because the way this level is scripted, it cannot be played in
  anarchy or other non-cooperative modes.  If you attempt to play
  this level in an anarchy mode, the level will immediately 
  terminate and the game will advance to the next level. 

DirectPlay Update:

  If you want to play Descent 3 over a modem or serial connection,
  you should download the DirectPlay 6.1a update from Microsoft. 
  This is available at:

    http://www.microsoft.com/directx/developer/downloads/dplay.asp

  This update drastically improves the performance and reliability of
  DirectPlay and is highly recommended.

PXO Stats:

  For games played on Parallax Online (PXO), the service keeps track 
  of the kills, deaths, and suicides for each pilot.  Since some types
  of multiplayer games are scored primarily on points or goals rather
  than kills and deaths (Entropy, Capture-the-flag), you may not want
  kills and deaths to be recorded when playing one of these.  In this
  case, we suggest using an alternate pilot for those games.

PXO Rankings:

  For games played on Parallax Online (PXO), Descent 3 supports a
  ranking system that computes a lifetime rank for each player.  This
  system is similar to the rankings used in chess; here are some of
  the features of this system:

  * New players start at 400.

  * Your ranking will increase with each win and decrease with each 
    loss, but will increase more if you beat a player with a higher
    ranking than you, and will decrease more if you lose to a player
    with a lower ranking.

  * The numerical rankings correspond to named ranks.  Note that it 
    may take a very long time to reach the higher ranks.  The 
    numerical rank needed for each named rank is shown below.

            0: Cadet
          600: Ensign
          900: Lieutenant
         1200: Lt. Commander
         1500: Commander
         1800: Captain
         2100: Commodore
         2400: Rear Admiral
         2600: Admiral
         3000: Demigod

  * When you achieve a new rank, you'll be notified via a HUD
    message.

  * You can see the rank of each player in a multiplayer game
    on the player list, viewable by pressing the F7 key.

  * You can view a list on PXO of all Descent 3 players sorted by 
    numerical at http://www.pxo.net/.

Dedicated Server:

  There is a new dedicated server command, "AudioTauntDelay <time in
  seconds>".  Use this command to set the time in seconds the a user
  must wait after sending an audio taunt before he or she is able to
  send another.

  You can set the difficulty level of a game in the dedicated
  server config file.  Use the command "SetDifficulty = <level>",
  where <level> is the difficulty, ranging from 0 (trainee) 
  to 4 (insane).

  If you're running multiple dedicated servers on the same computer,
  you need specify the an alternate GameSpy port number for the 
  additional servers.  Use the "-gamespyport" command-line option.

FreeSpace 2 and Klingon Academy Preview Movies:

  The \Stuff directory on CD 2 contains preview movies of the upcoming
  Interplay titles FreeSpace 2 and Klingon Academy.

Taunt Macro Tokens:

  Certain game-specific values can be automatically placed in taunt 
  macros by using the tokens listed below.  When a message is sent, 
  the game will replace the token with the specified value.  

     Token      Replaced With

     $$e        your current energy
     $$s        your current shields
     $$virus    your current virus count (Entropy only)
     $$flag     names of flags you are currently carrying (CTF only)

  Note that the author of a new multiplayer games can add new tokens 
  specific to that game.

Remote Server Administration From a Client:

  A game server can be administered from a client by using the following
  commands with the F8 message system.

    $remoteadminpass <pswd>   Sets a password for remote administration.

    $remoteadmin <on/off>     Turns on or off remote administration.

    $remote <cmd> <args>      Sends a remote command to the server.

    $remoteadminlogout <id>   Logs out the specified player.  If no 
                              id is specified, lists the players 
                              currently administrating remotely.
  Examples:

    $remote login buffy       Logs in with the password "buffy".
    $remote logout            Logs out.
    $remote $endlevel         Tells the server to end the level.

$wait Server Command:

  The $wait server command will suspend a multiplayer game.  This can
  be used at the start of a game to allow all players to join before
  starting play.

  Usage:

    $wait <on/off>     Forces clients to stop/start moving immediately

    $wait <seconds>    Forces clients to wait for the specified number
                       of seconds at the beginning of each level.


Auto-downloaded missions:

  The system to auto-download missions now supports zip files.  If
  a mission file specifies a zip file (one with the extension ".zip")
  in the mission URL, the mission file will be extracted from the zip
  file after downloading.

  Note:  This system works only with the default level of Zip 
  compression.

  Thanks to Jean-loup Gailly and Mark Adler for the zlib unzip code. 


Auto-downloaded game mods:

  A system to download some game mods was added in version 1.4  If you
  try to join a game that uses a modification that you don't already
  have, D3 will check the PXO server to see if it's available for
  download. If it is, the download will begin automatically. Only mods
  that have been verified as safe will be downloaded. If you have a
  modification you would like made available for auto-download, send
  email to d3mod@outrage.com.
  
Network loss statistics:

  The network loss value shown to clients in a network game is not 
  strictly accurate, but is a close approximation. Because of timing
  issues, this display can show loss when there isn't any.  Because 
  network packets may arrive out of order, a client may assume that
  packets are lost when they are merely late.

  Also, packets can be discarded by the TCP/IP stack as well as Descent
  3's internal networking layer.  This can happen for a variety of
  reasons, such as out of order packets.  These discarded packets will
  be counted as lost by Descent 3.


                     ____________________________

                        5. KEYBOARD CONTROLS
                     ____________________________



Keyboard Controls

While playing the game, press F2 then click on Customize keyboard
to customize your keyboard and joystick controls. To change a
particular key, click the box next to a command, then type
the key you want to use for that command. You can assign up to
two keys to each function.  Below is a list of the defaults,
and a few other controls that you will find helpful.

A              Accelerate
Z              Reverse                                                
S              Afterburner                                
Left           Turn left                                        
Right          Turn right                                                                                    
Up             Pitch down                                
Down           Pitch up                                        
ALT            Slide sideways while using arrow keys                        
Ctrl           Fire primary weapon
Spacebar       Fire secondary
F              Flare
H              Headlight
R              Rear view
Tab            Automap
Q              Bank left
E              Bank right
Pad7           Slide left
Pad9           Slide right
Pad-           Slide up
Pad+           Slide down
-              Shrink screen (speeds up game)
+              Enlarge screen
Print screen   Take a screenshot(TGA format)
                                                        
1              Laser & Super Laser                
2              Vauss & Mass driver                
3              Napalm Cannon & Microwave        
4              Plasma Cannon & EMD                
5              Fusion Cannon & Omega                
6              Concussion Missile & Frag Missile
7              Homing Missile & Guided
8              Impact Mortar & Napalm Rocket
9              Smart missile & Cyclone
0              Mega Missile & Black Shark

F1             Help screen
F2             Game options
F3             Show/hide cockpit
F4             Guide-Bot menu
F5             Record Demo
F6             Multiplayer: Show/hide menu
F7             Multiplayer: Show/hide stats
F8             Multiplayer: Send message to all
F9             Show/hide message log
F12            Drop Marker


Shift F1       Toggles Rear view camera/Marker camera/Guide-Bot
               camera on the left.
Shift F2       Toggles Rear view camera/Marker camera/Guide-Bot
               camera on the right
Shift Tab      Access the level briefing/objectives/automap

Guide-Bot commands:

  Shift 1       Find Next Active Goal
  Shift 2       Find Spew
  Shift 3       Find Powerup
  Shift 4       Find Energy 
  Shift 5       Escort Ship
  Shift 6       Use Extinguisher
  Shift 7       Use Wingnut Powerup
  Shift 8       Use Guardian Powerup
  Shift 9       Use Anti-virus Powerup
  Shift 0       Return to Ship  


                     ____________________________

                      6. MULTIPLAYER GAME TYPES
                     ____________________________



Descent 3 offers a variety of multiplayer games.  Here is a
description for each game type.

ANARCHY       - If you are looking for nothing but nonstop action,
                then this is the game type for you.  ANARCHY pits
                you up against everybody else in the game to see
                who is the best.

CO-OP         - CO-OP stands for COOPERATIVE.  This game type only
                works on Descent 3 Retribution levels.  Join with
                your friends and battle through all of Descent 3's
                levels.

CTF           - CTF stands for CAPTURE THE FLAG.

                In the Fury Collection, only two teams are
                supported (Blue and Red).  We recommend a max of
                8 people for this series.

                In the Bedlam Collection, up to 4 teams are
                supported (Blue, Red, Green and Yellow).  CTF is
                also supported in the Dementia and Frenzy missions;
                these both offer two-team support and up to 16 
                players.

ENTROPY       - Entropy is a teamwork-oriented game that emphasizes
                both action and strategy.  The object is to gain
                control of your opponent team's rooms while defending
                your own rooms.

                In Entropy each contested room will be either a 
                laboratory, an energy center, or a repair zone.  
                The number of these three types of rooms will vary 
                by level. The object is to take over all of your 
                enemy's laboratories, energy centers, and repair 
                zones.

                You take over an opponent's room by infecting it
                with virus samples.  These samples are created in your
                laboratories; to infect a room you must carry five
                or more samples to an opponent's room and remain still
                in that room for three seconds.  

                Your repair rooms allow you to repair damaged ships 
                on your team, and your energy centers allow you to 
                replenish your ships' energy reserves.  Entering any
                of your opponent's rooms will damage your ship.

                You are allowed to carry two virus samples for every 
                opponent you kill without dying yourself.  When you
                die the count is reset and you must kill more
                opponents in order to pick up more samples. You can 
                destroy an opponent's virus sample by colliding with 
                it.

                Virus samples are created every 20 seconds in a 
                laboratory room.  There can be only four samples
                in a lab room at one time, and a maximum of 16
                samples in play at once for each team.

                Score is kept for both individual players and for 
                teams.  When a player takes over an opponent's room,
                he or she gets five points and his or her team
                gets three points.  If all of an opponent's rooms
                are captured, the capturing team gets 10 points.

                If a team has no lab rooms left but does have a
                energy or repair center, one of the latter rooms
                will be converted into a lab.

HOARD         - Hoard was introduced in Descent II: Infinite Abyss.
                In this game, players gather Hoard Orbs by killing
                their opponents, gathering the Orbs and scoring in
                specified scoring zones.  The more Orbs you have
                the higher your score will be.  For example,
                1 Orb = 1 point, 2 Orbs = 3 points etc.

HYPER ANARCHY - Hyper Anarchy is a new game to the Descent series.
                In this mode, all Anarchy rules apply.  The new
                twist is a powerup that adds to your kills.

MONSTERBALL   - Monsterball is our version of Soccer.  This is a
                two team game with all the rules of Anarchy thrown
                in for fun.  Score by hitting the ball into the
                goal with your team's color.

ROBO-ANARCHY  - This mode is for Descent 3 Retribution only.  It
                plays just like ANARCHY, except the levels have
                all their native robots that would be in a single
                player game.

TEAM-ANARCHY  - Team Anarchy is a two team game where the points
                for kills are added to the team totals.


                     ____________________________

                        7. COMMAND-LINE OPTIONS
                     ____________________________


The following command-line options are available in Descent 3.
You can set command-line options on the Misc. tab of the Setup 
section of the Descent 3 launcher.  Case is not significant in
command-line options.


General Options:

  -lowmem            Uses scaled-down textures and lower quality 
                     (8-bit) sounds to conserve memory.

  -superlowmem       Uses the "-lowmem" settings and further scales 
                     down textures to conserve memory.

  -himem             Forces normal operations even when low memory 
                     conditions are detected.

  -vsync             Turns on Vertical Sync.  The flag will be enabled
                     in the registry so it will be on when the game is
                     run again.

  -mouseman          Enables special handling for the Logitech 
                     MouseMan.

  -chpro             Enables a CH Flightstick Pro or compatible 
                     joystick.

  -width <width>     Sets the screen resolution to the specified width,
                     if possible.

  -height <height>   Sets the screen resolution to the specified 
                     height, if possible.

  -nonetwork         Disables all network multiplayer functionality.

  -nosound           Disables all sound, including music.

  -nomusic           Disables music.

  -nolightmaps       Disables lightmaps. This will improve performance
                     on low-end video cards.

  -forcelightmaps    Forces the use of lightmaps, even the Default 
                     Detail Level is set to Low in the launcher setup.

  -nomultitexture    Disables single-pass multitexturing.  This option
                     could fix problems due to buggy drivers on 
                     future video cards.

  -nooutdoorfog      Disables fog on the terrain under Direct3D.  This
                     may improve performance on some cards.

  -setdir <path>     Specifies the working directory for Descent 3.

  -useexedir         Tells Descent 3 to use the directory in which the 
                     executable is located as the working directory.

  -alternatejoy      Causes DirectInput to be used for the joystick
  -directinput       or other game controller, instead of standard
                     Windows API.  Some joysticks need this option
                     to work with Descent 3.

  -pilot <name>      Specifies the pilot to use, skipping the pilot 
                     selection dialog when the game starts.

  -aspect <value>    Specifies the screen aspect ratio for 
                     non-standard displays, such as wide-screen 
                     TVs.  The aspect value is the width of the screen
                     divided by the height, so if you want to play 
                     Descent 3 on a 16:9 wide-screen TV, use 
                     "-aspect 1.78".

  -framecap <fps>    Limits the framerate to the number of frames per 
                     second specified.  Descent 3 defaults to a cap
                     of 50 FPS.  To remove the cap, use the
                     command-line option "-framecap 999".

  -makemovie         Causes the demo system to save a screenshot 
                     of every frame during playback.  These saved
                     frames can be used with video creation software
                     to make a movie.  NOTE:  When recording a demo
                     for use in making a movie, you should use the
                     "-framecap" command-line option to specify the
                     framerate of your movie.

  -fastdemo          Causes a demo to play back at the highest speed 
                     your computer is capable of, and to display some 
                     performance information when it's done.

  -timetest <file>   Causes Descent 3 to play back the specified
                     demo file upon startup.  When the demo is done,
                     the game will exit and create a file called 
                     fps.txt which contains detailed performance 
                     information.

  -highvidmem        Causes 2 MB Voodoo 1 cards to behave as if they
                     had more memory.  This will increase visual 
                     quality but may cause framerate problems.  This 
                     option has no effect if you use OpenGL or 
                     Direct3D.

  -subpixelcorrect   Adjusts for subpixel correction under Direct3D
                     for cards that do not support it.  You can tell 
                     if subpixel correction is not supported if your 
                     main menu initially has lines in it like a
                     tic-tac-toe board.

  -nocompress        Turns off S3TC texture compression for cards that
                     support it (such as the Savage3D and Savage4).  
                     Texture compression is on by default for these
                     cards.

  -bumped            Enables hardware bump-mapping on cards that 
                     support it, such as the Matrox G400.

  -NoRenderWindows   Causes all windows to be fully transparent.  Use
                     this option if your card does not correctly render
                     partially-tranparent windows.

  -z32bit            Enables the 32-bit z-buffer on cards that support
                     it, such as the Matrox G400.

  -autoexec <file>   Specifies the the full path and file name of the 
                     multiplayer config file (eg., autoexec.dmfc) to be
                     loaded and executed when a multiplayer game is 
                     initialized.  This is useful when running more 
                     than one dedicated server on one machine.

  -deadzone0 <k>     Specifies the size of the deadzone for a joystick.  
  -deadzone1 <k>     k is a value from 0.0 to 0.5 sets the deadzone 
                     to the specified fraction of the total joystick range.
                     -deadzone0 sets the first joystick and -deadzone1 sets
                     the second.  For example, "-deadzone0 0.2" sets the 
                     deadzone for joystick 0 to 20% of its total range.    

  -nomotionblur      Disables motion blur on robots (Pentium III only).

  -nosparkles        Disables powerup sparkles (Pentium III only).

  -nopentium3        Disables detection of the Pentium III processor.
                     Some early P3 machines may not work correctly with
                     Descent 3; if you have troubles, try this option.

  -mlooksens <scale> Determines how much the player moves when the mouse 
                     is moved.  The default value is 9.1; smaller values 
                     give more precision.

  -mission <name>    Loads the specified mission file at startup so that
                     mission-specific files (such as the loading screen)
                     can override the regular files.

  -mousesens <scale> Adjusts the sensitivity of the mouse when not
                     using mouselook mode.  A value greater than 1.0 
                     makes the mouse more sensitive; less than 1.0
                     makes it less sensitive.

  -nosatomega        Disables alpha saturation on the omega cannon effect.
                     Use this option if the omega cannon crashes your
                     video card.


  -nocrashbox        Disables the error dialog displayed if Descent3
                     crashes.



Multiplayer Options:

  -gspyfile <config file>   Specifies a GameSpy configuration file to 
                            use (default is gamespy.cfg)

  -gamespyport <port>       Specifies a port for to listen for GameSpy 
                            requests.  The default is 20142.

  -useip <IP>               Binds Descent 3 to this IP address.  Use 
                            this option to tell D3 which IP address 
                            to use if your computer has multiple IP 
                            addresses.

  -dedicated <config file>  Starts a dedicated server.

  -nomultibmp               Disables sending or receiving in-game 
                            custom bitmaps for ships.

  -useport <port>           Specifies the port that TCP/IP and IPX 
                            will use.  The default is 2092.

  -pxoport <port>           Specifies the port that PXO will tell
                            clients to use when contacting a server.
                            The default is 2092.

  -audiotauntdelay <time>   Sets the time in seconds the a user must 
                            wait after sending an audio taunt before 
                            he or she is able to send another.  This 
                            option is only active when starting a 
                            server.

  -playermessages           Disables all weapon-related HUD messages 
                            in multiplayer games.

  -httpproxy <addr>         Specifies an HTTP proxy server.  Descent 3
  -httpproxy <addr:port>    uses HTTP to auto-download a mission; use 
                            this option if your ISP requires the use 
                            of a proxy server for HTTP connections.
                            If the first form is used, the port value
                            defaults to 80.

  -nooutragelogo            Disables the Outrage logo that is normally 
                            displayed for five seconds at the start of a
                            multiplayer game.

  -usesmoothing             Enables code to smooth the interpoloation
                            for fast-moving objects.  This will fix 
                            "skipping" problems with fast weapons, such 
                            as the Phoenix.  This option applies to 
                            clients only; using it on the server has no 
                            effect.


                         _________________________
 
                             8. COMPATIBILITY
                         _________________________


Descent 3 is compatible with a wide range of computer hardware.
However, due to the limitations of some devices and the requirements
of Descent 3, some hardware is fully or partially incompatible
with Descent 3.

In this section, we have listed all the devices that have 
compatibility problems with Descent 3.  Wherever possible, we
have offered suggestions on getting the best results possible
with the given device.  If a device is not listed here, it
either works fine or has not been tested with Descent 3.

This section is divided into three parts:  3D APIs, Video Cards,
and Controllers.


3D APIs
-------

If you're having a problem with your video card, please also
check the "Video Cards" section below..

Descent 3 works on 3D accelerator cards that use the Glide, Direct3D, 
and OpenGL APIs.  By choosing the best API for your card, you will
get better performance and visual quality in Descent 3.

Glide:

  If you a have card with a 3Dfx Voodoo chipset, you should use the 
  Glide API.  You will definitely get the best performance.

  Because Glide disables your 2D video output, certain critical 
  Windows error messages ("blue screens") are not visible when
  playing under Glide.  If you remove your CD from the drive while
  Descent 3 is reading from it, for example, Windows will display
  a full-screen error message that will not be visible.  You can
  continue playing D3 by replacing the CD and pressing Enter.


OpenGL: 

  OpenGL does not support specular reflections, which are used
  in Descent 3 to make some surfaces shiny.


Direct3D:

  If you're using older drivers that are not certified for 
  DirectX 6, you may notice certain graphical errors, such as
  distorted fonts or a grid-like pattern on the main loading
  screen.  You can correct this problem with the "-subpixelcorrect" 
  command-line switch.


VIDEO CARDS
-----------

If you're having a problem with your video card, please also
check the "3D APIs" section above.

We've organized the list below by 3D chipset.  For each chipset
we've listed some common cards that use that chipset; however, we
were not able to list every card.  If your card isn't listed,
find out what 3D chipset it uses and see that section.


ATI Rage PRO:
  Cards: 3d Charger
         All-in-Wonder Pro
         Graphics Pro Turbo PC2TV
         Xpert LCD
         Xpert @play 98
         Xpert @work
         Xpert 98
         Xpert XL


  This card requires that Vertical Sync be turned ON.  If you've
  run the game with another video card and turned Vertical Sync
  off, you can turn it back on again with the "-vsync" command-
  line option.

  This card does not work properly under OpenGL.  We recommend
  using Direct3D.

  Tested with Direct3D driver version w82440en.

ATI Rage 128:
  Cards: All-in-Wonder 128
         Rage Fury
         Rage Magnum
         Xpert 128
         Xpert 99

  The current driver only supports Direct3D. Tested with Direct3D
  driver version w86076en.

  
Intel I740:  
  Cards: Diamond Stealth II G460
         Hercules Terminator 2x/i   
         Read3D Starfighter

  OpenGL does not work well with i740 cards; we recommend using 
  Direct3D with these cards.

  Make sure to download the latest drivers from Intel. 

  Several cards have reported problems with texture corruption. If
  you experience problems, try adding the '-nolightmaps' option in
  the Misc. tab of the Descent 3 launcher setup and disabling Dynamic
  Lighting in the Options/Details menu.


Matrox G200/G400:
  Cards: Millennium G200 SG 
         Millennium G200 SD
         Millennium G200 LE    
         Millennium G200   
         Millennium G200 SD PCI
         Millennium G200 SD
         Marvel G200-TV
         Millennium G400
         Millennium G400 MAX
         Millennium G400 SH
         Millennium G400 DH
   
  No known problems. Tested with Direct3D driver version 1677_451.


nVidia RIVA TNT, TNT2:
  Cards: Asus V3400 TNT
         Canopus Spectra 2500
         Creative Labs Graphics Blaster Riva TNT 
         Diamond Viper v550
         Elsa Erazor II
         Hercules Dynamite TNT
         Leadtek WinFast 3D S320
         Prolink  PixelView
         STB Velocity 4400
         Yuan AGP-520T
         Yuan AGP-500T
   
  We recommend playing under Direct3D with the current Detonator
  drivers. 

  Enabling auto-mipmapping (via the Control Panel) on TNT cards
  can cause textures to render incorrectly in Descent 3.  If you
  experience problems, adjust the Direct3D properties for your card 
  as follows.  Open up the Control Panel, then Display Properties, 
  RIVA TNT, Advanced Properties, and finally Direct3D.  For 
  "Automatically generate XXX mipmap levels", enter zero.  For
  "Auto-mipmap method", select "Binlinear".  For "Mipmap deatil level",
  select "Best Image Quality"

  The nVidia Detonator drivers built on 02/17/99 cause serious screen
  offset problems under OpenGL. If you choose to run under OpenGL, we
  recommend the .48 reference drivers available from nVidia.


nVidia RIVA 128:
  Cards: ASUS 3d Explorer
         Canopus Total3d 128V
         Diamond Viper 330          
         STB Velocity 128
         Yuan AGP-300S

  The Riva 128 doesn't perform well with lightmaps on large polygon 
  faces.  If you experience problems, try adding the '-nolightmaps' 
  option in the Misc. tab of the Descent 3 launcher setup. 


Orchid Righteous Voodoo 2:
Orchid Righteous Voodoo 1:

  The current drivers are for DirectX version 5 only and will only
  function under Glide.
  
 
Rendition v2000:
  Cards: Diamond Stealth II S220
         Hercules Thriller 3d
      
  The Rendition v2000 doesn't perform well with lightmaps on large
  polygon faces.  We recommend adding the '-nolightmaps' 
  option in the Misc. tab of the Descent 3 launcher setup. 
  The Rendition v2000 works with Direct3D only.


S3 Savage3D:
  Cards: Hercules Terminator Beast

  Be sure to use the latest reference drivers for this card, which
  are available from http://www.s3.com.

  Movies may not play back smoothly on some systems.


S3 Savage4:

  We recommend using Direct3D with driver version 8.03.11 or later. 
  You can get the latest drivers from the S3 web site
  (http://www.s3.com).  At the current time the 8.03.11 drivers are
  under the "Sneak Peek" Betas section.

  If you do not have the suggested drivers, you should use the 
  "-subpixelcorrect" command-line switch.

  Movies may not play back smoothly on some systems.


3Dfx Voodoo:
  Cards: Ask Innovision Cyber 3Dx5000 
         A-Trend Helios Voodoo 
         Best Data Arcade Fx 
         Biostar Venus 3D Graphics 
         Canopus Pure 3D
         Deltron Flash 3D 
         Diamond Monster 3D 
         Express 3D Acceleration 
         Gainward Dragon 1000 
         Guillemot Maxigamer 3D 
         High Tech His-Arcade3D-G 
         Miro Hiscore 3D 
         Modular 
         Orchid Righteous 3D 
         Quantum 3D Obsidian 50-Series 
         Quantum 3D Obsidian 100Sb Series 
         Skywell Magic 3D 
         Techworks Power 3D 
         Village Tronics 3D Overdrive

  2 MB Voodoo1 cards may not perform well. We recommend at least 4 MB
  of texture RAM.  To compensate for the 2 MB limitation, Descent3 will 
  automatically scale down all bitmaps.  To turn this behavior off, use 
  the "-highvidmem" command line switch.

  You might also want to try using the "-nolightmaps" command line 
  switch if you are having framerate problems with this card.

  We have seen occasional texture corruption problems on this card,
  which can result in the wrong textures being drawn in some places.
  If you see this problem, you can reset the card's textures as 
  follows:  Press F2 to bring up the Setup menu; select Video;
  change the MIP Mapping flag (i.e., disable it if it's turn on, or 
  enable it if it's turned off) and exit the Setup menu.  If you want,
  you can re-enter Setup and change the MIP Mapping flag to its
  original state.


3Dfx Voodoo Rush:
  Cards: Biostar Venus 3D Rush 
         Britek Viewtop 3D Voodoo  
         California Graphics 3D Emotion 
         Deltron Flash At3D Rush 
         Fountain Tech. 
         Gainward Dragon 2000 
         Hercules Stingray 128 
         I/O Magic Rush 
         Jazz Adrenaline Rush 3D 
         Joytech Raptor Rush 
         Kadatco Aristo 
         Megafame 
         Ventura 
         Intergraph Intense 3D Voodoo 

  Descent 3 does not work on Voodoo Rush cards under Direct3D; we
  recommend playing under Glide on these cards.  
 
3Dfx Voodoo Banshee:
  Cards: Gigabyte Ga-630 
         A-Trend Helios 3D-Banshee Edition 
         Creative 3D Blaster Banshee 
         Diamond Monster Fusion 
         Elsa Victory Ii 
         Guillemot Maxi Gamer Phoenix 
         Melco Buffalo Wgp-Fx 
         Metabyte Wicked 3D Vengeance 
         Pace 3D Edge 
         Quantum 3D Raven 
         Sky Well Magic Twinpower 

  We recommend playing under Glide on all 3Dfx Voodoo cards.

3Dfx Voodoo3:
  Cards: Voodoo3 2000 AGP/PCI
         Voodoo3 3000 AGP
         Voodoo3 3500 TV

  We recommend playing under Glide. Tested with driver version 
  1.02.03. Tested with Glide drivers posted 5/8/99 on 3Dfx site.

  Cards that are based on the Voodoo3 chipset do not work properly
  when Descent 3 is minimized, causing the program to exit.  3Dfx
  knows about this problem and says (as of 9/30/99) that the bug 
  should be fixed in the next driver release.


3Dfx Voodoo 2:
  Cards: Aopen Pa2000 Voodoo2 
         Canopus Pure 3D Ii 
         Creative 3D Blaster 
         Diamond Monster 3D Ii 
         Deltron Flash 3D  
         Guillemot Gamer 3D2 PCI 
         Metabyte Wicked3D 
         Micro Conversions Game Wizard 
         Miro Hiscore Dll 
         Quantum3D Obsidian2 S-Series 
         Quantum3D Obsidian2 X-Series Single Board SLI 
         STB Blackmagic 3D 
         Techworks Power 3D Ii 
         Village Tronics 3D Overdrive 2
            
  We recommend playing under Glide on all 3Dfx Voodoo cards.

  SLI support works but has not been extensively tested. 


CONTROLLERS
-----------

Key Tronics Keyboards:

  Using the left shift key in combination with keys on the keypad
  can cause the right shift key to appear to be pressed.  Pressing
  and releasing the right shift key will fix the problem.

Logitech MouseMan:

  To use a Logitech MouseMan with Descent 3, use the "-mouseman"
  command-line switch.

Logitech Wingman Force:

  The drivers for this joystick automatically disable the
  auto-centering spring whenever a force effect is played.  This
  means that you cannot use auto-centering and force feedback at the
  same time.  If you disable force-feedback support, auto-centering
  works fine.

  Logitech has informed us that version 3.2 of the Logitech Wingman
  software (not available at the time of this writing) will contain an
  option to keep centering on even when force effects are played. 
  Visit http://www.logitech.com/ for more information.

CH Flightstick Pro

  To use a CH Flightstick Pro (or compatible) joystick with
  Descent 3, use the "-chpro" command-line option.


                      ______________________________

                        9. MAXIMIZING PERFORMANCE
                      ______________________________


Descent 3 will play great on some systems, and more slowly on others. 
The speed you achieve depends on your CPU speed and the amount of RAM
in your computer.  In general, of course, you'll get better
performance with a faster CPU and more RAM.

Assuming you can't run to the store and buy a maxed-out system, here
are some ways you can improve the game's speed.

1.  Turn your detail level to "Low" or "Medium".  To do this:

    a.  Press F2.
    b.  Click "Detail Level Settings".
    c.  Click the "Low" or "Medium" button.
    d.  Click "OK".  Then click "Done".

2.  If the game is slow in the outside terrain, go to the Detail Level
    Settings menu (see #3 above) and reduce the Render Depth distance.

3.  Reduce your screen resolution to 512x384.  You can do this in the
    "Graphics Settings" menu (accessible from "Options" on the main 
    menu). This may help in particular with cards using the 3Dfx 
    Voodoo 1 chipset, and we have also seen improvement with several 
    older cards that use the Direct3D API.
     
4.  If the game is still too slow or jerky, try turning on the "low 
    memory" or "super low memory" options.  To do this:

    a.  Double-click "Descent 3.exe".
    b.  Click "Setup".
    c.  Click "Misc.".
    d.  In the text field "Optional D3 Command Line Switches", type 
        "-lowmem" or "-superlowmem" (without the quotes, and don't 
        forget the dash) and click OK.
    e.  Click OK, click Play, and play the game again.

    The -lowmem option will reduce the size of textures in the game, 
    and the -superlowmem option will force all textures to an even 
    smaller size. This will improve the ability of lower-end 3D  
    cards to render, and the lower memory usage in main memory will 
    improve general game speed on machines that are cramped for 
    memory, regardless of the 3D card you use.

5.  While playing the game, press the - key several times to reduce 
    the area the game will take up on your screen.  This may help 
    with certain video cards.

                       
                         _______________________

                           10. TROUBLESHOOTING
                         _______________________


Q:  The game looks funny!
A:  You may have out-of-date drivers for your video card, or your
    video card may not be compatible with Descent 3.  Check 
    the Compatibility Information section of this Readme, and be
    sure to get the latest drivers from the manufacturer of your
    video card.    
    
Q:  All I do is bump into the wall!  I'm frustrated!
A:  Relax.  Hold down Z to back up a bit.  This is our #1 tip for 
    novice Descent pilots.  You'll get a better perspective on the 
    room and your situation.  You can adjust the ship's auto-leveling 
    if you press F2 and click "General Settings".

Q:  The robots keep killing me!  They are too hard!
A:  In the pilot configuration menu, you can lower your difficulty 
    level to "Trainee" for an easier experience.

Q:  My game is too slow!
A:  See "Maximizing Performance", above.

Q:  Help!  I can't see!  The game is too dark!
A:  First, adjust the brightness control on your monitor.  You may
    also want to adjust the "Brightness" control in the Graphics 
    Settings menu. You may need to use the Display control panel to 
    change your card's brightness (also called gamma) setting -- this 
    control panel will probably override any changes you attempt to 
    make in Descent 3's Graphics Settings menu.

Q:  I get a horrible sound problem where it plays part of one sound 
    over and over and over!
A:  From Setup, click the "Audio" tab and choose DirectSound.  
    Machines that have lots of memory should use the Software mixer 
    for better performance, but if you don't have a ton of RAM, use 
    DirectSound.

Q:  The sound effects play late!
A:  This is a known problem under Windows NT with DirectX 3.  It's 
    not something we can fix. Sorry.

Q:  My ship spins around and around when I'm not touching the 
    controls!
A:  Adjust the trim controls on your joystick.  Also calibrate your 
    joystick in the Descent 3 Launcher.  If these steps don't work and
    you have a Thrustmaster joystick, there is a "centering utility" 
    available that can fix this problem.  Visit the Thrustmaster web 
    page at http://www.thrustmaster.com.

Q:  When I am in piggyback observer mode in multiplayer, the display 
    is very jerky!
A:  Piggyback mode only displays a frame each time you receive a 
    position update packet from the player you are piggybacking.  
    The frequency of update packets determines the speed of the 
    observer piggyback display.

Q:  Network play is jerky and slow!
A:  Set packets per second to 5.  Try a different type of game: 
    change to peer-to-peer, or change to client-server.  Make sure the 
    server is a fast machine that experiences a good frame rate 
    while playing, or try using a dedicated server.  If some clients 
    don't have lots of RAM, try playing "The Core" multiplayer level, 
    which is fairly small.  (See the Multiplayer Readme.txt file for 
    more information.)

Q:  The game crashes on my machine!
A:  If you've found a serious bug, chances are someone else has also 
    found it and we've already fixed it.  Use the Auto-Update feature 
    or visit our web site (http://www.outrage.com) to get the latest 
    version.  If that doesn't fix it, see the Customer Service and 
    Reporting Bugs sections below.


                    __________________________________

                      11. INTERPLAY CUSTOMER SERVICE
                    __________________________________
 

TROUBLESHOOTING DOCUMENTS ONLINE!

Interplay Productions Technical Support now offers troubleshooting
guides with complete installation and setup instructions as well as
information that will help you overcome the most common difficulties.
If you have access to the World Wide Web, you can find these at
http://www.interplay.com/support/.

Here you will find troubleshooting information on as well as
information on regular system maintenance and performance.

  DirectX                http://www.interplay.com/support/directx/
  Joysticks              http://www.interplay.com/support/joystick/
  Modems and Networks    http://www.interplay.com/support/modem/

  Descent 3              http://www.interplay.com/descent3/

If you have questions about Descent 3, our Technical Support
Department can help. Our web site contains up-to-date information
on the most common difficulties with our products, and this
information is the same as that used by our product support
technicians. We keep the product support pages updated on a regular
basis, so please check here first for no-wait solutions:

   http://www.interplay.com/support/

If you are unable to find the information you need on our web site,
please feel free to contact Technical Support via e-mail, phone, 
fax, or letter.  Please be sure to include the following information
in your e-mail message, fax, or letter:

  Title of Game
  Computer manufacturer
  Operating system (Windows 95, DOS 6.22, etc.)
  CPU type and speed in MHz
  Amount of RAM
  Sound card type and settings (address, IRQ, DMA)
  Video card
  CD-ROM
  Mouse driver and version
  Joystick and game card (if any)
  A copy of the CONFIG.SYS and AUTOEXEC.BAT files from your hard drive
  A description of the problem you're having

If you need to talk to someone immediately, call us at (949) 553-6678
Monday through Friday between 8:00AM-5:45PM, Pacific Standard Time.
24 hour, 7 days per week support is available through the use of our
automated wizard.   Please have the above information ready when you
call. This will help us answer your question in the shortest possible
time. When you call you will initially be connected with our automated
wizard.  For information pertaining to your specific title, press "1"
on the main menu and listen carefully to all prompts.  All titles are
listed alphabetically.  After you have selected your title, the most
common difficulties will be listed.  If the difficulty you are having
is not listed or you need additional assistance, you may press "0" on
your games main menu, and you will be transferred to a Technical
Support Representative.  No hints or codes are available from this
line.  

    Interplay Productions Technical Support

          Voice: (949) 553-6678
            Fax: (949) 252-2820

Internet E-mail: support@interplay.com
 World Wide Web: http://www.interplay.com
            FTP: ftp.interplay.com

           Mail: Interplay Productions Technical Support
                 16815 Von Karman  Avenue
                 Irvine, CA 92606



FOR CUSTOMER SERVICE WITHIN THE UK AND EUROPE:

Write to: Customer Support
          Interplay Productions, Ltd.
          Harleyford Manor
          Harleyford
          Henley Road
          Marlow
          Buckinghamshire
          SL7 2DX
          ENGLAND

   Voice: +44 (0) 1628 423723
     Fax: +44 (0) 1628 487752
   Email: Europe@interplay.com 

Limited Warranty:

    If within the first ninety days of purchase you have any problems 
    with this product please return it to the retailer.
    
    After ninety (90) days, you may return the software program to 
    Interplay Productions, provided the fault is not due to normal 
    wear and tear, and Interplay will mail a replacement to you.
    
    To receive a replacement you should enclose the defective medium 
    (including the original product label) in protective packaging 
    accompanied by:
    
    1) A cheque or postal order for 10 pounds sterling (to cover 
       postage and packaging).
    
    2) A brief statement describing the defect.
    
    3) The original receipt or address of the retailer
    
    4) Your return address
    
    Registered mail is recommended for returns.
    
    Please send to:
    Warranty Replacements, Interplay Productions Ltd., Harleyford 
    Manor, Harleyford, Henley Road, Marlow, Buckinghamshire SL7 2DX, 
    ENGLAND



                          ______________________

                            12. REPORTING BUGS
                          ______________________


If you can't get Descent 3 working on your system, double-check all
the wonderfully helpful information in the "Troubleshooting" section,
above. If every piece of software works on your computer except for
Descent 3, then try Interplay customer service, above.  

Before reporting a bug, you should make sure you have the latest
version of the game, because the bug you've found may already be
fixed.  Use Descent 3's Auto-Update feature or visit our web site
(http://www.outrage.com) to get an update patch if one is available.

If you would like to report a bug, send e-mail to d3bugs@outrage.com.
In your e-mail, please report the following:

1.  What product and version (in this case, "Descent 3 Version 1.0").  
    The version number is shown in the lower-right corner of the main 
    menu screen.
2.  What speed and type of CPU you have (example: "Pentium II at 
    300MHz")
3.  What operating system you have (example: "Windows 98")
4.  How much RAM you have (example: 128MB)
5.  If this is a multiplayer bug, mention your network configuration
    and whether you are behind a firewall.
6.  List the bug with any information you think is relevant.

       
                           ___________________

                             13. LEGAL STUFF
                           ___________________


SOFTWARE USE LIMITATIONS AND LIMITED LICENSE

This version, Descent 3(the "Software"), is intended solely for your
personal noncommercial home entertainment use.  You may not
decompile, reverse engineer, or disassemble the Software, except as
permitted by law. Interplay Productions and Outrage Entertainment,
Inc.  retain all rights and title in the Software including all
intellectual property rights embodied therein and derivatives
thereof.  You are granted a revocable, nonassignable limited license
to create derivative works of this Software solely for your own
personal noncommercial home entertainment use and may publicly
display such derivative works to the extent specifically authorized
by Interplay in writing.  A copy of this authorization, if any, will
be provided on Interplay's World Wide Web site, located at
http://www.interplay.com, or by contacting the legal department of
Interplay Productions in the US at (949) 553-6655.  The Software,
including, without limitation, all code, data structures, characters,
images, sounds, text, screens, game play, derivative works and all
other elements of the Software may not be copied (except as provided
below), resold, rented, leased, distributed (electronically or
otherwise), used on pay-per-play, coin-op or other for-charge basis,
or for any commercial purpose. Any permissions granted herein are
provided on a temporary basis and can be withdrawn by Interplay
Productions at any time.  All rights not expressly granted are
reserved.

Modem and Network Play.  If the Software contains modem or network
play, you may play the Software via modem transmission with another
person or persons directly without transmission through a third party
service or indirectly through a third party service only if such
service is an authorized licensee of Interplay.  For the purposes of
this license, a third party service refers to any third party service
which provides a connection between two or more users of the
Software, manages, organizes, or facilitates game play, translates
protocols, or otherwise provides a service which commercially 
exploits the Software, but does not include a third party service
which merely provides a telephonic connection (and nothing more) for
modem or network play.  Authorized licensee services are listed on
the Interplay Productions World Wide Web Site located at
http://www.interplay.com. This limited right to transmit the Software
expressly excludes any transmission of the Software or any data
streams thereof on a commercial basis, including, without limitation,
transmitting the Software by way of a commercial service (excepting
those specific commercial services licensed by Interplay) which
translates the protocols or manages or organizes game play sessions. 
If you would like information about obtaining a pay-for-play or
commercial license to the Software, please contact Interplay
Productions in the US at +(949) 553-6655.  Nothing in this paragraph
is intended to prevent you from downloading the Software from
Interplay's Web site or from commercial service providers authorized
by Interplay to provide the Software to you.

Acceptance of License Terms. By downloading or acquiring and then
retaining this Software, you assent to the terms and restrictions of
this limited license. If you acquired the Software and do not accept
the terms of this limited license, you must return the Software
together with all packaging, manuals and other material contained
therein to the store where you acquired the Software for a full
refund and if you downloaded the Software, you must delete it.

Trademark and copyright (c) 1999, 2000 Interplay Productions and 
Outrage Entertainment, Inc.  All rights reserved.

