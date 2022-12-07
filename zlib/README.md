# zlib-1.2.11 
[Ultima Offline eXperiment 3](https://github.com/UOX3DevTeam/UOX3/) utilizes zlib for access to Ultima Online data files and packets.
This version comes with build setups in the make/ folder for making a static library. Other build files have been removed.

For the full, official zlib project complete with compilation solutions for other platforms and third party contributions, visit https://github.com/madler/zlib

For detailed instructions on compiling and using this zlib configuration with UOX3, check out https://github.com/UOX3DevTeam/UOX3.

<details>
  <summary>Visual Studio 2022</summary>
	Traverse to the make/vs2022/zlib folder, and open zlib.sln.
	Select either Release/Debug, and Build it.
</details>

<details>
  <summary>CMake</summary>

	Traverse to the make/cmake directory
	1) Create a build location 
		- Enter: mkdir build  
	2) Move to that locations
		- Enter: `cd build`  
	3) Create the make files   
		- **WINDOWS**  (from a Developers command prompt)
			- Enter:  cmake .. -DCMAKE_BUILD_TYPE=Release -G"NMake Makefiles" 
		- **macOS** 
			- Enter: cmake .. -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"  
		- **All Other Operation Systems**    
			- Enter:  cmake .. -DCMAKE_BUILD_TYPE=Release  
	4) Build the system entering: cmake --build . --config Release
	5) The resulting product will be in the current (build) directory  
	
</details>