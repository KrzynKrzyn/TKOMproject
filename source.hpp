#ifndef SOURCE_HPP_INCLUDED
#define SOURCE_HPP_INCLUDED

#include <fstream>
#include "error_manager.hpp"

class Source
{
    public:
        virtual bool isEOF() const = 0;

        virtual char getCharacter() const = 0;
        virtual char nextChar() = 0;

        virtual int getPosition() const = 0;
        virtual int getLine() const = 0;
};

class FileSource : public Source
{
    private:
        std::ifstream file;
        ErrorManager &error_manager;

        int current_line = 0;
        int current_position = -1;

        char character = '\0';

    public:
        bool isEOF() const { return file.eof(); }

        char getCharacter() const { return character; }
        char nextChar();

        int getPosition() const {return current_position; }
        int getLine() const {return current_line; }

        FileSource(std::string filename, ErrorManager &em): error_manager(em)
        {
            file.open(filename);
            if(!file.good()) error_manager.handleError(Error(Error::Type::File_non_existent, 0, 0));
        }

        ~FileSource() { file.close(); }

};

#endif // SOURCE_HPP_INCLUDED
