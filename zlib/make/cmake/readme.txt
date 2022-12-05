1) Open a terminal window    
	- **Windows** users note:  
		- From the windows Start menu, open a Developers Command Prompt  
2) Navigate to your *UOX3* directory where you cloned using git  
2) Create a build location 
	- Enter: mkdir build  
3) Move to that locations
	- Enter: `cd build`  
4) Create the make files   
	- **WINDOWS**  
		- Enter:  cmake .. -DCMAKE_BUILD_TYPE=Release -G"NMake Makefiles" 
	- **macOS** 
		- Enter: cmake .. -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"  
	- **All Other Operation Systems**    
		- Enter:  cmake .. -DCMAKE_BUILD_TYPE=Release  
5) Build the system entering: cmake --build . --config Release
6) The resulting product will be in the current (build) directory  
