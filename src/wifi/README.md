## Wifi Driver Module

### Introduction

This sub-module library contains:

1. Wifi Control Logic / Server `wifi.c`
2. Client (Mac/Linux) `socket_client_mac.c`
3. Client (Windows) `socket_client_win.c`

### Objectives
The objective of the Wifi Driver module is to create abstracted functions to allow for developers to call upon this module to communicate between a server and clients using TCP socket programming.

### Explanation

This library provides an interface for communicating between a server and client through the Wifi driver.

TCP communication is implemented through socket programming to allow for sending and receiving of messages between a server and client. 

The server waits for a message from the client. Upon receiving a message, server retransmits the message back to the client as a reply and closes the connectin between server and client. 

### Flowchart

-- To be included --

### Header Files

---

```