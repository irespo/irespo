#include "irespoicoico.hpp"

namespace irespo {

	void irespoicoico::setapp(name application)
	{
		require_auth(_self);
		require_auth(application);

		eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
		configs(_self, _self).set(config{ application }, application);
	}

	void irespoicoico::addauser(name user)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		auto iter = allowedusers.find(user);

		if (iter == allowedusers.end()) {

			allowedusers.emplace(configs(_self, _self).get().application, [&](auto& row) {
				row.user = user;
			});
		}
	}

	void irespoicoico::addausers(vector<name> users)
	{
		eosio_assert(configs(_self, _self).exists(), "Application account not configured");
		require_auth(configs(_self, _self).get().application);

		for (auto&& n : v) {
			auto iter = allowedusers.find(n);

			if (iter == allowedusers.end()) {

				allowedusers.emplace(configs(_self, _self).get().application, [&](auto& row) {
					row.user = n;
				});
			}
		}	
	}

} /// namespace irespo

