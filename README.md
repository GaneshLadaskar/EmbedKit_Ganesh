# EmbedKit_Ganesh

**Author:** Ganesh Ladaskar

## Build Instructions
gcc -Wall -std=c99 ringbuf.c -o ringbuf

## Run
./ringbuf

## Modules
- **ringbuf.c** — Fixed-capacity circular (ring) buffer for uint8_t data
  with 8-byte capacity; supports ISR-safe write/read with overflow and
  underflow protection. Uses bitwise AND for fast wrap-around without division.
