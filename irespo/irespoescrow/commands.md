```
./cleos.sh -u https://nodes.get-scatter.com push action irespoescrow setapp '["irespoescrow"]' -p irespoescrow

./cleos.sh -u https://nodes.get-scatter.com set account permission irespoescrow active '{"threshold": 1, "keys":[{"key":"EOS6iURLMSeuGLtPUn5c2Jtw2S5LBXRNkZS1Poox4mEVUYvvAGytk", "weight":1}]
, "accounts":[{"permission":{"actor":"irespoescrow","permission":"eosio.code"},"weight":1}], "waits":[] }' owner -p irespoescrow
```