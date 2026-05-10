# EmbedKit_Ganesh

Name: Ganesh

## Build

```sh
gcc -Wall -std=c99 ringbuf.c -o ringbuf
```

## Run

```sh
./ringbuf
```

## Modules

- `ringbuf.c`: Fixed-size `uint8_t` circular buffer with write, read, count, full, and empty operations.
