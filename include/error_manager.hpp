#ifndef ERROR_MANAGER_HPP_INCLUDED
#define ERROR_MANAGER_HPP_INCLUDED

#include <unordered_map>
#include <deque>
#include <vector>
#include "error.hpp"

class ErrorManager
{
    private:
        std::deque<Error> noncritical_errors;
        static std::unordered_map<Error::Type, std::string, EnumClassHash> error_message;

        std::string getSeverity(Error err) const;
        std::string getMessage(Error err) const;
        std::string getInfo(Error err) const;

    public:
        void handleError(Error err);
        std::vector<std::string> getWarnings();
};

#endif // ERROR_MANAGER_HPP_INCLUDED
