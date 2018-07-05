#include "irespooracle.hpp"

namespace irespo {

	void irespooracle::setapp(name application)
	{
		require_auth(_self);
		require_auth(application);

		eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
		configs(_self, _self).set(config{ application }, application);
	}

	void irespooracle::addoracle(uint64_t id
		, string type
		, uint64_t value
		, uint64_t update_time
		, string description) {

		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		//oracles oracletable(_self, _self);

		auto iter = oracles.find(id);

		if (iter == oracles.end()) {

			oracles.emplace(configs(_self, _self).get().application, [&](auto& row) {
				row.id = id;
				row.type = type;
				row.value = value;
				row.update_time = update_time;
				row.description = description;
			});

			print(type, " added with value ", value, " at time ", update_time);
		}
		else {
			oracles.modify(iter, configs(_self, _self).get().application, [&](auto& row) {
				row.type = type;
				row.value = value;
				row.update_time = update_time;
				row.description = description;
			});

			print(type, " updated with value ", value, " at time ", update_time);
		}
	}


	void irespooracle::deloracle(uint64_t id) {
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		//oracles oracletable(_self, _self);

		auto iter = oracles.find(id);

		if (iter == oracles.end()) {
			print("No oracle to delete");
		}
		else {
			oracles.erase(iter);
			print(iter -> type, " deleted ");
		}
	}


} /// namespace irespo

