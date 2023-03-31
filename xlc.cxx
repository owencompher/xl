// converts stdin as bfxl to bf to stdout
#include <iostream>
#include <cstring>
#include <string>
#include "convert.h"

using namespace std;

int
main(int argc, char* argv[])
{
    char cc[4];
    if (argc>1 && strlen(argv[1])>4) strncpy(cc, argv[1], 4);
    else strncpy(cc, "#{\\}", 4);
    
    string in ( (istreambuf_iterator<char>(cin)),
                (istreambuf_iterator<char>()));
                
    cout << convert(in, cc) << endl;
}
