#include "irespoescrow.hpp"

namespace irespo {

	void irespoescrow::setapp(name application)
	{
		require_auth(_self);
		require_auth(application);

		eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
		configs(_self, _self).set(config{ application }, application);
	}

	void irespoescrow::withdraw(uint64_t from_user_id,
		name to_account,
		asset  quantity,
		string       memo)
	{
		
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");

		require_auth(_self);
		require_auth(configs(_self, _self).get().application);

		auto iter = escrows.find(from_user_id);
		eosio_assert(iter != escrows.end(), "No tokens to withraw");

		eosio_assert(quantity.is_valid(), "invalid quantity");
		eosio_assert(quantity.amount > 0, "must withraw positive quantity");
		eosio_assert(iter->quantity.amount >= quantity.amount, "withraw no more than owned by account");
		eosio_assert(quantity.symbol == iter->quantity.symbol, "symbol precision mismatch");
		eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

		escrows.modify(iter, configs(_self, _self).get().application, [&](auto& row) {
			row.quantity -= quantity;
		});

		action(permission_level{ _self, N(active) }, N(irespotokens), N(transfer),
			make_tuple(_self, to_account, quantity, string("irespoescrow to user account"))).send();
	}




} /// namespace irespo

