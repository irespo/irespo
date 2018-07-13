#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

using namespace eosio;

namespace irespo {

	using std::string;

	class irespoicoico : public contract {
	public:
		irespoicoico(account_name self) :contract(self), allowedusers(_self, _self) {}

		// @abi action
		void setapp(name application);

		// @abi action
		void addauser(name user);

		// @abi action
		void addausers(vector<name> users);

		// @abi action
		void delauser(name user);

		// @abi action
		void delall(name application);

		struct config {
			name application;
		};

		typedef singleton<N(config), config> configs;

		// @abi table
		struct alloweduser {
			name user;

			name primary_key() const { return user; }

			EOSLIB_SERIALIZE(alloweduser, (user))
		};

		multi_index<N(alloweduser), alloweduser> allowedusers;
	}; /// namespace irespo
}

EOSIO_ABI(irespo::irespoicoico, (setapp)(addauser)(addausers)(delauser)(delall))