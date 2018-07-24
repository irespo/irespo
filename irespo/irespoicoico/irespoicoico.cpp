#include "irespoicoico.hpp"

namespace irespo {

	void irespoicoico::setapp(name application)
	{
		require_auth(_self);
		require_auth(application);

		eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
		configs(_self, _self).set(config{ application }, application);
	}

	void irespoicoico::setico(name irespooracle, uint32_t icostarttime, uint32_t icoendtime)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		icoconfigs(_self, _self).set(icoconfig{irespooracle,  icostarttime,  icoendtime }, configs(_self, _self).get().application);
	}

	void irespoicoico::addauser(uint64_t ico_id, name user)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
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

	void irespoicoico::addausers(vector<allowedico> users)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
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

	void irespoicoico::delauser(uint64_t ico_id)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		auto iter = allowedicos.find(ico_id);

		if (iter != allowedicos.end()) {
			allowedicos.erase(iter);
		}
	}

	void irespoicoico::delall(name application)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		auto iter = allowedicos.begin();

		while (iter != allowedicos.end())
		{
			iter = allowedicos.erase(iter);
		}
	}

	void irespoicoico::logdata(name application)
	{
		uint64_t oracle_id = 1;

		eosio_assert(configs(_self, _self).exists(), "Application account not configured ");
		require_auth(configs(_self, _self).get().application);

		auto icocon = icoconfigs(_self, _self).get();

		name irespooracle = icocon.irespooracle;
		uint64_t currentTime = current_time();

		logs l(_self, _self);

		oracles o(irespooracle, irespooracle);

		auto iterOracle = o.find(oracle_id);
		auto iterLog = l.find(currentTime);

		if (iterLog == l.end()) {
			l.emplace(configs(_self, _self).get().application, [&](auto& row) {
				row.logtime = currentTime;
				row.irespooracle = icocon.irespooracle;
				row.exchangerate = iterOracle->value;
				row.icostarttime = icocon.icostarttime;
				row.icoendtime = icocon.icoendtime;
			});
		}

	}

	void irespoicoico::dellogdata(name application)
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

	void irespoicoico::apply(const account_name contract, const account_name act) {

		if (act == N(transfer)) {
			transferReceived(unpack_action_data<currency::transfer>(), contract);
			return;
		}

		auto &thiscontract = *this;

		switch (act) {
			EOSIO_API(irespoicoico, (setapp)(setico)(addauser)(addausers)(delauser)(delall)(logdata)(dellogdata))
		};
	}

	void irespoicoico::transferReceived(const currency::transfer &transfer, const account_name code) {
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		
		if (code != N(irespoappapp))
		{
			eosio_assert(icoconfigs(_self, _self).exists(), "ICO not configured");

			eosio_assert(static_cast<uint32_t>(code == N(eosio.token)), "needs to come from eosio.token");
			eosio_assert(static_cast<uint32_t>(transfer.memo.length() > 0), "needs a memo with the name");
			eosio_assert(static_cast<uint32_t>(transfer.quantity.symbol == S(4, EOS)), "only EOS token allowed");
			eosio_assert(static_cast<uint32_t>(transfer.quantity.is_valid()), "invalid transfer");
			eosio_assert(static_cast<uint32_t>(transfer.quantity.amount > 0), "must be positive quantity");

			if (transfer.to != _self) {
				return;
			}

			uint64_t oracle_id = 1;
			auto icocon = icoconfigs(_self, _self).get();
			name irespooracle = icocon.irespooracle;
			uint32_t icostarttime = icocon.icostarttime;
			uint32_t icoendtime = icocon.icoendtime;
			uint32_t nowTime = now();

			//dates within ICO
			eosio_assert(icostarttime < nowTime, "ICO has not started");
			eosio_assert(nowTime  < icoendtime, "ICO has ended");

			oracles o(irespooracle, irespooracle);
			auto iterOracle = o.find(oracle_id);
			uint64_t value = iterOracle->value;

			eosio_assert(50000 <= value && value <= 200000, "Check EOS/USD rate");			

			uint64_t ico_id = std::stoull(transfer.memo);
			auto iterUser = allowedicos.find(ico_id);
			
			//the sending account must match the one registered in our app - memo should contain the ico_id that can be found in the account settings
			require_auth(iterUser->user);


			//sending IRESPO TOKENS
			action(permission_level{ _self, N(active) }, N(irespotokens), N(transfer),
				make_tuple(_self, to_account, quantity, string("irespoescrow to user account"))).send();

		}
		
	}

} /// namespace irespo

extern "C" {

	using namespace irespo;
	using namespace eosio;

	void apply(uint64_t receiver, uint64_t code, uint64_t action) {
		auto self = receiver;
		irespoicoico contract(self);
		contract.apply(code, action);
		eosio_exit(0);
	}
}
