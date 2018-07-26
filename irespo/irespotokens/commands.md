./cleos.sh -u https://nodes.get-scatter.com push action irespotokens create '{ "issuer": "irespoappapp", "maximum_supply": "120000000.000000 IRESPO", "transfer_locked": true}' -p irespotokens

./cleos.sh -u https://nodes.get-scatter.com push action irespotokens issue '{ "to": "irespoappapp", "quantity": "120000000.000000 IRESPO", "memo": "Issue of the tokens"}' -p irespoappapp

./cleos.sh -u https://nodes.get-scatter.com push action irespotokens transfer '{"from": "irespoappapp", "to": "irespoicoico", "quantity": "80000000.000000 IRESPO", "memo": "transfer to ICO account"}' -p irespoappapp