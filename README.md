# Steganography-Project
Software to embed and extract messages within png files
Project Page: https://github.com/55thuser/Steganography-Project
### Compiling
lodepng.cpp, lodepng.h and StegProj.cpp must be in the same file for compiling
To compile use the following command:   g++ StegProj.cpp lodepng.cpp
### Configuration
StegProj.cpp uses a configuration folder for easy access to file names
The configuration file is whitespace sensitive so a space character must appear before and after your input e.g: mode: X 

pngName:    Name of png you would like to embed to or extract from

fileName:   Name of file to be embedded, only used in embedding process
outputName: Name of output file
width:      Width of image
height:     Height of image
bits:       Bits per byte of png RGBA data to be overwritten, will use least significant bits min:1 max:8
mode:       Mode to be used 'E' for embedding 'X' for extracting

### Lodepng
Lodepng was not created by me the project can be found here: https://github.com/lvandeve/lodepng
the contributers of lodepng are not affiliated with this project