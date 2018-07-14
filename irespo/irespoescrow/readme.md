**irespoescrow** account is used to store *IRESPO* tokens of the users that have not registered their *EOS* Account names. 

It is a solution for the users of the application that are registered and have tokens but do not have an *EOS* account to store it.

**irespoescrow** contains a table escrow that holds the information about the application user and his tokens.

It accepts only transfers of *IRESPO* tokens and only iRespo accounts have permissions to interact with the contract.

The methods are:
1. `setapp` - sets the application account - the signature of this account is required in all the other methods
2. `withdraw` - allow withdrawal of the funds of the particular user from the **irespoescrow** account. The typical call will be when user registers the account in the application and iRespo sends the tokens from **irespoescrow** to registered user *EOS* account. It is required to call instead of normal token transfer, as it also changes the balances within escrow table

Apart from the above methods there is external apply method that observes *IRESPO* token transfers to **irespoescrow**. If there is a transfer `transferReceived` method is called. The memo field contains Id of the user that should receive the funds and will determine the row of escrow table.
