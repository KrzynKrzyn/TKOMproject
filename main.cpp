#include <iostream>

#include "lexer.hpp"

using namespace std;

int main()
{
    FileSource blob("example.txt");

    for(int i=0;i<25;++i)
    {
        cout << blob.nextChar();
        cout << '\t' << blob.getLine() << ":" << blob.getPosition() << endl;
    }


    return 0;
}
