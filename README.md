# IRC-Chat (Internet Relay Chat)

This project was created in order to **simulate an Internet Relay Chat (IRC)** adapting [RFC 1459 specifications](https://tools.ietf.org/html/rfc1459). Currently, this project **supports more than one client connected at the same time** by using multithreads.

## Development

All the project was developed using **C++11 languague** and **g++ compiler** on Fedora 31 workstation. We also tested on Ubuntu 18.04.2. using WSL on Windows.

## How to use

- Certify that you have **make** installed and use `$ make all` to compile the source code;
- For a first interaction, **open three terminals**;
- On first terminal, use the command below for **ssh tunneling with remote port forwarding** from serveo.net or other trusted source of yours, which allows connections from outside of your localhost (optional): 
```
$ ssh -R 60000:localhost:60000 serveo.net
```
- On second terminal, run **server side** application by using the command below. You won't see nothing but a message telling you on what ip address and port the server is running after using it:
```
$ make s
```
- On last (and others) terminal(s), use the following command to run **client side application**. Enter **server's ip** (which is **0.0.0.0** if you're in the same network where server is running **or 159.89.214.31** if you're running with the optional ssh tunneling) and **server's port**, which is **60000** by default (see [Notes](https://github.com/henriquesqs/University/tree/master/Computer%20Networks#Notes) for entering a correct port);
```
$ make c
```
- On terminal(s) running client side, wait for the welcome message and follow the instructions provided by the application.

## Notes
Perhaps you'll need to change the port where server is running. To do so, go to [this line on serverMultithread.cpp](https://github.com/henriquesqs/University/blob/ee87ab6dfadc5293c767998f89ddf106f587d18e/Computer%20Networks/chat/serverMultithread.cpp#L825) and change the int variable "port" to an available port of your Operating system (OS). Also, you'll need to change the port on ssh command line from 60000 to whatever port you're now using and also change the entered port on client side application when asked.
