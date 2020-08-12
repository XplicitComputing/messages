#include <cmath> 
#include <fstream>
#include <iostream>
#include <string>
#include <vector>



std::string filename(std::string path)
{
    return path.substr(0, path.find_first_of("."));
}
