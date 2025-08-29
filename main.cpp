#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>


int main() {

    std::ifstream inputfile("messages.txt");

    if (!inputfile.is_open()) 
    {
        std::cout<< "File not opened" << std::endl;
        return 0;
    }

    std::cout<< "File opened" << std::endl;

    int bytes_to_read = 8;
    char byte_buffer ;
    std::string byte_data ;

    int prev_index {0};
    int curr_index;


    while (!inputfile.eof())
    {
        int i {0};

        
        while ( i < bytes_to_read)
        {
            inputfile.get(byte_buffer);
            byte_data += byte_buffer;

            if (byte_buffer == '\n')
            {
                prev_index = curr_index;
                curr_index = byte_data.length();

                std::cout << byte_data.substr(prev_index, curr_index - prev_index) << std::endl;
            }
            
            i ++;        
            
        }

    }

    std::cout<< "read: " << byte_data << " - end of read." << std::endl;  

    return 0;
    
}