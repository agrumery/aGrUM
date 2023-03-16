aGrUM samples
(c) Pierre-henri Wuillemin & Christophe Gonzales
LIP6 2011-2023


In each directory, 

-------------- Linux ------
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Releas
make 
./example
---------------------------

------------- Windows --------
mkdir build
cd build
cmake ..  -DCMAKE_BUILD_TYPE=Release
MSBuild.exe .\EXAMPLE.sln  /property:Configuration=Release
 .\Release\example.exe
------------------------------
(or you can open EXAMPLE.sln in visual C++)


should execute the example