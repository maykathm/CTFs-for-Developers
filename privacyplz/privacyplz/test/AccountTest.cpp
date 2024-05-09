
#include <boost/test/unit_test.hpp>
#include "privacyplz/Account.hpp"


BOOST_AUTO_TEST_SUITE(account_test)

BOOST_AUTO_TEST_CASE(cannot_access_flag) {
    privacyplz::Account account;
    BOOST_CHECK(!account.canAccessFlag());
}

BOOST_AUTO_TEST_CASE(throws_exception_if_no_access) {
    privacyplz::Account account;
    auto check = [](const std::runtime_error& error){
        std::string message("Nope! No flag for you.");
        return message.find(error.what()) != std::string::npos;
    };
    BOOST_CHECK_EXCEPTION(account.getFlag(), std::runtime_error, check);
}

BOOST_AUTO_TEST_SUITE_END()