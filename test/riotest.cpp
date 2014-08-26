#include "Rio.h"
#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>

using namespace std;

int main(int argc, const char *argv[])
{
    int fd_read = open("../data/123/C36-Medical020.txt", O_RDONLY);
    if(fd_read == -1)
        throw runtime_error("open file failed.");
    Rio rio(fd_read);
    char buffer[1024];
    rio.readLine(buffer, 1024);
    cout << buffer << endl;
    cout << strlen(buffer) << endl;
    return 0;
}
