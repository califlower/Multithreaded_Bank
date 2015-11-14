# Multithreaded_Bank
A multithreaded bank application

##Client

This is a command line program that takes 1 argument
To correctly connect to the server, type the address of the machine 
it's running on. 

As of now, only inputting localhost will work

##Server

This is a command line program that takes no arguments. Simply run it, and then keep it in the background

**open**
Starts a new account. Account name must be under 100 characters. You cannot have more than 20 accounts at any one time
Accounts get deleted on server closing.

**start**

**debit**
Removes money from an account. 

**credit**
Credits money to an ccount

**finish**
Finishes the session cleanly and exits all threads. Better than closing the terminal though
there is a signal handler that should handle closing a terminal unexpectedly and other things like Control C
