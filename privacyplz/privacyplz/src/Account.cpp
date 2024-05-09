#include "privacyplz/Account.hpp"
#include <filesystem>
#include <fstream>

namespace privacyplz {

    Account::Account() : access(Access::NONE){}

    std::string Account::getFlag() {
        if (!canAccessFlag()) {
            throw std::runtime_error("Nope! No flag for you.");
        }
        std::filesystem::path roottxt("/root/root.txt");
        if (!std::filesystem::exists(roottxt)) {
            throw std::runtime_error("The file /root/root.txt is missing or not accessible");
        }
        std::ifstream file(roottxt);
        if (!file.is_open()) {
            throw std::runtime_error("The file /root/root.txt is missing or not accessible");
        }
        std::string flag;
        getline(file, flag);
        return flag;
    }

    bool Account::canAccessFlag() {
        return access == Access::ALL;
    }

}