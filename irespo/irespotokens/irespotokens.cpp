/**
*  @file
*  @copyright defined in eos/LICENSE.txt
*/

#include "irespotokens.hpp"

namespace irespo {

	void irespotokens::create(account_name issuer,
		asset        maximum_supply,
		bool         transfer_locked)
	{
		require_auth(_self);

		auto sym = maximum_supply.symbol;
		eosio_assert(sym.is_valid(), "invalid symbol name");
		eosio_assert(maximum_supply.is_valid(), "invalid supply");
		eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

		stats statstable(_self, sym.name());
		auto existing = statstable.find(sym.name());
		eosio_assert(existing == statstable.end(), "token with symbol already exists");

		statstable.emplace(_self, [&](auto& s) {
			s.supply.symbol = maximum_supply.symbol;
			s.max_supply = maximum_supply;
			s.issuer = issuer;
			s.transfer_locked = transfer_locked;
		});
	}


	void irespotokens::issue(account_name to, asset quantity, string memo)
	{
		auto sym = quantity.symbol;
		eosio_assert(sym.is_valid(), "invalid symbol name");
		eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

		auto sym_name = sym.name();
		stats statstable(_self, sym_name);
		auto existing = statstable.find(sym_name);
		eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
		const auto& st = *existing;

		require_auth(st.issuer);
		eosio_assert(quantity.is_valid(), "invalid quantity");
		eosio_assert(quantity.amount > 0, "must issue positive quantity");

		eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
		eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

		statstable.modify(st, 0, [&](auto& s) {
			s.supply += quantity;
		});

		add_balance(st.issuer, quantity, st.issuer);

		if (to != st.issuer) {
			SEND_INLINE_ACTION(*this, transfer, { st.issuer,N(active) }, { st.issuer, to, quantity, memo });
		}
	}

	void irespotokens::unlock(asset unlock)
	{
		eosio_assert(unlock.symbol.is_valid(), "invalid symbol name");
		auto sym_name = unlock.symbol.name();
		stats statstable(_self, sym_name);
		auto token = statstable.find(sym_name);
		eosio_assert(token != statstable.end(), "token with symbol does not exist, create token before unlock");
		const auto& st = *token;
		require_auth(st.issuer);

		statstable.modify(st, 0, [&](auto& s) {
			s.transfer_locked = false;
		});
	}


	void irespotokens::transfer(account_name from,
		account_name to,
		asset        quantity,
		string       memo)
	{
		eosio_assert(from != to, "cannot transfer to self");
		require_auth(from);
		eosio_assert(is_account(to), "to account does not exist");
		auto sym = quantity.symbol.name();
		stats statstable(_self, sym);
		const auto& st = statstable.get(sym);

		if (st.transfer_locked && to != N(irespoicoico)) {
			eosio_assert(has_auth(st.issuer) || has_auth(N(irespoicoico)) || has_auth(N(irespoescrow))
				,"missing required authority of irespo issuer or ICO contract");
		}

		require_recipient(from);
		require_recipient(to);

		eosio_assert(quantity.is_valid(), "invalid quantity");
		eosio_assert(quantity.amount > 0, "must transfer positive quantity");
		eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
		eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");


		sub_balance(from, quantity);
		add_balance(to, quantity, from);
	}

	void irespotokens::sub_balance(account_name owner, asset value) {
		accounts from_acnts(_self, owner);

		const auto& from = from_acnts.get(value.symbol.name(), "no balance object found");
		eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");


		if (from.balance.amount == value.amount) {
			from_acnts.erase(from);
		}
		else {
			from_acnts.modify(from, owner, [&](auto& a) {
				a.balance -= value;
			});
		}
	}

	void irespotokens::add_balance(account_name owner, asset value, account_name ram_payer)
	{
		accounts to_acnts(_self, owner);
		auto to = to_acnts.find(value.symbol.name());
		if (to == to_acnts.end()) {
			to_acnts.emplace(ram_payer, [&](auto& a) {
				a.balance = value;
			});
		}
		else {
			to_acnts.modify(to, 0, [&](auto& a) {
				a.balance += value;
			});
		}
	}

} /// namespace irespo

EOSIO_ABI(irespo::irespotokens, (create)(issue)(transfer)(unlock))