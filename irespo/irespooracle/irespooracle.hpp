#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

using namespace eosio;

namespace irespo {

	using std::string;

	class irespooracle : public contract {
	public:
		irespooracle(account_name self) :contract(self), oracles(_self, _self) {}

		// @abi action
		void setapp(name application);

		// @abi action
		void addoracle(uint64_t id
			, string type
			, uint64_t value
			, uint64_t update_time
			, string description);

		// @abi action
		void deloracle(uint64_t id);


	//private:
		// @abi table
		struct oracle {
			uint64_t id;
			string type;
			uint64_t value;
			uint64_t update_time;
			string description;

			uint64_t primary_key() const { return id; };
		};

		struct config {
			name application;
		};

		multi_index<N(oracle), oracle> oracles;
		typedef singleton<N(config), config> configs;

	};
	
}; /// namespace irespo

EOSIO_ABI(irespo::irespooracle, (setapp)(addoracle)(deloracle))