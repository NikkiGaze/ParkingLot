# ParkingLot 
Known issues:
- Drawing is supported only under linux 
- Drawing x-axis is in timastamps, not in dates (limit of used plotting lib)

Credits:
- for plotting, simple 3d party wrapper of python lib is used, taken from here https://github.com/lava/matplotlib-cpp
- for reading json, simple 3d party lib is used from here https://github.com/nlohmann/json

LINUX:
-----------
REQUIREMENTS
- python2.7 python-matplotlib (if you want drawing)

TO BUILD
- g++ main.cpp -std=c++11 -I/usr/include/python2.7 -lpython2.7

TO RUN
- ./a.out <sample.txt> or <sample.json> [-DDRAW_PLOT]
-----------

WIN:
-----------
TO BUILD
- cl main.cpp

TO RUN
- main.exe sample.txt
-----------
