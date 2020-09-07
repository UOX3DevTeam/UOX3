# UOX3
[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

**master** ![Windows x86 Build](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x86%20Build/badge.svg?branch=master)

**develop** ![Windows x86 Build - develop](https://github.com/UOX3DevTeam/UOX3/workflows/Windows%20x86%20Build/badge.svg?branch=develop)

Ultima Offline eXperiment 3 - the original open source Ultima Online server emulator, allowing people to run their own, custom UO shards since 1997. News, releases, forums, additional documentation and more can be found at https://www.uox3.org

Supported UO Client versions: **~4.0.0p** to **~7.0.86.2**. For additional details on UO client compatibility, check http://www.uox3.org/forums/viewtopic.php?f=1&t=2289

Requires Razor or similar tool in order to connect: https://github.com/msturgill/razor/releases

Also check out our [UOX3 Tools](https://github.com/UOX3DevTeam/UOX3-Tools) and [UOX3 Resources](https://github.com/UOX3DevTeam/UOX3-Resources) repositories!

Join us on the [Ultima Offline eXperiment 3](https://discord.gg/uBAXxhF) server on Discord for a quick chat!

---

# How to compile UOX3...
# ...under Linux or MacOS
## Step 1: Clone the UOX3 Git Repository
First step, open a new terminal and enter the commands below:

1) **sudo apt install git** - This will install git if not already installed (Ubuntu/Debian-based Linux variants). If you're using a non-Debian flavour of Linux, use the default package manager that comes with it to install git instead.
2) **git clone https://github.com/UOX3DevTeam/UOX3.git** - This will clone the stable branch of the UOX3 git repository into a subdirectory of the current directory you're in, named UOX3. The latest verified compatible version of SpiderMonkey (v1.7) is also included.

## Step 2: Compile UOX3
You'll need a couple tools before you can compile UOX3 on Linux: **GNU Make** (*v4.2.1* or higher recommended) and **gcc**. Install these through your favourite package manager or through your flavour of Linux' variant of the following terminal commands (which are specific to Debian/Ubuntu):

1) **sudo apt install make**
2) **sudo apt install gcc**

Once these are in place, navigate to the **UOX3/source/** directory in your terminal and execute the following commands, in order:

1) **chmod +x automake.sh** - Makes the automake.sh bash script executable.
2) **./automake.sh** - First compiles the Spidermonkey JS library bundled with UOX3, then makes the actual UOX3 build

Once this process is done, you can copy your new uox3 binary to the root of the directory you intend to be the working directory for your UOX3 shard. Along with the binary, you'll also need to copy the files and folders contained in the data subdirectory. Once you have all the files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs) to finish your UOX3 setup.

---

# ...under Windows
## Step 1: Clone the UOX3 Git Repository
1) Download and install [GitHub Desktop](https://desktop.githubc.om). If you already have another tool for git installed, you can use that instead.
2) Run GitHub Desktop (or your preferred git tool) and click File->Clone Repository from the menu.
3) Click the URL tab, enter **https://github.com/UOX3DevTeam/UOX3.git**, then provide a local path for where you want the UOX3 git repository cloned on your drive.
4) Hit the Clone button!

## Step 2: Compile UOX3 using your favourite compiler
### Option A) Visual Studio 2017/2019 ([Free Community edition](https://visualstudio.microsoft.com/downloads/))
*This option will let you use Visual Studio solution/project files to compile both UOX3 and SpiderMonkey with Visual Studio's default VC++ compiler. Note that you can download the [Free Community edition](https://visualstudio.microsoft.com/downloads/) of Visual Studio if you don't have it already.*

#### SpiderMonkey ####
1) Navigate to the **UOX3\spidermonkey** folder and open **SpiderMonkey.sln** in Visual Studio.
2) Click **Build > Batch Build** from the menu, select **js32 Debug*** and **js32 Release*** from the panel that pops up, then click the **Build** button.
3) Visual Studio will compile SpiderMonkey and create *spidermonkey\Debug* and *spidermonkey\Release* folders with **js32.dll** and **js32.lib** files in them.

#### UOX3 ####
1) Open **UOX3_Official.sln** from the **UOX3\source** folder.
2) Select **Build -> Configuration Manager** and choose whether you want to compile a Debug or Release-version of UOX3. Debug can be useful if you want to insert break-points in the code to debug what's happening in real time, but for performance and stability, make a Release-version.
3) Select **Build -> Build UOX3_Official** to start compiling UOX3. When done, you'll find **uox3.exe** either in **UOX3\source\release** or in **UOX3\source\debug**, depending on your choice in the previous step.

#### Adding SpiderMonkey references in Configuration Manager ####
<sub>*These steps are only required if the default UOX3 VS solution/project files don't already have references to SpiderMonkey setup (they should). If the steps above to compile UOX3 should fail, try the following to verify or re-add the SpiderMonkey references:*</sub>
* <sub>Click **Project -> UOX3_Official Properties -> VC++ Directories**.</sub>
* <sub>If references to SpiderMonkey are already included in the **Include Directories**, **Library Directories** and **Source Directories** sections in the form of *..\spidermonkey*, you can close the project property panel and **skip to step 4**. If not, follow the next few steps to add those references yourself.</sub>
* <sub>Select the dropdown arrow behind **Include Directories** and choose **Edit**, then click the **New Line** button and add a reference to the **UOX3\spidermonkey** folder.</sub>
* <sub>In the same manner, add a reference to **UOX3\spidermonkey\debug** in the **Library Directories** section, and a reference to **UOX3\spidermonkey** in the **Source Directories** section. Then press apply.</sub>
* <sub> Repeat this process for both Release and Debug configurations (chosen at top of panel).</sub>

### Option B) Visual Studio 2017/2019 and CMake ([Free Community edition](https://visualstudio.microsoft.com/downloads/))
*This option requires installing the Visual Studio toolset named **Linux development with C++** and the component called **Visual C++ tools for CMake and Linux**. Use the Visual Studio Installer to modify your install of Visual Studio if you don't already have these installed!*

1) Start Visual Studio and use **File > Open > CMake** and select *CMakeLists.txt* in the root project folder (*Example: **D:\UOX3***). **Don't** open the similarly named file in the source folder directly.
2) After Visual Studio is done loading the project and has generated some necessary files, select **CMake > Change CMake Settings > UOX3** from the toolbar menu, and select either *x86-Debug* or *x86-Release* depending on what kind of build you want to make - or do this using the Solution Configuration select menu.
3) When Visual Studio is done switching to the new configuration, select **CMake > Build All** from the toolbar menu to start compiling UOX3.
4) When done, you'll find **uox3.exe** and **js32.dll** in a subfolder named **out** of the root project folder, more specifically **/UOX3/out/build/x86-Debug** or **x86-Release**, based on the selected configuration.

Once this process is done, you can copy your new **uox3.exe** and **js32.dll** files from the appropriate output folders (depending on which method and configuration you used) to the root folder of your actual UOX3 project. You'll also need to copy the files and folders contained within the **data** subfolder of the UOX3 repository, if you don't already have these. Once you have all the files in place, you can follow the regular steps listed under **Installation and Setup > Configuring Your UOX3 Shard** in the UOX3 documentation (see docs folder, or visit https://www.uox3.org/docs) to finish your UOX3 setup!
