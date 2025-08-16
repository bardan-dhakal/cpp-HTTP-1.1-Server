#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>


int main() {

    std::cout << "Compile Test" << std::endl;
    std::ifstream inputfile("messages.txt");

    if (!inputfile.is_open()) 
    {
        std::cout<< "File not opened" << std::endl;
        return 0;
    }

    std::cout<< "File opened" << std::endl;

    int bytes_to_read = 8;
    char byte_buffer;
    std::string eight_byte_data ;


    while (!inputfile.eof())
    {
        for (int i = 0; i < bytes_to_read; i++)
        {
            inputfile.get(byte_buffer);
            eight_byte_data += byte_buffer;
        }

        std::cout<< "read: " << eight_byte_data << std::endl;

    }


    return 0;
    

}