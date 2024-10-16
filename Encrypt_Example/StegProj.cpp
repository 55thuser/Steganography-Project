#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "lodepng.h"
#include <iomanip>
std::string pngName;
std::string fileName;
std::string outputName;
unsigned int imageWidth;
unsigned int imageHeight;
int bits;
char mode;
//Project Page: https://github.com/55thuser/Steganography-Project
int returnInputInt(std::string inputString){
    int loc = inputString.find(" ");
    std::string numberString = inputString.substr(loc, inputString.length());
    return std::stoi(numberString);
}
char returnInputChar(std::string inputString){
    int loc = inputString.find(" ");
    std::string numberString = inputString.substr(loc, inputString.length());
    return numberString[1];
}
std::string returnInputString(std::string inputString){
    int loc = inputString.find(" ");
    int loc1 = inputString.find(" ",loc+1);
    std::string numberString = inputString.substr(loc+1, loc1-loc-1);
    return numberString;
}
void readConfig(){
    std::ifstream steg;
    steg.open("steganography.cfg");
    std::vector<std::string> inputs;
    int start = 0;
    std::string temp;
    while(std::getline(steg,temp)){
        inputs.push_back(temp);
    }
    pngName = returnInputString(inputs[0]);
    fileName = returnInputString(inputs[1]);
    outputName = returnInputString(inputs[2]);
    imageWidth = returnInputInt(inputs[3]);
    imageHeight = returnInputInt(inputs[4]);
    bits = returnInputInt(inputs[5]);
    mode = returnInputChar(inputs[6]);
}
void decompress(std::vector<u_int8_t> &image, std::string filename){
    int errorCode = lodepng::decode(image, imageWidth, imageHeight, filename);
    if(errorCode){
        std::cout << "decoder error " << errorCode << ": " << lodepng_error_text(errorCode) << std::endl;
    }
    return;
}
bool bitRead(u_int8_t byte, u_int8_t numBit){
    u_int8_t selector = 0b00000001;
    selector = selector<<numBit;
    u_int8_t output = byte&selector;
    if(output>0){
        return 1;
    }
    return 0;
}
void bitWrite(u_int8_t &byte, int numBit, bool value){
    u_int8_t selector = 0b00000001;
    selector = selector<<numBit;
    if(value)
        byte = byte|selector;
    else{
        selector = ~selector;
        byte = byte&selector;
    }
    return;
}

void writeData(std::vector<u_int8_t> &image){
    std::ifstream inputFile(fileName);
    char current;
    int byteLocation = 0;
    int bitLocation = bits-1;
    while(inputFile.get(current)){
        for(int i = 7; i>=0; i--){
            bool bit = bitRead((u_int8_t)current, i);
            //std::cout<<+current;
            bitWrite(image[byteLocation], bitLocation, bit);
            bitLocation--;
            if(bitLocation == -1){
                bitLocation = bits-1;
                byteLocation++;
            }
        }
    }
    //write a End of message at end of image
    std::string EndofFile = "\n--------------------END OF FILE--------------------\n\n";
    for(int i = 0; i<EndofFile.length(); i++){
        for(int j = 7; j>=0; j--){
            bool bit = bitRead(EndofFile[i], j);
            bitWrite(image[byteLocation], bitLocation, bit);
            bitLocation--;
            if(bitLocation == -1){
                bitLocation = bits-1;
                byteLocation++;
            }
        }
    }
    return;
    
}
std::vector<char> readEncodedImage(std::vector<u_int8_t> &image){
    int bitLocation = bits-1;
    int byteLocation = 0;
    std::vector<char> output;
    u_int8_t currentChar = 0x00;
    int counter = 0;
    for(int i = 0; i<image.size(); i++){
        for(int j = bitLocation; j>= 0; j--){
            bool bit = bitRead(image[i], j);
            bitWrite(currentChar, 7-counter, bit);
            counter++;
            if(counter == 8){
                output.push_back(currentChar);
                currentChar = 0x00;
                counter = 0;
                byteLocation++;
            }
        }
    }
    return output;
}
void outputConfig(){
    std::cout<<std::left<<std::setw(15)<<"pngName: "<<pngName<<std::endl;
    std::cout<<std::left<<std::setw(15)<<"fileName: "<<fileName<<std::endl;
    std::cout<<std::left<<std::setw(15)<<"outputName: "<<outputName<<std::endl;
    std::cout<<std::left<<std::setw(15)<<"width: "<<imageWidth<<std::endl;
    std::cout<<std::left<<std::setw(15)<<"height: "<<imageHeight<<std::endl;
    std::cout<<std::left<<std::setw(15)<<"bits: "<<bits<<std::endl;
    std::cout<<std::left<<std::setw(15)<<"mode: "<<mode<<std::endl;
}
int main(){
    readConfig();
    outputConfig();
    std::vector<u_int8_t> image;
    decompress(image, pngName);
    if(mode == 'E'){
        writeData(image);
        unsigned error = lodepng::encode(outputName, image, imageWidth, imageHeight);
        if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }
    else if(mode == 'X'){
        std::vector<char> output = readEncodedImage(image);
        std::ofstream outfile(outputName);
        for(int i = 0; i<output.size(); i++){
            outfile<<output[i];
        }
        outfile.close();
    }
}