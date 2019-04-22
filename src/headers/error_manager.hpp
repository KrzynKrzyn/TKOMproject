#ifndef ERROR_MANAGER_HPP_INCLUDED
#define ERROR_MANAGER_HPP_INCLUDED

#include <unordered_map>
#include <queue>
#include "error.hpp"

class ErrorManager
{
    private:
        std::queue<Error> noncritical_errors;
        static std::unordered_map<Error::Type, std::string, EnumClassHash> error_message;

        std::string getMessage(Error err) const;

    public:
        void handleError(Error err);
};

#endif // ERROR_MANAGER_HPP_INCLUDED
