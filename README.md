# sead

This is a decompilation of sead, the standard C++ library for first-party Nintendo games.

Unlike the [sead cafe decompilation project](https://github.com/aboood40091/sead) & [sead NX decompilation project](https://github.com/open-ead/sead) & [sead windows project](https://github.com/stupidestmodder/sead/tree/main), which this repo derives from, this project targets the 3DS version of sead.

The objective is to recreate the standard library as accurately as possible, so that interoperability can eventually be achieved by adding support for other platforms and by making it easier to create projects that interact with sead games.

Because sead is statically linked in games (and only in games), acquiring the original sead requires legally owning at least one recent first-party Nintendo game. Picking a game that ships with debugging symbols is strongly recommended:

-NX-

* Super Mario Odyssey (version 1.0.0) ([buy it here](https://www.nintendo.com/games/detail/super-mario-odyssey-switch/))
* Splatoon 2 (version <= 3.1.0) ([buy it here](https://www.nintendo.com/games/detail/splatoon-2-switch/))
* [Nintendo Labo](https://labo.nintendo.com/) (the pilot build has symbols, file names and assertions)
* Any other title that has symbols and uses sead

-CTR-

* Mario Kart 7 (Download Play all regions except Chinese) ([purchase discontinued](https://www.nintendo.com/au/games/nintendo-3ds/mario-kart-7/))
* Animal Crossing: New Leaf Welcome Amiibo(version 1.2 and above, RTTI) ([purchase Discontinued](https://www.nintendo.com/au/games/nintendo-3ds/animal-crossing-new-leaf-welcome-amiibo/))

File names, function names and the file organization come from debugging symbols, assertions and information in all of the aforementioned titles.
Nobody except Nintendo has the source code of sead, not even third-party developers.

Note that many names (especially for inlined, templated functions) are just plain guesses.

## Folder Structure

* **/LIBRARY_ROOT/sead/**

*    |____ **addins** - Additional libraries used by *sead*.

*    |____ **include** - Headers used for *sead*.

*    |____ **lib** - Libraries used by *sead*.

*    |____ **modules/src** - Module source code.

*    |____ **template/ctr_nw4c** - Template files for the **ctr_nw4c** framework. Used by games using the *sead* engine. (Such as code, shaders, etc.)

## Addins

* **libms** - LibMessageStudio for CTR

## Libraries

* **CtrSDK** - The standard Software Development Kit for 3DS.
* **Nw4cEngine** - The NintendoWare4Ctr (NW4C) Graphics and Sound engine.

## Modules

For progress, refer to [the GitHub project page](https://github.com/LoigiFan72/sead). Several modules currently fail to build for Switch.

* **audio** - Audio and sound
* **basis** - Types, asserts, allocation operators
* **codec** - Base64, CRC16, CRC32
* **container** - Templated container classes
* **controller** - Controller
* **devenv** - Development environment (debug utilities)
* **filedevice** - File IO
* **framework** - Framework (game framework, tasks, etc.)
* **geom** - Geometry
* **gfx** - Graphics
* **heap** - Heap (arenas, disposers, different types of heaps)
* **hostio** - Host IO (communication with PCs)
* **math** - Maths utilities (vector, matrix, etc.)
* **message** - Message Studio wrapper
* **prim** - Primitives (strings, enums, RTTI, etc.)
* **random** - Random number generator
* **resource** - Resource (loading, decompressing, etc.)
* **stream** - Stream IO
* **tentative** - Tentative resources (Bitmap handler)
* **thread** - Thread utilities (threads, critical sections, message queues, etc.)
* **time** - Time utilities

### Platform specific source

Platform-specific files are usually placed into a subdirectory that is called:

* **ctr** for 3Ds
* **winctr** for CTR Emulated Windows

### Platform Frameworks

## ctr

* **ctr/ConsoleFrameWorkCtr** — Basic CTR application framework. Only initializes one screen for the device.

* **ctr_nw4c/GameFrameworkCtrNw4c** — Base game framework with compatability with the **Nw4c** engine. Builds upon `GameFramework` and provides groundwork for using Nw4c with CTR. 

* **ctr_nw4c/DoubleCmdGameFrameworkCtrNw4c** — Dual-screen game framework. Extends `GameFrameworkCtrNw4c` to initialize and manage both the top and bottom screens, including their respective frame buffers and presentation.

* **ctr_nw4c/UlcdDoubleCmdGameFrameworkCtrNw4c** — ULCD dual-screen framework. Extends `DoubleCmdGameFrameworkCtrNw4c` and adapts its display handling for a left/right screen configuration, primarily presenting and managing the left and right displays.

### Version specific source

Different features of sead can be implemented/left out in conjunction to which game the library is being used for:

Set `SEAD_VERSION` to:
- `SEAD_VERSION_NONE`      (0): Placeholder until determined
- `SEAD_VERSION_REDPEPPER` (1): Super Mario 3D Land
- `SEAD_VERSION_CTRDASH`   (2): Mario Kart 7
- `SEAD_VERSION_GARDEN`    (3): Animal-Crossing: New Leaf
- `SEAD_VERSION_BIGRED`    (4): New Super Mario Bros. 2

Presets and features for more games can be added if desired.

## Building

Building this project requires:

- ARM C++ Complier (ARMCC) Version 4.0/4.1/5.0 [which can be found here.](https://github.com/RE-Pepper/data/releases/tag/dasdasdsa)
- The Nintendo 3DS Software Development Kit hooked to your project [which can be found here.](https://github.com/LoigiFan72/CTRSDK).
- The Nintendo 3DS NintendoWare Graphics / Sound engine hooked to your project [which can be found here.](https://github.com/LoigiFan72/NW4C). (*required* for **audio**, **gfx**, **framework/ctr_nw4c** libraries.)
- **Note:** Compiler is the same has your games version. i.e. MK7 Uses 894, so **sead** will use the same.

### Configuration

sead can be configured with several compile-time defines:

* `SEAD_DEBUG`: enables assertions and HostIO code.

#### Platforms
* `CTRSDK` : Platform for CTR
* `WINDOWS_CTR` : Platform for Windows emulating CTR

Other platforms (generic Unix, iOS, Android, NX, and cafe) are not supported.

## Contributing

### Non-inlined functions
When **implementing non-inlined functions**, please compare the assembly output against the original function and make it match the original code. At this scale, that is pretty much the only reliable way to ensure accuracy and functional equivalency.

However, given the large number of functions, certain kinds of small differences can be ignored when a function would otherwise be equivalent:

* Regalloc differences.

* Instruction reorderings when it is obvious the function is still semantically equivalent (e.g. two add/mov instructions that operate on entirely different registers being reordered)

When ignoring minor differences, add a `// NOT_MATCHING: explanation` comment and explain what does not match.

### Header utilities or inlined functions
For **header-only utilities** (like container classes), use pilot/debug builds, assertion messages and common sense to try to undo function inlining. For example, if you see the same assertion appear in many functions and the file name is a header file, or if you see identical snippets of code in many different places, chances are that you are dealing with an inlined function. In that case, you should refactor the inlined code into its own function.

Also note that introducing inlined functions is sometimes necessary to get the desired codegen.

If a function is inlined, you should try as hard as possible to make it match perfectly. For inlined functions, it is better to use weird code or small hacks to force a match as differences would otherwise appear in every single function that inlines the non-matching code, which drastically complicates matching other functions. If a hack is used, wrap it inside a `#ifdef MATCHING_HACK_{PLATFORM}` (see above for a list of defines).

## Planned Devices ##

* **winctr** - Allow a Windows Device to Emulate the CTR Platform.

### Tentative PR Contributing rules
The `ctrdecomp` organization follows a set of standards to maintain consistency and quality across our projects. To help contributors meet these standards, our team has established the following guidelines:

* **All code must be submitted through the GitHub Pull Request process.**

* **Code must not be obtained from illegal or unauthorized material.** If such material is detected, the contribution **will not** be accepted.

* **Use of AI must be disclosed.** Contributors must disclose when and where they use AI.

* **All code must be reviewed by a human before submission.** Contributors are responible for reviewing to match styling, errors, etc.

* **Decompiled code should be reasonably representative of how the original source code may have been written. Avoid excessive or unnecessary pointer arithmetic when the underlying data is clearly identifiable as a struct or class.** In general, a raw Ghidra decompilation that merely compiles is not sufficient for PR acceptance; the code should be properly cleaned up, structured, and made readable.

* **Most functions should have a corresponding Doxygen documentation comment above its top-most declaration.** Most one-line functions are exempt here, but generally over 2-3 lines is a decent rule of thumb.

* **All code must be C++03-compliant.**