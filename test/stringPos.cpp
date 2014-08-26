#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>

using namespace std;

int main(int argc, const char *argv[])
{
    ifstream in;
    in.open("../data/library/C11-Space/C11-Space0256.txt");
    if(!in)
        throw runtime_error("open");
    string line;
    string content;
    getline(in, line);
    cout << line << endl;
    cout << line.size() << endl;
    size_t pos = line.find("工程");
    if(pos != string::npos)
        cout << pos << endl;
    size_t pos1 = line.find("\n");
    if(pos1 == string::npos)
        cout << pos1 << endl;
    return 0;
}
