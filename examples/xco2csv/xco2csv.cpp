#include "xco2csv.hpp"
#include "vector.pb.h"


std::string name{ "test.xco" };                                                 // default file name (if none provided)


bool
load(Messages::Vector64& msg)
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



void
exportCSV(Messages::Vector64& msg, std::string filepath)
{
    std::cout << "exporting to: " << filepath << std::endl;
    
    if (filepath.empty())
    {
        std::cerr << "Export CSV error: invalid file path" << std::endl;
        return;
    }

    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (!file.is_open())
    {
        std::cerr << "Export CSV error: File not opened: " << filepath << std::endl;
        return;
    }
   
    auto C = std::max(1, msg.components());
    for (int column = 0; column < C; column++)
    {
        if (column != 0)
        {
            file << ",";
        }
        if (C > 1)
        {
            file << msg.name() << " [" << std::to_string(column) << "]";
        }
        else
        {
            file << msg.name();
        }
    }
    file << std::endl;

    auto L = msg.values_size() / C;
    for (int row = 0; row < L; row++)
    {
        for (int column = 0; column < C; column++)
        {
            if (column != 0)
            {
                file << ",";
            }
            file << std::to_string(msg.values(row*C + column));
        }
        file << std::endl;
    }
    file.close();
}



int main(int argc, char** argv)
{
    std::cout << "launching xco2csv app for: " << name << "..." << std::endl;

    //mkdir("csv");
    
    for (auto a = 1; a < argc; a++)
    {
        name = argv[a];
        std::cout << "converting: " << name << "..." << std::endl;

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
        exportCSV(msg, filename(name) + ".csv");
    }

    std::cout << "closing xco_to_csv app." << std::endl;

    return 0;
}
