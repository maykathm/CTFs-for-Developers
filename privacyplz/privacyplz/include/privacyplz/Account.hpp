#ifndef PRIVACYPLZ_ACCOUNT_HPP
#define PRIVACYPLZ_ACCOUNT_HPP

#include <string>
#include <memory>
#include "Access.hpp"

namespace privacyplz {
    class Account {
    public:
        Account();
        [[nodiscard]] std::string getFlag();
        [[nodiscard]] bool canAccessFlag();
    private:
        Access access;
    };
}

#endif