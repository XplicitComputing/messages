# Messages

What is Messages? 

Messages (libxcmessages) is a schema designed for engineers and scientists that enables numerical and related data to be shared across high-performance computing sessions and programming languages. It requires a <a href=https://developers.google.com/protocol-buffers/docs/downloads>protoc 3 compiler</a>.

A message is defined to be some transmit-able idea, useful for a specific context (though actually quite flexible given how multiple messages can interact). Messages are a collection of 18 practical concepts for computer-aided engineering. It could be coalesced data for a particular property, the description of a physical model as its algorithms, the specific setup of a system, etc.. Ideally, each message concept is characteristic and orthogonal to other messages, constructing a basis set with a few practical message types (to-be-defined and learned). Messages permit embedding sub-message members, which makes complex obect-oriented concepts possible. In other cases, vectorized data takes advantage of packed repeated fields to permit parallel operations and direct copying. In order to communicate across a spectrum of programming languages/environments, the layout and names (and types) of the known data fields must be defined and convention maintained to promote high-performance interoperability. This schema leverages machine-generated code as the accessors (get/set functions) for integrated engineering environments such as xcompute-server and xcompute-client.

## 0. getting started


18 useful messages are distributed into four proto files:
- `vector.proto` - numeric arrays using packed arena allocation (compatible with xco files)

- `spatial.proto` - elements and regions for a geometry (compatible with xcg files) 

- `system.proto` - domain setup, models, parameters, associations (compatible with xcs files)  

- `meta.proto` - meta-data and user-graphics for a specific system (compatible with xcm files) 


These definitions are then fed into a protoc compiler to generate a linkable static library and bindings for the following languages: C++, Objective-C, C#, PHP, Python, Java, Javascript, Ruby, Dart, and Go. 


### 1. installing dependencies

Try downloading and installing Google Protobuf 3 with the script:
```
sudo sh install/install-protobuf-online.sh
```
or alternately, build from provided static sources:
```
sudo sh install/install-protobuf-offline.sh
```

For special installation, see: https://github.com/protocolbuffers/protobuf/blob/master/src/README.md
. If compiling manually, run `./autogen.sh`  and set the prefix path to `/usr/local` or desired location: 
```
./configure --prefix=/usr/local/xcompute
```
Then follow with the standard: `make`, `make check`, `sudo make install`, `sudo ldconfig`

### 2. checking dependencies

Protobuf3 should have been installed in the previous step. Assure protocol buffer compiler has installed and is version 3+:
```
protoc --version
```
And check path:
```
which protoc
```
There should be a corresponding set of `libprotobuf` in `/usr/local/lib/` and headers in `/usr/local/include/` , or wherever you specified as the installation prefix.

### 3. generating bindings

Invoke the protoc compiler for the desired output language (by defining the name of the output folder following equals sign) against proto files to yield libmessages.a and bindings. For instance, to generate only c++ accessors for vector.proto:

```
protoc --cpp_out=cpp vector.proto 
```
Header and source files with `*.pb.h` and `*.pb.cc` extensions are generated (to be included in your larger project).

Multiple languages and proto files can be compiled at once, like so:

```
protoc --cpp_out=cpp --csharp_out=csharp –objc_out=objc --ruby_out=ruby --python_out=python --java_out=java --js_out=javascript vector.proto system.proto spatial.proto meta.proto
```

### 4. basic save & load

In order to save or transmit, a program’s associative data structures must be serialized into a format that permits representation in contiguous memory and/or storage. For instance, several built-in serialization functions are available for C++ including those that leverage a std output file stream:

```#include "vector.pb.h" ```

```
std::ofstream outfile(path); // create an output file stream with given path
msg.SerializeToOstream(&outfile); // serialize a prepared Messages::Vector64
outfile.close(); // finish the file and release resource
```
Serialized data can then be transmit over file-and-wire to another computer session, where it is then de-serialized (parsed) using the conjugate pair load function such as std input file stream:
```
std::ifstream infile(path); // create an input file stream
Messages::Vector64 msg; // create an empty message container
msg.ParseFromIStream(&infile); // de-serialize binary file to fill message
infile.close(); // finish the file and release resource
```
Please see `README.pdf` for thorough introduction and examples.

### 5. license and fair-use agreement

These four proto files (and generated library/bindings) are licensed under a Creative Commons Attribution-NoDerivatives 4.0 International License. You are free to use these components for personal, academic, commercial, and/or research use. No warranty is implied nor provided unless otherwise stated in a separate engineering support agreement. As part of the license agreement, this document must remain alongside any provided *.proto definitions and not be altered in any way. 
