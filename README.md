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

You can either type 

*open accountname* together as one command

or

*open*

*accountname*  as two commands


**start**


Starts an account session for a client. Can only start one account session per client per account

You can either type 

*start accountname* together as one command

or

*start*

*accountname*  as two commands



**debit**

Removes money from an account. 

**credit**

Credits money to an account

**finish**

Finishes the session cleanly and exits all threads. Better than closing the terminal though
there is a signal handler that should handle closing a terminal unexpectedly and other things like Control C
