#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/singleton.hpp>

using namespace eosio;
using namespace std;

class irespopolls : public contract {
    using contract::contract;

public:
    irespopolls (account_name self) : contract(self),
     _pollresults(_self, _self){}

    // @abi action
    void setapp(account_name application){
        require_auth(_self);
        require_auth(application);

        eosio_assert(!configs::exists(), "Configuration already exists");
        configs::set(config{application});
    }
    // @abi action
    void addpoll(account_name application){
        eosio_assert(configs::exists(), "Application account not configured");
        require_auth(configs::get().application);

    }

private:
    struct config {
        account_name application;
        EOSLIB_SERIALIZE(config, (application))
    };

    typedef singleton<N(irespopolls), N(config), N(irespopolls), config> configs;
    // @abi table
    struct pollresults {
        uint64_t eventId;
        string eventName;

        uint64_t questionId;
        string questionText;

        uint8_t  isEventPasswordProtected;
        uint8_t  isLoggedUserRequired;
        uint8_t  isEOSUserRequired;

        uint64_t startDateTimeUTC;
        uint64_t endDateTimeUTC;

        uint64_t primary_key() const {return questionId; }

        EOSLIB_SERIALIZE( pollresults, (eventId)(eventName)(questionId)(questionText)(isEventPasswordProtected)(isLoggedUserRequired)(isEOSUserRequired))
    };

    multi_index<N(pollresults), pollresults> _pollresults;
};

EOSIO_ABI( irespopolls, (setapp)(addpoll))