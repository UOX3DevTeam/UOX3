# UOX3
Ultima Offline eXperiment 3 - the original open source Ultima Online server emulator

news, releases, forums, additional documentation and more can be found at https://www.uox3.org

For details on UO client compatibility, check http://www.uox3.org/forums/viewtopic.php?f=1&t=2289

Requires Razor or similar tool in order to connect: https://github.com/msturgill/razor/releases

# How to compile UOX3...
# ...under Linux
Refer to instructions at https://www.uox3.org/forums/viewtopic.php?f=6&t=457

# ...under Windows
## Step 1: Clone the UOX3 Git Repository
1) Download and install GitHub Desktop (or your preferred git tool) from https://desktop.github.com/
2) Run GitHub Desktop, and then press **File->Clone Repository** (or *CTRL+SHIFT+O*)
3) Click on URL tab, then enter https://github.com/Xoduz/UOX3.git, as well as a local path for where to store the repository - for example **D:\UOX3**, then hit the Clone button. This will clone the UOX3 repository to your drive in the specified local path.

## Step 2: Compile UOX3 using your favourite compiler
### Option A) Visual Studio 2017
1) Execute **Vcvarsall.bat x86 -vcvars_ver=14.0** from a command prompt in the *C:\Program Files (x86)\Microsoft Visual Studio\2017\VC\Auxiliary\Build* folder to enable the *nmake* command
2) Next, also from a command prompt, execute **mymake.bat** from the *UOX3/spidermonkey* folder. This will create some debug/release folders with *js32.dll* and *js32.lib* files in them.
3) Start up Visual Studio 2017, open the project solution called **UOX3_Official.sln** from the *UOX3\source* folder.
4) From the toolbar menu at the top, go to **Debug -> UOX3_Official Properties -> VC++ Directories**.
5) Select the **dropdown arrow** behind *Include Directories* and choose **Edit**, then click the **New Line** button and add a reference to the **UOX3\spidermonkey** folder.
6) In the same manner, add a reference to **UOX3\spidermonkey\debug** in the *Library Directories* section, and a reference to **UOX3\spidermonkey** in the *Source Directories* section. Then press OK.
7) Select **Build -> Configuration Manager** and choose whether you want to compile a *Debug* or *Release*-version of UOX3. Debug can be useful if you want to insert break-points in the code for debugging purposes, but most of the time you want to make a Release-version.
8) Select **Build -> Build UOX3_Official** to start compiling UOX3. When done, you'll find *UOX3.exe* either in *UOX3\source\release\* or in *UOX3\source\debug*, depending on your choice in the previous step.
9) If you made a Release version, you can now **copy the new *UOX3.exe* file** to the root folder of your actual UOX3 project (not the git repository) and run it!

If you don't already have a UOX3 project folder, you can either download one from uox3.org and replace the UOX3.exe with your newly compiled one, or you can make a copy of the *UOX3\data folder* from the git repository, move it to your preferred location on your drive (I suggest outside the git repository folder, to avoid conflicts when downloading updates ot UOX3 in the future), **copy UOX3.exe** into the root folder (next to *uox.ini*). You'll also need to **copy js32.dll** (which you made in step 2) from *UOX3\spidermonkey\release* into the same folder as UOX3.exe.

Note that if you want to execute UOX3 from within Visual Studio 2017, you'll need to provide a copy of *js32.dll* (from *UOX3\Spidermonkey\debug* or *\release*) in *UOX3\source\debug* or *\release folders*, as well as specify the working directory under **Project -> Properties -> Configuration Properties -> Debugging -> Working Directory** for either Debug or Release configuration (selectable from dropdown at top of window)

### Option B) Visual Studio 2005-2015
See alternative instructions at https://www.uox3.org/forums/viewtopic.php?f=6&t=453
