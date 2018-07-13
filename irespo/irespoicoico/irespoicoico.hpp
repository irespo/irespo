#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

using namespace eosio;

namespace irespo {

	using std::string;

	class irespoicoico : public contract {
	public:
		irespoicoico(account_name self) :contract(self){}

		// @abi action
		void setapp(name application);

	}; /// namespace irespo

EOSIO_ABI(irespo::irespoicoico, (setapp))