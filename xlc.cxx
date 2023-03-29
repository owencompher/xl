// converts stdin as bfxl to bf to stdout
#include <iostream>
#include <string>
#include "convert.h"

using namespace std;

int
main()
{
    string bfx; char line[80];
    while(cin.getline(line, 80)) bfx += line;
    
    char controlChars[4] = {'#', '{', '\\', '}'};
    string bf = convert(bfx, controlChars);

    cout << bf << endl;
}
