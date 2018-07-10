#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>

#include <string>

using namespace eosio;

namespace irespo {

	using std::string;

	class irespoescrow : public contract {
	public:
		irespoescrow(account_name self) :contract(self), escrows(_self, _self) {}

		// @abi action
		void setapp(name application);

		// @abi action
		void withdraw(uint64_t from_user_id,
			name to_account,
			asset  quantity,
			string       memo);

		void apply(const account_name contract, const account_name act);

		void transferReceived(const currency::transfer &transfer, const account_name code);

		// @abi table
		struct escrow {
			uint64_t user_id;
			asset    quantity;

			uint64_t primary_key() const { return user_id; };
		};

		struct config {
			name application;
		};

		multi_index<N(escrow), escrow> escrows;
		typedef singleton<N(config), config> configs;

	};
	
} /// namespace irespo

