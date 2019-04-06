#ifndef ERROR_MANAGER_HPP_INCLUDED
#define ERROR_MANAGER_HPP_INCLUDED

#include <queue>
#include <unordered_map>
#include "error.hpp"

class ErrorManager
{
    private:
        std::queue<Error> errors;
        static std::unordered_map<Error::Type, std::string, EnumClassHash> error_message;

    public:
        void handleError(Error err)
        {
            throw std::runtime_error("Line: " + std::to_string(err.getLine()) +
                                     "\tPosition: " + std::to_string(err.getPosition()) +
                                     "\tError: " + error_message.find(err.getType())->second);
        };
};

#endif // ERROR_MANAGER_HPP_INCLUDED
