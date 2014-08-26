#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using namespace std;

int main(int argc, const char *argv[])
{
    ofstream out;
    out.open("test.txt");
    if(!out)
        throw runtime_error("open");
    out << "This is an apple." << endl;
    long pos = out.tellp();
    out.seekp(pos - 7);
    out << "sam";
    out.close();
    ifstream in;
    in.open("test.txt");
    if(!in)
        throw runtime_error("open fail to read.");
    in.seekg(pos - 5);
    string read;
    in >> read;
    cout << read << endl;
    return 0;
}

