#include <iostream>
#include <string>

using namespace std;

int main(int argc, const char *argv[])
{
    string str = "<content>Hello, world, I am Zhang Shibo.</content>";
    string::size_type pos1, pos2;
    pos1 = str.find("<content>") + 9;
    pos2 = str.find("</content>");
    string sub = str.substr(pos1, pos2 - pos1);
    cout << sub << endl;
    return 0;
}
