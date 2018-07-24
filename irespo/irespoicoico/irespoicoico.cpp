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
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		auto icocon = icoconfigs(_self, _self).get();

		name irespooracle = icocon.irespooracle;
		uint64_t currentTime = current_time();

		logs l(_self, _self);

		oracles o(irespooracle, irespooracle);

		auto iterOracle = o.find(1);
		auto iterLog = l.find(currentTime);

		if (iterLog != l.end()) {
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


} /// namespace irespo

