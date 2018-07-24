#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>
#include <string>

using namespace eosio;

namespace irespo {

	using std::string;

	class irespoicoico : public contract {
	public:
		irespoicoico(account_name self) :contract(self), allowedicos(_self, _self) {}

		// @abi table
		struct allowedico {
			uint64_t ico_id;
			name user;

			uint64_t primary_key() const { return ico_id; }

			EOSLIB_SERIALIZE(allowedico, (ico_id)(user))
		};

		// @abi table
		struct log {
			uint64_t logtime;
			name irespooracle;
			uint64_t exchangerate;
			uint32_t icostarttime;
			uint32_t icoendtime;

		    uint64_t primary_key() const { return logtime; }

			EOSLIB_SERIALIZE(log, (logtime)(irespooracle)(exchangerate)(icostarttime)(icoendtime))
		};

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

		struct icoconfig {
			name irespooracle;
			uint32_t icostarttime;
			uint32_t icoendtime;
		};

		// @abi action
		void setapp(name application);

		// @abi action
		void setico(name irespooracle, uint32_t icostarttime, uint32_t icoendtime);

		// @abi action
		void addauser(uint64_t ico_id, name user);

		// @abi action
		void addausers(vector<allowedico> users);

		// @abi action
		void delauser(uint64_t ico_id);

		// @abi action
		void delall(name application);
		
		// @abi action
		void logdata(name application);

		// @abi action
		void dellogdata(name application);			

		void apply(const account_name contract, const account_name act);

		void transferReceived(const currency::transfer &transfer, const account_name code);


		typedef singleton<N(config), config> configs;
		typedef singleton<N(icoconfig), icoconfig> icoconfigs;

		multi_index<N(allowedico), allowedico> allowedicos;

		typedef multi_index<N(oracle), oracle> oracles;
		typedef multi_index<N(log), log> logs;

	}; /// namespace irespo
}

EOSIO_ABI(irespo::irespoicoico, (setapp)(setico)(addauser)(addausers)(delauser)(delall)(logdata)(dellogdata))