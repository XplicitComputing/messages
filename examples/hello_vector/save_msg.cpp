#include "vector.pb.h"

#include <cmath> 
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>




std::string name{ "sine-sweep.xco" };                                                               // file name
std::size_t N{ 1000000 };                                                                           // 1M 64-bit floating-points (8 MB) -- feel free to change



void fill_data(Messages::Vector64& msg)
{
    // CREATE DATA and fill protobuf msg


    // 2a. we're going to write directly into the msg buffer
    msg.set_name("my test data");

    // set members -- again look at vector.proto definition
    msg.set_components(2);                                                                          // other properties such as position have 3 for xyz

    // for non-trivial data structures, get a mutable version of the protobuf
    auto& buffer_vals = *msg.mutable_values();                                                      // returns a pointer to the entry

    // allocate the protobuf vector ahead of time (if not adding one at a time)
    buffer_vals.Resize(2 * N, 0.0);                                                                 // N elements, initialized to value 0.0

#pragma omp parallel for
    for (auto n = 0; n < N; n++)
    {
        auto t = double(n) / double(N - 1);                                                         // some random function
        buffer_vals[2 * n] = std::sin(100.0 * t * t);                                               // this is just some pretend input data
        buffer_vals[2 * n + 1] = std::cos(200.0 * t * t);                                           // a pretend second component
    }

    // 2b. one would load up more content in the messages...
}



bool save(Messages::Vector64& msg)
{
    // SAVE protobuf msg


    // 3a. open an output file stream
    std::ofstream outfile(name);
    if (!outfile.is_open())
    {
        // handle errors

        std::cerr << "error saving file: " << name << std::endl;
        std::cout << std::strerror(errno) << std::endl;

        return false;
    }

    // 3. print some stats to show we've actually reconstructed it
    std::cout << "data name: " << msg.name() << std::endl;
    std::cout << "components: " << msg.components() << std::endl;
    std::cout << "length: " << msg.values_size() / msg.components() << std::endl;
    std::cout << "size: " << msg.values_size() << std::endl;
    std::cout << "saved " << msg.values_size() * sizeof(double) << " bytes to: " << name << std::endl;

    // 3b. bytes are streamed from protobuf into the output file
    msg.SerializeToOstream(&outfile);

    // 3c. close the file when done
    outfile.close();

    return true;
}



int main(int argc, char** argv)
{
    if (argc > 1)//can change name by passing in
    {
        name = argv[1];
    }

    std::cout << "launching save app for: " << name << "..." << std::endl;

    // 1. create a new protobuf message (see vector.proto for what's available):
    Messages::Vector64 msg;

    // 2. fill buffer with data
    fill_data(msg);

    // 3. when ready to save or transmit
    save(msg);

    std::cout << "closing save app." << std::endl;

    // message should be stored on file in this directory

    return 0;
}
