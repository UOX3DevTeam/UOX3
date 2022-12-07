# UOX3
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) [![Coverity Scan Build Status](https://scan.coverity.com/projects/23322/badge.svg)](https://scan.coverity.com/projects/ultima-offline-experiment-3)

**master** ![Windows x86 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x86%20Build/badge.svg?branch=master) ![Windows x64 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x64%20Build/badge.svg?branch=master) ![Linux x64 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Linux%20x64%20Build/badge.svg?branch=master)

**develop** ![Windows x86 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x86%20Build/badge.svg?branch=develop) ![Windows x64 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x64%20Build/badge.svg?branch=develop) ![Windows x64 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Linux%20x64%20Build/badge.svg?branch=develop)

**Ultima Offline eXperiment 3** - the original open source Ultima Online server emulator, allowing people to run their own, custom UO shards since 1997. Comes with cross-platform 64-bit support for **Windows**, **Linux**, **macOS**, and **FreeBSD**. News, releases, forums, additional documentation and more can be found at https://www.uox3.org

Supported UO Client versions: **~4.0.0p** to **~7.0.91.15** (with encryption removed by [ClassicUO](https://www.classicuo.eu), [Razor](https://github.com/msturgill/razor/releases) or similar tools). For additional details on UO client compatibility, check https://www.uox3.org/forums/viewtopic.php?f=1&t=2289

UOX3 relies on **SpiderMonkey v1.7.0** for its JS-based scripting engine, and on **zlib-1.2.11** for data compression matters, and comes bundled with specific, compatible versions of these.

Join the [UOX3 Discord](https://discord.gg/uBAXxhF) for support and/or a quick chat!

---

# How to compile UOX3...
UOX3 supports Integrated Development Environment(IDE) building for Visual Studio 2022, and XCode.  CMake is available for command line building.
## Step 1: Setting up an environment
### Acquire the compiler and build tools
	-) **Windows** Visual Studio 2022 ([Free Community edition](https://visualstudio.microsoft.com/downloads/))
	-) **macOS** Via the App store,download XCode.
	-) **Linux** Debian based distributions execute: 'sudo apt install build-essentials'
### Aquire Git
	-) **Windows or macOS** Download [GitHub Desktop](https://desktop.github.com/)
	-) **Linux** Debian based distributions execute: 'sudo apt install git'
	
*** Acquire CMake (if using command line build generation)*
	-) **Windows** CMake is part of the Visual Studio IDE, nothing more is needed
	-) **macOS** Download and install [CMake](https://cmake.org/download/)
	-) **Linux** Debian based sitrubitons execute: 'sudo apt install cmake'
## Step 2: Cloning the software
	-) **GitHub Desktop**
		-) Run GitHub Desktop and click **File->Clone Repository** from the menu.
		-) Click the **URL** tab, enter **https://github.com/UOX3DevTeam/UOX3.git**, then provide a local path for where you want the UOX3 git repository cloned on your drive.
		-) Hit the **Clone** button!
	-) **Command Line**
		-) `git clone https://github.com/UOX3DevTeam/UOX3.git`
  			-)If you'd rather grab another branch of the git repository, like the **develop** branch where most updates get pushed first before being merged into the master branch, you can use the following command *after* completing the previous step: `git checkout develop`
## Step 3: Compile UOX3
	-) **Windows** Open the UOX3\make\VS2022\uox3\uox3.sln file, select build.
	-) **macOS** Open the UOX3/make/XCode/uox3/uox3.xcworkspace.  Select build.
	-) **Linux**
		<details> 
			<summary> Single command</summary>
			Enter from a termain windows in the UOX3 directory: `./automake.sh`
		</details>
		<details>
			<summary>Individual steps</summary>
			Enter the following:
			-) `mkdir build`
			-) `cd build`
			-) `cmake ../make/cmake -DCMAKE_BUILD_TYPE=Release `
			-) `cmake --build . --config Release`
		</details>
	The binary/executable will be located:
		**Windows** UOX3\make\vs2022\uox3\x64\Release\uox3.exe
		**macOS** UOX3\make\XCode\uox3\Build\Products\Release\uox3
		**Linux** UOX3/make/cmake/build/uox3

Once you have all the required files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs/index.html#configureUOX3) to finish your UOX3 setup!
