# IRC-Chat (Internet Relay Chat)

Projeto criado para **simular um chat IRC* adptando as [especificções RFC 1459](https://tools.ietf.org/html/rfc1459). O projeto suporta mais de um cliente conectado ao mesmo tempo usando multithread.

## Membros

- Jônatas Alves Lopes, NUSP 11796552 ([jony](https://github.com/Jony33Invent));
- Pedro Henrique Raymundi, NUSP 11795634 ([pedro](https://github.com/PedroRaymundi)).
- Wictor Dalbosco Silva, NUSP 11871027 ([wictor](https://github.com/WictorDalbosco)).

## Desenvolvimento

O projeto foi desnvolvido utilizando **linguagem C++11** e um **compilador g++**. Os testes foram feitos no Ubuntu 18.04.2 usando WSL no Windows.

## Como usar

- Certifique-se que o **make** está instalado e utilize `$ make all` para compilar todos os códigos fonte;
- Para simular uma interação, abra ao menos **três terminais**;
- No primeiro, será compilado o **servidor** utilizando o comando abaixo, que informará que a conexão foi estabelecida.
```
$ make s
```
- No segundo e terceiro terminais, serão os clientes, que serão compilado com o comando abaixo, e repita em quantos terminais quiser para simular um chat. Lembrando que o IP e porta são fornecidos no primeiro terminal que foi aberto. IP e porta padrão 0.0.0.0 e 60000.
```
$ make c
```
- Agora só seguir as instruções dadas na aplicação.
