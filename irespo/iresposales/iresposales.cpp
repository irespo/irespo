#include "iresposales.hpp"

namespace irespo {

	void iresposales::setapp(name application)
	{
		require_auth(_self);
		require_auth(application);

		eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
		configs(_self, _self).set(config{ application }, application);
	}

	void iresposales::setsales(name irespooracle, uint32_t salesstart, uint32_t salesend)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		salesconfigs(_self, _self).set(salesconfig{irespooracle,  salesstart,  salesend }, configs(_self, _self).get().application);
	}

	void iresposales::addauser(uint64_t ico_id, name user)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured.");
		require_auth(configs(_self, _self).get().application);

		auto iter = allowedicos.find(ico_id);

		if (iter == allowedicos.end()) {

			allowedicos.emplace(configs(_self, _self).get().application, [&](auto& row) {
				row.ico_id = ico_id;
				row.user = user;
			});
		}
		else{
			allowedicos.modify(iter, configs(_self, _self).get().application, [&](auto& row) {
				row.user = user;
			});
		}
	}

	void iresposales::addausers(vector<allowedico> users)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured.");
		require_auth(configs(_self, _self).get().application);

		for (auto& n : users)
		{
			auto iter = allowedicos.find(n.ico_id);

			if (iter == allowedicos.end()) {

				allowedicos.emplace(configs(_self, _self).get().application, [&](auto& row) {
					row.ico_id = n.ico_id;
					row.user = n.user;
				});
			}
			else
			{
				allowedicos.modify(iter, configs(_self, _self).get().application, [&](auto& row) {
					row.user = n.user;
				});
			}
		}	
	}

	void iresposales::delauser(uint64_t ico_id)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		auto iter = allowedicos.find(ico_id);

		if (iter != allowedicos.end()) {
			allowedicos.erase(iter);
		}
	}

	void iresposales::delall(name application)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		auto iter = allowedicos.begin();

		while (iter != allowedicos.end())
		{
			iter = allowedicos.erase(iter);
		}
	}

	void iresposales::logdata(name application)
	{
		uint64_t oracle_id = 1;

		eosio_assert(configs(_self, _self).exists(), "Application account not configured ");
		require_auth(configs(_self, _self).get().application);

		auto salescon = salesconfigs(_self, _self).get();

		name irespooracle = salescon.irespooracle;
		uint64_t currentTime = current_time();

		logs l(_self, _self);

		oracles o(irespooracle, irespooracle);

		auto iterOracle = o.find(oracle_id);
		auto iterLog = l.find(currentTime);

		if (iterLog == l.end()) {
			l.emplace(configs(_self, _self).get().application, [&](auto& row) {
				row.logtime = currentTime;
				row.irespooracle = salescon.irespooracle;
				row.exchangerate = iterOracle->value;
				row.salesstart = salescon.salesstart;
				row.salesend = salescon.salesend;
			});
		}

	}

	void iresposales::dellogdata(name application)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		logs l(_self, _self);

		auto iter = l.begin();

		while (iter != l.end())
		{
			iter = l.erase(iter);
		}
	}

	void iresposales::addpurchase(account_name purchaser, asset irespobought, asset eospaid)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured.");
		require_auth(configs(_self, _self).get().application);

		purchases p(_self, _self);
		auto iter = p.find(purchaser);
		
		if (iter == p.end()) {
			p.emplace(configs(_self, _self).get().application, [&](auto& row) {
				row.purchaser = purchaser;
				row.irespobought = irespobought;
				row.eospaid = eospaid;
				
			});
		}
		else {
			p.modify(iter, configs(_self, _self).get().application, [&](auto& row) {
				row.irespobought = irespobought;
				row.eospaid = eospaid;

			});
		}
	}

	void iresposales::apply(const account_name contract, const account_name act) {

		if (act == N(transfer)) {
			transferReceived(unpack_action_data<currency::transfer>(), contract);
			return;
		}

		auto &thiscontract = *this;

		switch (act) {
			EOSIO_API(iresposales, (setapp)(setsales)(addauser)(addausers)(delauser)(delall)(logdata)(dellogdata)(addpurchase))
		};
	}

	void iresposales::transferReceived(const currency::transfer &transfer, const account_name code) {
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		
		if (transfer.from != N(irespoappapp))
		{
			if (transfer.to != _self) {
				return;
			}
			eosio_assert(salesconfigs(_self, _self).exists(), "Sale not configured");

			eosio_assert(static_cast<uint32_t>(code == N(eosio.token)), "needs to come from eosio.token");
			eosio_assert(static_cast<uint32_t>(transfer.memo.length() > 0), "needs a memo with the name");
			eosio_assert(static_cast<uint32_t>(transfer.quantity.symbol == S(4, EOS)), "only EOS token allowed");
			eosio_assert(static_cast<uint32_t>(transfer.quantity.is_valid()), "invalid transfer");
			eosio_assert(static_cast<uint32_t>(transfer.quantity.amount >= 1000), "must be at least 0.1 EOS");

			uint64_t oracle_id = 1;
			auto salescon = salesconfigs(_self, _self).get();
			name irespooracle = salescon.irespooracle;
			uint32_t salesstart = salescon.salesstart;
			uint32_t salesend = salescon.salesend;
			uint32_t nowTime = now();

			//dates within ICO
			eosio_assert(salesstart < nowTime, "Sales has not started");
			eosio_assert(nowTime  < salesend, "Sales has ended");

			oracles o(irespooracle, irespooracle);
			auto iterOracle = o.find(oracle_id);
			uint64_t USDrate = iterOracle->value;

			eosio_assert(30000 <= USDrate && USDrate <= 120000, "Check EOS/USD rate");

			uint64_t ico_id = std::stoull(transfer.memo);
			auto iterUser = allowedicos.find(ico_id);

			//the sending account must match the one registered in our app - memo should contain the ico_id that can be found in the account settings
			require_auth(iterUser->user);

			asset receivedEOS = transfer.quantity;
			uint64_t EOSamount = receivedEOS.amount;
			uint64_t priceInUSDcents = 15;
			uint64_t IRESPOamount = (EOSamount * USDrate) / priceInUSDcents;

			asset IRESPOtoSend = asset(IRESPOamount, S(6, IRESPO));
			//sending IRESPO TOKENS
			action(permission_level{ _self, N(active) }, N(irespotokens), N(transfer),
				make_tuple(_self, transfer.from, IRESPOtoSend, string("Thank you for buying iRespo tokens!"))).send();

			//adding to purchase table
			purchases p(_self, _self);
			auto iterPurchase = p.find(transfer.from);

			if (iterPurchase == p.end())
			{
				p.emplace(configs(_self, _self).get().application, [&](auto& row) {
					row.purchaser = transfer.from;
					row.irespobought = IRESPOtoSend;
					row.eospaid = receivedEOS;
				});
			}
			else
			{
				p.modify(iterPurchase, configs(_self, _self).get().application, [&](auto& row) {
					row.irespobought += IRESPOtoSend;
					row.eospaid += receivedEOS;
				});
			}
		}
		
	}

} /// namespace irespo

extern "C" {

	using namespace irespo;
	using namespace eosio;

	void apply(uint64_t receiver, uint64_t code, uint64_t action) {
		auto self = receiver;
		iresposales contract(self);
		contract.apply(code, action);
		eosio_exit(0);
	}
}
