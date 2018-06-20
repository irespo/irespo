#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/singleton.hpp>

using namespace eosio;
using namespace std;

class irespoevents : public contract {
    using contract::contract;

public:
    irespoevents(account_name self) : contract(self),
     _events(_self, _self){}

    // @abi table
    struct event {
        uint64_t eventId;
        string eventHash;
		name author;

        uint64_t primary_key() const {return eventId; }

        EOSLIB_SERIALIZE(event, (eventId)(eventHash)(author))
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
    void addevent(uint64_t eventId
           , string eventHash
           , name author
    ){
        eosio_assert(configs(_self, _self).exists(), "Application account not configured");
        require_auth(configs(_self, _self).get().application);

        auto iter = _events.find(eventId);

        if(iter == _events.end()){

			_events.emplace(configs(_self, _self).get().application, [&](auto& row){
                row.eventId = eventId;
                row.eventHash = eventHash;
                row.author = author;
            });

            print("Event added");
        }
        else{
            print("Event already exists");
        }
    }

    // @abi action
    void deleteevent(uint64_t eventId){
        eosio_assert(configs(_self, _self).exists(), "Application account not configured");
        require_auth(configs(_self, _self).get().application);

        auto iter = _events.find(eventId);

        if(iter == _events.end()){
            print("No event to delete");
        }
        else{
			_events.erase(iter);
            print("Event deleted");
        }
    }

    multi_index<N(event), event> _events;
};

EOSIO_ABI(irespoevents, (setapp)(addevent)(deleteevent))