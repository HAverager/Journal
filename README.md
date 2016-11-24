**Commands for installation with CMake:** 
mkdir build  
cd  build/  
cmake ..  
make  

**Commands for installation with Makefile:**
make  

**Doxygen documentation:**
cd doc  
doxygen Doxyfile.in  

Created documentation is available as html in folder ./doc/html  


**Using library as root macro**
Create rootlogon.C file with following code (put correct path to the library and includes):  
{  
	gSystem->Load("../libjournal.so");  
	gROOT->SetMacroPath("../include");  
	gROOT->ProcessLine(".L journal.h");  
}  
Copy this file to the place, where the root is lunched.  

