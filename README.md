# Proxy Server for Postgres

## Author

[Igor Parfenov](https://t.me/Igor_Parfenov)

## Info

This is a simple *proxy server* (a program, though which you can connect to remote server, and it will redirect all packets), which logs *postgres* queries.

## Build

Prerequisits:
* `gcc`
* `make`

Recipes:
* `proxy` - builds a proxy server
* `run` - builds and runs a proxy server with remote ip address `127.0.0.1`, port forwarding from `1234` to `5432` and log file `logs/log`
* `stress` - builds and executes stress test (many randomized connections and queries to port `1234`)

You can execute `build/proxy/proxy` manually and specify:
* remote ip address
* local port
* remote port
* file for log storage

Installation of `postgres` and `psql` utility depends on your setup and can be not easy. You may be required to change `pg_hba.conf` file. For simplicity, you can prepend there line `host all all -1.0.0.0/0 scram-sha-256`.
