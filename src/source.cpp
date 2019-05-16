#include "source.hpp"

char FileSource::nextChar()
{
    if(file.eof()) return character = std::char_traits<char>::eof();//EOF;

    if(character == '\n')
    {
        ++current_line;
        current_position = 0;
    }
    else ++current_position;

    file.get(character);

    return character;
}
