# UOX3
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![Coverity Scan Build Status](https://scan.coverity.com/projects/23322/badge.svg)](https://scan.coverity.com/projects/ultima-offline-experiment-3)

**master** ![Windows x86 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x86%20Build/badge.svg?branch=master) ![Windows x64 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x64%20Build/badge.svg?branch=master) ![Linux x64 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Linux%20x64%20Build/badge.svg?branch=master)

**develop** ![Windows x86 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x86%20Build/badge.svg?branch=develop) ![Windows x64 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x64%20Build/badge.svg?branch=develop) ![Windows x64 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Linux%20x64%20Build/badge.svg?branch=develop)

**Ultima Offline eXperiment 3** - the original open source Ultima Online server emulator, allowing people to run their own, custom UO shards since 1997. Comes with cross-platform 64-bit support for **Windows**, **Linux** and **macOS**. News, releases, forums, additional documentation and more can be found at https://www.uox3.org

Supported UO Client versions: **~4.0.0p** to **~7.0.90.16** (with encryption removed by [Razor](https://github.com/msturgill/razor/releases) or similar tool). Open source clients like [ClassicUO](https://www.classicuo.eu) also supported. For additional details on UO client compatibility, check https://www.uox3.org/forums/viewtopic.php?f=1&t=2289

UOX3 relies on **SpiderMonkey v1.7.0** for its JS-based scripting engine, and on **zlib-1.2.11** for data compression matters, and comes bundled with specific, compatible versions of these.

Join the [UOX3 Discord](https://discord.gg/uBAXxhF) for support and/or a quick chat!

---

# How to compile UOX3...
# ...under Linux or macOS
## Step 1: Clone the UOX3 Git Repository
<details>
  <summary>Using git and Terminal</summary>

First step, open a new terminal and enter the commands below:

1) (Linux) `sudo apt install git` - This will install git if not already installed (Ubuntu/Debian-based Linux variants). If you're using a non-Debian flavour of Linux, use the default package manager that comes with it to install git instead.
1) (macOS) `xcode-select --install` - This will install git if not already installed, along with required make and gcc tools
2) `git clone https://github.com/UOX3DevTeam/UOX3.git` - This will clone the stable master branch of the UOX3 git repository into a subdirectory of the current directory you're in, named UOX3. The latest verified compatible version of SpiderMonkey (v1.7.0) is also included, as well as a minimal set of files required to compile zlib-1.2.11.

<details>
  <summary>Checking out Other Branches</summary>

  If you'd rather grab another branch of the git repository, like the **develop** branch where most updates get pushed first before being merged into the master branch, you can use the following command *after* completing the previous step:
  `git checkout develop`

</details>
</details>

<details>
  <summary>(macOS alternative) GitHub Desktop</summary>

  1) Download and install the macOS version of [GitHub Desktop](https://desktop.github.com/).
  2) Run GitHub Desktop and click **File->Clone Repository** from the menu.
  3) Click the **URL** tab, enter **https://github.com/UOX3DevTeam/UOX3.git**, then provide a local path for where you want the UOX3 git repository cloned on your drive.
  4) Hit the **Clone** button!

</details>

## Step 2: Compile UOX3
<details>
  <summary>Compiling with GNU Make and GCC (v9.x and above)</summary>

You'll need a couple tools before you can compile UOX3 on Linux, like **GNU Make** (*v4.2.1* or higher recommended) and **gcc** (*v9.x* or higher recommended). Install these through your favourite package manager or through your flavour of Linux' variant of the following terminal command (example specific to Debian/Ubuntu Linux flavours):

1) (Linux only) `sudo apt install build-essential`

Once these are in place, navigate to the **UOX3** project folder in your terminal and execute the following command from the project's root directory:

`./automake.sh` - First compiles the SpiderMonkey JS library bundled with UOX3, then compiles the included zlib library, and finally compiles the actual UOX3 build, before copying the compiled binary to the **root** UOX3 project directory.
</details>

<details>
  <summary>Special note for users of Debian/GCC versions below 9.x</summary>

  If your version of gcc is below 9.x, you'll need to add *-lstdc++fs* in *Makefile* to include the experimental filesystem library from c++17.

  Change the following line (~34) from

    `$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS)`

  to

    `$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS) -lstdc++fs`
</details>

