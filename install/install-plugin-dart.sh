sudo apt-add-repository ppa:hachre/dart
sudo apt-get update
sudo apt-get install darteditor dartsdk dartium

git clone https://github.com/dart-lang/protobuf.git
cd protobuf/protoc_plugin
sudo make check
sudo make -j4
sudo make install


