#include <iostream>
#include <fstream>
#include <vector>
#include <cmath> 

#include "vector.pb.h"

using namespace std;

string name{"sine-sweep.xco"}; // file name


//SAVE protobuf msg
bool load(Messages::Vector64& msg){
	//2a. open a file stream
	ifstream infile(name);
	if (!infile.is_open()){//handle errors
		cerr << "error opening file: " << name << endl;
		cout << strerror(errno) << endl;
		return false;
	}
	cout << "opened file: " << name << endl;
	//2b. bytes are streamed from protobuf into the output file
	msg.ParseFromIstream(&infile);
	//2c. close the file when done
	infile.close();
	return true;
}


//MAIN FUNCTION
int main(int argc, char** argv){
	if (argc > 1)//can change name by passing in
		name = argv[1];
    cout << "launching load app for: " << name << "..." << endl;
	//1. create a new protobuf message (see vector.proto for what's available):
	Messages::Vector64 msg;

	//2. when ready to save or transmit
	load(msg);

	//3. print some stats to show we've actually reconstructed it
	cout << "data name: " << msg.name() << endl;
	cout << "components: " << msg.components() << endl;
	cout << "length: " << msg.values_size()/std::max(1,msg.components()) << endl;
	cout << "size: " << msg.values_size() << endl;
	cout << "bytes: " << msg.values_size()*sizeof(double) << endl;
	//4. do something else with it, like load into a vector and compute...
	cout << "closing load app." << endl;
	//message should be stored on file in this directory
    return 0;
}
