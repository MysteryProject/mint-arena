**Changes**

  * Unreal tournament kill awards, see [New Client CVARS](../../wiki/New-Cvars#other)
  * Instagib weapon server command, see [New Server CVARS](../../wiki/New-Cvars#misc)
  * New obituary feed similar to battlefield 4, see [Obituary Feed CVARS](../../wiki/New-Cvars#obituary-feed)
  * New mapcycle config server command, see [New Server CVARS](../../wiki/New-Cvars#misc)
  * New weapons: Mini-Railgun, Auto-Shotty, Tap-Rifle, Impact Cannon
  * New [Game Modes](../../wiki/Game-Modes)

**Custom Spearmint Q3** game code.

To use this you'll need the [Spearmint engine](https://github.com/zturtleman/spearmint).

  * On Windows, install [Cygwin and mingw-w64](https://github.com/zturtleman/spearmint/wiki/Compiling#windows).
  * Get the source for Spearmint and build it using `make`.
  * Get the source for this repo and build it using `make`.
  * Copy the pak?.pk3 files for baseq3 and missionpack into the directories in `mint-arena/build/release-mingw32-x86/`.
  * Copy the [spearmint-patch-data](https://github.com/zturtleman/spearmint-patch-data) for baseq3 and missionpack there too.

If you put both projects in the same directory you can launch the game using;

    spearmint/build/release-mingw32-x86/spearmint_x86.exe +set fs_basepath "mint-arena/build/release-mingw32-x86/" +set fs_game "baseq3"

On Linux and OS X you'll need to put `./` before the command and substitute the correct platform and architecture (look in the build directory).

