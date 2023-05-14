# IRC-Chat (Internet Relay Chat)

This project was created to **simulate an IRC chat** by adapting the [RFC 1459 specifications](https://tools.ietf.org/html/rfc1459). The project supports multiple clients connected at the same time using multithreading.

## Members

- [Jônatas](https://github.com/Jony33Invent) Alves Lopes, NUSP 11796552;
- [Pedro](https://github.com/PedroRaymundi) Henrique Raymundi, NUSP 11795634;
-  [Wictor](https://github.com/WictorDalbosco) Dalbosco Silva, NUSP 11871027.

## Development

The project was developed using **C++11** language and a **g++ compiler**. The tests were done on Ubuntu 18.04.2 using WSL on Windows.

## How to use

- Make sure **make** is installed and use `$ make all` to compile all source codes;
- To simulate an interaction, open at least **three terminals**;
- In the first one, the **server** will be compiled using the command below, which will inform that the connection has been established.

```
$ make s
```
- In the second and third terminals, the **clients** will be compiled with the command below, and repeat it in as many terminals as you want to simulate a chat. Remember that the IP and port are provided in the first terminal that was opened. The default IP and port are 0.0.0.0 and 60000.

```
$ make c
```
- Now just follow the instructions given in the application.

A demonstration video can be found at this [link](https://drive.google.com/file/d/1qGheYsW0si0KAt_xWnuBftutnJTb9YLF/view?usp=sharing)
