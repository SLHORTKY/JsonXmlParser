#include "Parser.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main(){
    std::string filepath = ""; // example code 
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file" << std::endl;
        return 1; 
    }

    std::string line;
    std::string jsonString;
    
    // Read the file line by line
    while (std::getline(file, line)) {
        jsonString += line;
    }
    file.close();
    
    Parser parser;
    Xml::Object* obj = parser.ParseXml(jsonString);
    std::cout <<obj->toXmlString() << std::endl;
    
    return 0;
}