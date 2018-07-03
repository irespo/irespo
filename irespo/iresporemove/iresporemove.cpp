#include <eosiolib/eosio.hpp>
using namespace eosio;

class iresporemove : public eosio::contract {
public:
	using contract::contract;

	/// @abi action 
	void a(account_name user) {
		
	}
};

EOSIO_ABI(iresporemove, (a))