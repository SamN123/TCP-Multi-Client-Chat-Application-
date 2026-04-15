# TCP-Multi-Client-Chat-Application-

This project creates a client chat application using the TCP transfer layer protocol. The project demonstrates the functionality of sending messages from multiple clients.

TCP echo server in C using BSD sockets + `select()` for concurrency.

---

## Features

* Supports **multiple simultaneous clients**
* Echoes every message back to the client that sent it
* Uses `select()` for handling multiple connections in a single process
* File-based authentication system (`users.txt`)
* Clean disconnect handling
* Built and tested on Linux (Ubuntu)

---

## Project Description

This application implements a TCP-based client-server chat system. The server listens for incoming client connections and manages multiple clients at the same time using I/O multiplexing with `select()`.

Each client must authenticate using a username and password stored in a text file before being allowed to send messages. Once authenticated, clients can send messages to the server, which echoes the message back to the same client.

The goal of this project is to demonstrate:

* TCP socket programming
* Multi-client handling without threads
* Basic authentication using file parsing
* Real-time message exchange

---

## How to Compile and Run

### 1. Clone the repository

```bash
git clone https://github.com/SamN123/TCP-Multi-Client-Chat-Application-.git
cd TCP-Multi-Client-Chat-Application-
```

### 2. Compile the program

```bash
gcc -Wall -Wextra mainDriver.c server_Imp.c client_Imp.c -o chatapp
```

### 3. Run the server

```bash
./chatapp server
```

### 4. Run clients (in separate terminals)

```bash
./chatapp client
```

---

## Authentication Setup

Ensure `users.txt` is formatted as:

```
username,password
username,password
```

Example:

```
client1,one
client2,two
client3,three
```

---

## Design Overview

### Server Design

* Uses a **single process** with `select()` to handle multiple clients
* Maintains a list of active client sockets
* Monitors all sockets for activity using file descriptor sets (`fd_set`)
* Accepts new connections and processes client messages in a loop

### Client Design

* Connects to the server using TCP
* Sends authentication credentials (`AUTH|username|password`)
* Enters a loop to send and receive messages
* Terminates when a negative integer is entered

### Communication Flow

1. Client connects to server
2. Client sends login credentials
3. Server validates credentials using `users.txt`
4. If valid → client enters messaging loop
   If invalid → connection is closed
5. Server echoes messages back to the originating client

---

## Known Limitations

* Uses `select()`, which is limited by `FD_SETSIZE` (max number of clients)
* No encryption (credentials and messages are sent in plain text)
* No message broadcasting (only echoes back to sender)
* File-based authentication is not scalable for large systems
* No persistent storage of chat history
* Limited error handling for malformed client input

---

## Notes

* Make sure the server is run from the same directory as `users.txt`
* Use multiple terminals to simulate multiple clients
* If the server does not start, ensure the port is not already in use (`pkill chatapp`)

---
