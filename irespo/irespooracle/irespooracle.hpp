#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

using namespace eosio;

namespace irespo {

	using std::string;

	class irespooracle : public contract {
	public:
		irespooracle(account_name self) :contract(self) {}

		// @abi action
		void setapp(name application);

		// @abi action
		void addoracle(uint64_t id
			, string type
			, uint64_t value
			, string description
			, uint64_t update_time);

		// @abi action
		void deloracle(uint64_t id);


	private:
		// @abi table
		struct oracle {
			uint64_t id;
			string type;
			uint64_t value;
			string description;
			uint64_t update_time;

			uint64_t primary_key() const { return id; };
		};

		struct config {
			name application;
		};

		typedef multi_index<N(oracles), oracle> oracles;
		typedef singleton<N(config), config> configs;

	};


}; /// namespace irespo

EOSIO_ABI(irespo::irespooracle, (setapp)(addoracle)(deloracle))