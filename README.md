To run the two Boost.Redis clients:

1. Clone the `redis` submodule

2. Build the clients:

```
make
```

3. Start the Redis server

4. Run the clients:

```sh
./pusher <queue_name> <redis_host> <redis_port>
```

```sh
./popper <queue_name> <redis_host> <redis_port>
```

The `<queue_name>`, `<redis_host>`, and `<redis_port>` arguments are optional. If not provided, default values will be used.