<details>
  <summary>Manual Instructions</summary>

  If you don't wish to rely on the automake.sh script, but want control over the process yourself, follow these steps (same as what automake.sh does):

  - `cd spidermonkey`
  - `make -f Makefile.ref DEFINES=-DHAVE_VA_LIST_AS_ARRAY CC=gcc`
  ### Linux ###
  - `ar -r libjs32.a Linux_All_DBG.OBJ/*.o`
  - `cp Linux_All_DBG.OBJ/jsautocfg.h ./`
  ### macOS ###
  - `ar rcs libjs32.a Darwin_DBG.OBJ/*.o`
  - `cp Darwin_DBG.OBJ/jsautocfg.h ./`

  *Next up, compile a static library of the included zlib package:*

  - `cd ../zlib`
  - `make distclean`
  - `./configure`
  - `make`

  *At this point, now cd to the root UOX3 project directory and build UOX3:*

  - `cd ../source`
  - `make`

</details>

---

Once done compiling, you will find the compiled uox3 binary in the root UOX3 directory. You can copy this binary to the directory you intend to run your UOX3 shard from, along with all the files and folders contained in the UOX3/data subdirectory.

**It is recommended** to run your UOX3 shard from a separate, dedicated directory instead of the data directory in your local UOX3 git repository, to avoid potential git conflicts and accidental overwrites when pulling updates to UOX3 from GitHub in the future.

<details>
  <summary>Copying Required Files to Dedicated UOX3 Directory</summary>

This is an example of how to copy all required files to a directory called UOX3 in your user account's home directory
1) *navigate to root UOX3 project directory*
2) `mkdir ~/UOX3`
3) `cp uox3 ~/UOX3`
4) `cp -r data/* ~/UOX3`
5) `cd ~/UOX3`

</details>

Once you have all the required files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs/index.html#configureUOX3) to finish your UOX3 setup.

---

# ...under Windows
## Step 1: Clone the UOX3 Git Repository
1) Download and install [GitHub Desktop](https://desktop.githubc.om). If you already have another tool for git installed, you can use that instead.
2) Run GitHub Desktop (or your preferred git tool) and click **File->Clone Repository** from the menu.
3) Click the **URL** tab, enter `https://github.com/UOX3DevTeam/UOX3.git`, then provide a local path for where you want the UOX3 git repository cloned on your drive.
4) Hit the **Clone** button to clone the stable master branch of the UOX3 git repository to the specified local path, along with the latest verified compatible version of SpiderMonkey (v1.7.0).

## Step 2: Compile UOX3
### Option A) Visual Studio 2017/2019 ([Free Community edition](https://visualstudio.microsoft.com/downloads/))
<details>
  <summary>Visual Studio 2017/2019</summary>

*This option will let you use Visual Studio solution/project files to compile both UOX3 and SpiderMonkey with Visual Studio's default VC++ compiler. Note that you can download the [Free Community edition](https://visualstudio.microsoft.com/downloads/) of Visual Studio if you don't have it already. This approach also embeds SpiderMonkey directly inside UOX3 for a slightly larger (~1-2MB) executable, instead of requiring a separate DLL file, and comes with options for compiling either **32-bit** or **64-bit** (default) versions of UOX3.*

***Note:*** You'll need to install **"Desktop development with C++"** via the Visual Studio Installer if you don't have it already, along with the option for this titled **MSVC v141 - VS 2017 C++ x64/x86 build tools (v14.16)**

#### SpiderMonkey ####
1) Navigate to the **UOX3\spidermonkey** folder and open **SpiderMonkey.sln** in Visual Studio.
2) Make sure you have **js32** selected in the Solution Explorer, then select **Release** and either **x64** (64-bit) or **Win32** (32-bit) in the Solution Configuration/Platform dropdown menus
3) Click **Build > Build js32** from the menu.
4) Visual Studio will compile SpiderMonkey and create **spidermonkey\Release\x64** (64-bit) or **spidermonkey\Release\x86** (32-bit) folders with the compiled **js32.lib** library file contained within. No further actions are necessary here, so you can close the SpiderMonkey VS Solution.

