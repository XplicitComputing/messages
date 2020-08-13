#include "vector.pb.h"

#include <cmath> 
#include <ctime>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>




std::string name{ "sweep.xco" };        // file name
std::size_t N{ 10000 };                 // length of output (sample count)
double w_max = 100.0;                   // w_max frequency end of sweep
unsigned int C = 2;                     // number of channels to output



// prints current date/time in string
std::string
current_time()
{
    std::time_t rawtime;
    std::time (&rawtime);
    std::string txt = std::asctime(std::localtime(&rawtime));

    // remove invalid last character
    txt.pop_back();
    return txt;
}


// FILL DATA -- in this case we've added some example sine sweep functions
void fill_data(Messages::Vector64& msg)
{
    // CREATE DATA and fill protobuf msg
    std::cout << "filling data..." << std::flush;

    // 2a. we're going to write directly into the msg buffer
    msg.set_name(name);

    // set members -- again look at vector.proto definition
    msg.set_components(C);                                                                          // other properties such as position have 3 for xyz

    // for non-trivial data structures, get a mutable version of the protobuf
    auto& buffer_vals = *msg.mutable_values();                                                      // returns a pointer to the entry

    // allocate the protobuf vector ahead of time (if not adding one at a time)
    buffer_vals.Resize(C * N, 0.0);                                                                 // N elements, initialized to value 0.0

#pragma omp parallel for
    for (auto n = 0; n < N; n++)
    {
        auto t = double(n) / double(N - 1);                                                         // parameterize t=0:1
        buffer_vals[C * n] = std::sin(w_max * t * t);                                               // chanenl 0
        buffer_vals[C * n + 1] = std::cos(w_max * t * t);                                           // channel 1

        // add more if desired (change C)
    }

    // 2b. one could load up more content in the messages...
    msg.set_comment("w_max=" + std::to_string(w_max) + " N=" + std::to_string(N) + " C=" + std::to_string(C) + " " + current_time());
    std::cout << "done." << std::endl;
}



// SAVE protobuf msg to file
bool save(Messages::Vector64& msg)
{
    std::cout << "saving: " << name << "..." << std::flush;
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
    std::cout << "done." << std::endl;
    return true;
}



// MAIN function - program starts here 
int main(int argc, char** argv)
{
    std::cout << "launching xco save app...(sweep generator)" << std::endl;
    
    for (auto a = 1; a < argc; a++)
    {
        std::string arg(argv[a]);
        if (arg.size() < 2)
        {
            break;
        }
        if (argv[a][0] == '-')                                      // -X
        {
            if (argv[a][1] == 'w')
            {
                w_max = std::stod(arg.substr(2,arg.size()));
            }
            else if (argv[a][1] == 'N')
            {
                N = std::stoul(arg.substr(2,arg.size()));
            }
            else if (argv[a][1] == '-')                             // --name
            {
                name = arg;
            }
            break;
        }
        
    }

    // 1. create a new protobuf message (see vector.proto for what's available):
    Messages::Vector64 msg;

    // 2. fill buffer with data... in this case, sine sweeps
    fill_data(msg);

    // 3. when ready to save or transmit
    save(msg);
    std::cout << "closing xco save app." << std::endl;

    // message should be stored on file in this directory

    return 0;
}
