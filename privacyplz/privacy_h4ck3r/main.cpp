#include <privacyplz/Account.hpp>
#include <privacyplz/Access.hpp>
#include <iostream>

int main() {
    privacyplz::Account account;
    account.access = privacyplz::Access::ALL;
    std::cout << account.getFlag() << std::endl;
    return 0;
}