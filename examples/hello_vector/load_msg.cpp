#include "vector.pb.h"

#include <cmath> 
#include <fstream>
#include <iostream>
#include <string>
#include <vector>




std::string name{ "test.xco" };                                               // file name



bool load(Messages::Vector64& msg)
{
    // SAVE protobuf msg


    // 2a. open a file stream
    std::ifstream infile(name);
    if (!infile.is_open())
    {
        // handle errors

        std::cerr << "error opening file: " << name << std::endl;
        std::cout << strerror(errno) << std::endl;

        return false;
    }
    std::cout << "opened file: " << name << std::endl;

    // 2b. bytes are streamed from protobuf into the output file
    msg.ParseFromIstream(&infile);

    // 2c. close the file when done
    infile.close();

    return true;
}



int main(int argc, char** argv)
{
    if (argc > 1)//can change name by passing in
    {
        name = argv[1];
    }
    std::cout << "launching load app for: " << name << "..." << std::endl;

    // 1. create a new protobuf message (see vector.proto for what's available):
    Messages::Vector64 msg;

    // 2. when ready to save or transmit
    load(msg);

    // 3. print some stats to show we've actually reconstructed it
    std::cout << "data name: " << msg.name() << std::endl;
    std::cout << "components: " << msg.components() << std::endl;
    std::cout << "length: " << msg.values_size() / std::max(1,msg.components()) << std::endl;
    std::cout << "size: " << msg.values_size() << std::endl;
    std::cout << "bytes: " << msg.values_size() * sizeof(double) << std::endl;

    // 4. do something else with it, like load into a vector and compute...
    std::cout << "closing load app." << std::endl;

    // message should be stored on file in this directory

    return 0;
}
