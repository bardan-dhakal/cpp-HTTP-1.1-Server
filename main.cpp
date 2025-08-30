#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>



std::string getLines(std::ifstream& inputfile)
{
    int bytes_to_read = 8;
    char byte_buffer ;
    std::string lines ;

    while (!inputfile.eof())
    {
        int i {0};        
        while ( i < bytes_to_read)
        {
            inputfile.get(byte_buffer);
            lines += byte_buffer;
            i ++;                
        }
    }
    return lines;
}

void extract_each_line(const std::string& lines)
{
    int prev_index {0};
    int length = lines.length();

    std::string line {};

    for (int curr_index {0}; curr_index < length; curr_index++)
    {

        if (lines[curr_index] == '\n' && curr_index != length - 1)
        {  
            
            line = lines.substr(prev_index, curr_index - prev_index + 1);
            std::cout << "read : " << line;

            prev_index = curr_index + 1;
        }
    }
} 


int main() {

    std::ifstream inputfile("messages.txt");

    if (!inputfile.is_open()) 
    {
        std::cout<< "File not opened" << std::endl;
        return 0;
    }

    std::cout<< "File opened" << std::endl;

    std::string lines = getLines(inputfile);


    extract_each_line(lines);

    return 0;
    
}