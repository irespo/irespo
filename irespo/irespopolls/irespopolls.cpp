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

    struct option {
        uint64_t optionId;
        string optionText;
        uint32_t numberOfVotes;

        EOSLIB_SERIALIZE(option , (optionId)(optionText)(numberOfVotes))
    };

    // @abi table
    struct pollresult {
        uint64_t questionId;
        string questionText;
        uint64_t eventId;
        string eventName;
        uint8_t  isEventPasswordProtected;
        uint8_t  isLoggedUserRequired;
        uint8_t  isEOSUserRequired;
        uint64_t startDateTimeUTC;
        uint64_t endDateTimeUTC;
        vector<option> options;

        uint64_t primary_key() const {return questionId; }

        EOSLIB_SERIALIZE( pollresult, (questionId)(questionText)(eventId)(eventName)(isEventPasswordProtected)(isLoggedUserRequired)(isEOSUserRequired)(startDateTimeUTC)(endDateTimeUTC)(options))
    };

    struct config {
        name application;
        EOSLIB_SERIALIZE(config, (application))
    };

    typedef singleton<N(config), config> configs;

    // @abi action
    void setapp(name application){
        require_auth(_self);
        require_auth(application);

        eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
        configs(_self,_self).set(config{application}, application);
    }

    // @abi action
    void addpoll(uint64_t questionId
           , string questionText
           , uint64_t eventId
           , string eventName
           , uint8_t  isEventPasswordProtected
           , uint8_t  isLoggedUserRequired
           , uint8_t  isEOSUserRequired
           , uint64_t startDateTimeUTC
           , uint64_t endDateTimeUTC
           , vector<option> options
    ){
        eosio_assert(configs(_self, _self).exists(), "Application account not configured");
        require_auth(configs(_self, _self).get().application);

        auto iter = _pollresults.find(questionId);

        if(iter == _pollresults.end()){

            _pollresults.emplace(configs(_self, _self).get().application, [&](auto& row){
                row.questionId = questionId;
                row.questionText = questionText;
                row.eventId = eventId;
                row.eventName = eventName;
                row.isEventPasswordProtected = isEventPasswordProtected;
                row.isLoggedUserRequired = isLoggedUserRequired;
                row.isEOSUserRequired = isEOSUserRequired;
                row.startDateTimeUTC = startDateTimeUTC;
                row.endDateTimeUTC = endDateTimeUTC;
                row.options = options;
            });

            print("Poll added");
        }
        else{
            print("Poll already exists");
        }
    }

    // @abi action
    void deletepoll(uint64_t questionId){
        eosio_assert(configs(_self, _self).exists(), "Application account not configured");
        require_auth(configs(_self, _self).get().application);

        auto iter = _pollresults.find(questionId);

        if(iter == _pollresults.end()){
            print("No poll to delete");
        }
        else{
            _pollresults.erase(iter);
            print("Poll deleted");
        }
    }

    multi_index<N(pollresult), pollresult> _pollresults;
};

EOSIO_ABI( irespopolls, (setapp)(addpoll)(deletepoll))