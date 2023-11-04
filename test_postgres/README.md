# test_postgres

I don't really know what I'm doing. :0

1. [Installation](#installation)
2. [Helpful Commands](#helpful-commands)
3. [Helpful Scripts](#helpful-scripts)

## Installation
To install PostgreSQL in WSL2, see the following: [link](https://learn.microsoft.com/en-us/windows/wsl/tutorials/wsl-database#install-postgresql). 

## Helpful Commands

To access the shell
```
sudo -u postgres psql
```
(Assumption: `postgres=#`) 
```
\c <DATABASE_NAME>  --> connects to db
\dt                 --> displays tables
```
Start/stop/view status
```
sudo service postgresql status 
sudo service postgresql start
sudo service postgresql stop
```
Just be postgres
```
sudo -i -u postgres 
```

## Helpful Scripts
This creates `bindb` and an empty `bintable`.
```
psql -f setup.sql
```