### zlib ###
1) Navigate to the **UOX3\zlib** folder and open **zlib.sln** in Visual Studio.
2) Select **Release** and either **x64** (64-bit) or **Win32** (32-bit) in the Solution Configuration/Platform dropdown menus
3) Click **Build > Build zlib-static** from the menu.
4) Visual Studio will compile zlib and create **zlib\x64\Release** (64-bit) or **zlib\x86\Release** (32-bit) folders with the compiled **zlib-static.lib** library file contained within. No further actions are necessary here, so you can close the zlib VS Solution.

#### UOX3 ####
1) Open **UOX3_Official.sln** from the **UOX3\source** folder.
2) Make sure you have **UOX3_Official** selected in the Solution Explorer, then select either **Release** or **Debug**, and either **x64** (64-bit) or **Win32** (32-bit) in the *Solution Configuration/Platform dropdown menus*, or via **Build -> Configuration Manager**.
3) Select **Build -> Build UOX3_Official** to start compiling UOX3. When done, you'll find **UOX3.exe** either in **UOX3\source\Release\x64** (or **\x86**) or in **UOX3\source\Debug\x64** (or **\86**), depending on your choices in the previous step.

<details>
  <summary>Adding SpiderMonkey/zlib references in Configuration Manager</summary>

If VS give you link errors when attempting to build UOX3, references to SpiderMonkey or zlib might have gone missing! Try the following steps to add them back.

1) Right click on **UOX3_Official** in the Solution Explorer, and select Properties.
2) With the desired configuration (ex: Release, x64) selected at the top of the panel, add references to SpiderMonkey and zlib in these sections:
  * *VC++ Directories >* **Include Directories** (add path to SpiderMonkey and zlib root folders)
  * *VC++ Directories >* **Library Directories** (add path to SpiderMonkey **Release\x64** or **Release\x86** folder, as well as zlib **\x64\Release** or **\x86\Release** folder, depending on desired configuration)
  * *VC++ Directories >* **Source Directories** (add path to SpiderMonkey and zlib root folders)
  * *Linker >* **Additional Library Dependencies** (add path to SpiderMonkey **Release\x64** or **Release\x86** folder, as well as zlib **\x64\Release** or **\x86\Release** folder, depending on desired configuration)
Press apply!
Repeat process for both Release and Debug configurations (chosen at top of panel), then retry the UOX3 build process!

</details>
</details>

### Option B) Visual Studio 2017/2019 and CMake ([Free Community edition](https://visualstudio.microsoft.com/downloads/))
<details>
  <summary>Visual Studio 2017/2019 and CMake</summary>

*This option requires installing the Visual Studio toolset named **Linux development with C++** and the component called **Visual C++ tools for CMake and Linux**. Use the Visual Studio Installer to modify your install of Visual Studio if you don't already have these installed! This approach compiles SpiderMonkey to a separate DLL (**js32.dll**) file that needs to live inside the same folder as the main UOX3 executable, and currently only supports compiling a **32-bit** version of UOX3.*

1) Start Visual Studio and use **File > Open > CMake** and select *CMakeLists.txt* in the root project folder (*Example: **D:\UOX3***). **Don't** open the similarly named file in the source folder directly.
2) After Visual Studio is done loading the project and has generated some necessary files, select **CMake > Change CMake Settings > UOX3** from the toolbar menu, and select either *x86-Debug* or *x86-Release* depending on what kind of build you want to make - or do this using the Solution Configuration select menu.
3) When Visual Studio is done switching to the new configuration, select **CMake > Build All** from the toolbar menu to start compiling UOX3.
4) When done, you'll find **uox3.exe** and **js32.dll** in a subfolder named **out** of the root project folder, more specifically **/UOX3/out/build/x86-Debug** or **x86-Release**, based on the selected configuration.
5) Note that the **js32.dll** file must be copied to the same folder as UOX3.exe.

</details>

---

Once done compiling, you can copy your new **uox3.exe** (and if using CMake, **js32.dll**) file from the appropriate output folders (depending on which method and configuration you used) to the root folder of your actual UOX3 project. You'll also need to copy the files and folders contained within the **data** subfolder of the UOX3 repository, if you don't already have these.

**It is recommended** to run your UOX3 shard from a separate, dedicated folder instead of the data folder in your local UOX3 git repository, to avoid potential git conflicts and accidental overwrites when pulling updates to UOX3 from GitHub in the future.

Once you have all the required files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs/index.html#configureUOX3) to finish your UOX3 setup!
