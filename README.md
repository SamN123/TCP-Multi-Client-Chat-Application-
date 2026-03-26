# TCP-Multi-Client-Chat-Application-
This project creates a client chat application using the TCP transfer layer protocol. The project demonstrates the functionality of sending messages from multiple clients. 
TCP echo server in C using BSD sockets + `select()` for concurrency.

## Features
- Supports **multiple simultaneous clients**
- Echoes every message back to the client that sent it
- Single-process, non-blocking I/O with `select()`
- Clean disconnect handling
- Built and tested on Linux

- ## How to Build & Run

```bash
make
./server
