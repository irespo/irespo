#include "irespoescrow.hpp"

namespace irespo {

	void irespoescrow::setapp(name application)
	{
		require_auth(_self);
		require_auth(application);

		eosio_assert(!configs(_self, _self).exists(), "Configuration already exists");
		configs(_self, _self).set(config{ application }, application);
	}

	




} /// namespace irespo

