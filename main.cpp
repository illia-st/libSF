#include <iostream>
#include "SF.h"

int main(){
    std::string file_name {};
    SF sf;
    while(true){
        std::cout << "Enter the file name which absolute path you want to know > ";
        std::getline(std::cin, file_name);
        if(file_name == "BREAK"){
            break;
        }
        std::cout << sf.FindPath(file_name) << std::endl;
    }

    return 0;
}