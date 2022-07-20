/*
    Estabelecendo um socket no server-side da aplicação
*/

#include <algorithm>
#include <arpa/inet.h>
#include <deque>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "classChannel.h"

using namespace std;

void newConnection(sockaddr_in server_address, sockaddr_in client, Channel *c, int currentClient, string nick, vector<thread> *connectionsList, char *ip) {

    /*
        Legenda dos erros:

        0: Não é possível se auto banir.
        1: Você não está conectado ao canal para usar esse comando.
        2: Não foi possível achar este usuário no canal.
        3: Comando exclusivo do admin.

        Sucesso:
        0: Usuário banido do canal!
    */

    char buff[4096];    // variavel para armazenar mensagens do receptor (recv) e funções de envio
    char auxBuff[4096]; // variável aux para buffer
    char nickname[50];  // variável para armazenar o nick do cliente que mandou a mensagem

    int bytesSend = 0, bytesReceived = 0; // variável para controlar os bytes do recptor (recv) e funções de envio
    string msg, aux, auxToClientResponse;

    char confirmNick[69] = "Seu nick está registrado. Se divirta!!\n\nComece a mandar mensagens:";
    char welcomeMsg[46] = "Olá! Este é o chat por servidor TCP.\n";

    Member temp; // infos temporária do cliente que mandou msg
    temp.name = (*c).members[currentClient].name;
    temp.socket = (*c).members[currentClient].socket;

    nick.copy(nickname, nick.size()); // cliente atual salvo numa nova variável 

    for (int count = 0; count < 5; count++) {

        bytesSend = send(temp.socket, welcomeMsg, sizeof(welcomeMsg), 0); // Mensagem de boas vindas
        if (bytesSend < 0)
            continue;
        else {
            bytesSend = 0;
            break;
        }
    }

    for (int count = 0; count < 5; count++) {

        bytesSend = send(temp.socket, confirmNick, sizeof(confirmNick), 0); // Confirmando o nick

        if (bytesSend < 0)
            continue;
        else {
            bytesSend = 0;
            break;
        }
    }

    std::cout << "\n~ " << nickname << " entrou no canal " << (*c).name << ".\n";

    while (true) {

        aux.clear();              // Limpando aux.
        msg.erase(0, msg.size()); // Limpando mensagem.
        memset(buff, 0, 4096);    // Limpando buffer.
        bytesReceived = 0;        // Esvaziando bytesReceived.
        bytesSend = 0;            // Esvaziando bytesSend.

        bytesReceived = recv(temp.socket, buff, 4096, 0); // Esperando por uma mensagem

        // Checando erros
        if (bytesReceived == -1) {
            cerr << "Erro ao receber mensagem. Parando.\n";
            return;
        }

        else if (bytesReceived > 0) {

            string user = buff; // variável para armazenar usuário alvo de um comando enviado

            if (strcmp(buff, "Usuário banido") == 0) {
                (*connectionsList).erase((*connectionsList).begin() + currentClient); // removendo thread de cliente da lista de threads
                // std::cout << "Admin " << (*c).getAdmin().name << " baniu " << nickname << " do canal \"" << (*c).name << "\".";
                return;
            }

            // função /mute
            else if (strncmp(buff, "/mute", 5) == 0) {

                int res = 0; // variável para armazenar o resultado da próxima operação results of the next operation

                vector<Member>::iterator it = (*c).getMembersIterator(user.substr(6, user.length()));

                // Checks if user is not on channel
                if (it == (*c).members.end()) {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*c).members[currentClient].socket, nickname, sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send((*c).members[currentClient].socket, "This nickname could not be found on this channel.", 55, 0);
                        bytesSend = send((*c).members[currentClient].socket, "ERROR2", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }

                // Checks if user is admin of this channel
                else if ((*c).getAdmin().name.compare(nickname) == 0) {

                    aux = "Você foi mutado!";

                    // Sending to user that he/she has been muted
                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*it).socket, (*it).name.c_str(), 50, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // Sending message to user saying that he/she has been muted
                        bytesSend = send((*it).socket, aux.c_str(), 4096, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    res = (*c).muteUser(user.substr(6, user.length())); // muting user

                }

                // User is not admin of this channel so this command can't be used
                else {

                    // Sending his/she nickname
                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*c).members[currentClient].socket, nickname, sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    // Sending error message
                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send((*c).members[currentClient].socket, "ERROR. This command is admin only.", 55, 0);
                        bytesSend = send((*c).members[currentClient].socket, "ERROR3", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }
            }

            // implementation of /unmute
            else if (strncmp(buff, "/unmute", 7) == 0) {

                int res = 0; // variável to armazenar results of the next operation

                vector<Member>::iterator it = (*c).getMembersIterator(user.substr(8, user.length()));

                // Checks if user is not on channel
                if (it == (*c).members.end()) {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*c).members[currentClient].socket, nickname, sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send((*c).members[currentClient].socket, "This nickname could not be found on this channel.", 55, 0);
                        bytesSend = send((*c).members[currentClient].socket, "ERROR2", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }

                // Checks if user is admin of this channel
                else if ((*c).getAdmin().name.compare(nickname) == 0) {

                    aux = "Você foi desmutado!";

                    // Sending to user that he/she has been unmuted
                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*it).socket, (*it).name.c_str(), 50, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*it).socket, aux.c_str(), 4096, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    res = (*c).unmuteUser(user.substr(8, user.length())); // muting user

                }

                // User is not admin of this channel so this command can't be used
                else {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*c).members[currentClient].socket, nickname, sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send((*c).members[currentClient].socket, "ERROR. This command is admin only.", 55, 0);
                        bytesSend = send((*c).members[currentClient].socket, "ERROR3", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }
            }

            // implementation of /whois
            else if (strncmp(buff, "/whois", 6) == 0) {

                vector<Member>::iterator it = (*c).getMembersIterator(user.substr(7, user.length()));

                // Checks if user is not on channel
                if (it == (*c).members.end()) { // user is not on channel

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send(temp.socket, temp.name.c_str(), sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send(temp.socket, "This nickname could not be found on this channel.", 55, 0);
                        bytesSend = send(temp.socket, "ERROR2", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }

                // Checks if user is admin of this channel
                else if (temp.name.compare((*c).getAdmin().name) == 0) { // user is on channel and its an admin

                    string userIp = (*c).getUserHost((*it).name); // Getting user's ip

                    // Sending requested information only to admin
                    for (int count = 0; count < 5; count++) {

                        bytesSend = send(temp.socket, temp.name.c_str(), sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        aux = "Endereço IP do usuário " + (*it).name + " é " + userIp;
                        bytesSend = send(temp.socket, aux.c_str(), 4096, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }

                // User is not admin of this channel so this command can't be used
                else {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send(temp.socket, temp.name.c_str(), sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send(temp.socket, "ERROR. This command is admin only.", 55, 0);
                        bytesSend = send(temp.socket, "ERROR3", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }
            }

            // implementation of /kick
            else if (strncmp(buff, "/kick", 5) == 0) {

                int res = 0; // variável to armazenar results of the next operation

                vector<Member>::iterator it = (*c).getMembersIterator(user.substr(6, user.length()));

                // Checks if user is not on channel
                if (it == (*c).members.end()) {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send(temp.socket, temp.name.c_str(), sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send(temp.socket, "This nickname could not be found on this channel.", 55, 0);
                        bytesSend = send(temp.socket, "ERROR2", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }

                // Checks if user is admin of this channel
                else if (temp.name.compare((*c).getAdmin().name) == 0) {

                    aux = "Você foi banido!";

                    // Sending to user that he/she has been kicked from channel
                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*it).socket, (*it).name.c_str(), sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send((*it).socket, aux.c_str(), 4096, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    string kickedUser = (*it).name;

                    res = (*c).removeUser(user.substr(6, user.length())); // removing user from channel members list

                    // User successfully removed
                    if (res == 1) {

                        // std::cout << "Admin " << nickname << " just kicked " << user.substr(6, user.length()) << " from channel \"" << (*c).name << "\".";

                        // Sending to users that an admin has kicked someone out of the channel
                        for (int j = 0; j < (*c).members.size(); j++) {

                            if ((*c).members[j].socket != -1 && (*c).members[j].name.compare((*c).admin.name) != 0) {

                                // sending to clients that one of them diconnected from the server
                                for (int count = 0; count < 5; count++) {

                                    bytesSend = send((*c).members[j].socket, kickedUser.c_str(), sizeof(nickname), 0);

                                    if (bytesSend < 0)
                                        continue;
                                    else {
                                        bytesSend = 0;
                                        break;
                                    }
                                }

                                aux = " banido!\n";

                                for (int count = 0; count < 5; count++) {

                                    bytesSend = send((*c).members[j].socket, aux.c_str(), 4096, 0);

                                    if (bytesSend < 0)
                                        continue;
                                    else {
                                        bytesSend = 0;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

                // User is not admin of this channel so this command can't be used
                else {

                    for (int count = 0; count < 5; count++) {

                        bytesSend = send(temp.socket, temp.name.c_str(), sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send(temp.socket, "ERROR. This command is admin only.", 55, 0);
                        bytesSend = send(temp.socket, "ERROR3", 7, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }
            }

            // implementation of /quit command
            else if (strcmp(buff, "/quit") == 0) {

                for (int j = 0; j < (*c).members.size(); j++) {
                    if ((*c).members[j].socket != -1) {

                        // sending to clients that one of them diconnected from the server
                        if ((*c).members[j].name.compare(temp.name) != 0) {
                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*c).members[j].socket, temp.name.c_str(), sizeof(nickname), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }

                            aux = " desconectado!\n";

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*c).members[j].socket, aux.c_str(), 4096, 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }

                        // sending to client that user diconnected from the server
                        else {

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*c).members[j].socket, temp.name.c_str(), sizeof(nickname), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }

                            aux = "Você se deconectou! Até a próxima.";

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*c).members[j].socket, aux.c_str(), 4096, 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }
                    }
                }

                int ver = (*c).removeUser(temp.name); // Removing client from the list of connections

                std::cout << temp.name << " desconectou.\n";

                return;
            }

            // implementation of /ping command
            else if (strcmp(buff, "/ping") == 0) {

                for (int count = 0; count < 5; count++) {

                    bytesSend = send(temp.socket, temp.name.c_str(), sizeof(nickname), 0);

                    if (bytesSend < 0)
                        continue;
                    else {
                        bytesSend = 0;
                        break;
                    }
                }

                aux = "pong";

                for (int count = 0; count < 5; count++) {

                    bytesSend = send(temp.socket, aux.c_str(), 4096, 0);

                    if (bytesSend < 0)
                        continue;
                    else {
                        bytesSend = 0;
                        break;
                    }
                }
            }

            // normal messages
            else {

                // User is not muted so can send messages
                if ((*c).isMuted(temp.name) == 0) {

                    for (int j = 0; j < (*c).members.size(); j++) {
                        if ((*c).members[j].socket != -1 || (*c).members[j].name.compare(temp.name) == 0) {
                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*c).members[j].socket, temp.name.c_str(), sizeof(nickname), 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }
                    }

                    // If the received message ends with value 4, then the message is bigger than 4096 bytes.
                    while (buff[4095] == 4) {

                        buff[4095] = 0;

                        memset(buff, 0, 4096); // Limpando buffer.
                        bytesReceived = 0;     // Esvaziando variável.

                        bytesReceived = recv(temp.socket, buff, 4096, 0); // Receiving

                        if (bytesReceived < 0)
                            std::cout
                                << "Erro ao receber resposta do servidor.\n\n";

                        else if (bytesReceived == 0 || strlen(buff) == 0) {
                            std::cout << "\nRecenbendo nada1\n";
                            break;
                        }

                        else { // bytesReceived > 0
                            strcpy(auxBuff, buff);
                            msg += buff;

                            for (int j = 0; j < (*c).members.size(); j++) {

                                if ((*c).members[j].socket != -1 || (*c).members[j].name.compare(temp.name) == 0) {

                                    for (int count = 0; count < 5; count++) {

                                        bytesSend = send((*c).members[j].socket, buff, 4096, 0);

                                        if (bytesSend < 0)
                                            continue;
                                        else {
                                            bytesSend = 0;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Sending received message for all the clients except the one who send the current message
                    for (int j = 0; j < (*c).members.size(); j++) {

                        if ((*c).members[j].socket != -1 || (*c).members[j].name.compare(temp.name) == 0) {

                            for (int count = 0; count < 5; count++) {

                                bytesSend = send((*c).members[j].socket, buff, 4096, 0);

                                if (bytesSend < 0)
                                    continue;
                                else {
                                    bytesSend = 0;
                                    break;
                                }
                            }
                        }
                    }

                    // If the message is smaller than 4096 chars, i.e, the remaining of
                    // the big message, we are storing it in buff and sending to client.
                    buff[4095] = 0;
                    msg += buff;

                    std::cout << "\nMensagem de " << temp.name << ": " << msg << "\n"; // Displaying message
                }

                // User is muted so can't send messages
                else if ((*c).isMuted(temp.name) == 1) {

                    // Sending to user that he/she is muted
                    for (int count = 0; count < 5; count++) {

                        bytesSend = send(temp.socket, nickname, sizeof(nickname), 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }

                    for (int count = 0; count < 5; count++) {

                        // bytesSend = send(temp.socket, "This nickname could not be found on this channel.", 55, 0);
                        bytesSend = send(temp.socket, "Você está mutado...", 4096, 0);

                        if (bytesSend < 0)
                            continue;
                        else {
                            bytesSend = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void newChannel(Channel *c, sockaddr_in server_address, sockaddr_in client, string nickname, deque<Channel> *channels, char *ip) {

    vector<thread> threads; // Will armazenar the created threads to the new connection on channel
    int numConnections = 0; // Represents the number of connected clients on this channel

    while (true) {

        // Verifying the number of created threads by this thread (newChannel) and the members number in this channel.
        // If they are different them create another thread para handle the new connection

        if ((*c).members.size() < SOMAXCONN && (*c).numConnections < (*c).members.size()) {

            nickname = (*c).members[(*c).members.size() - 1].name; //adjusting the nickname para create the connection

            // Creating one thread for the requisition that just was accepted
            threads.push_back(thread(newConnection, server_address, client, c, (*c).members.size() - 1, nickname, &threads, ip));

            // Detach the new thread from the main thread
            threads[threads.size() - 1].detach();

            // Increases the num of connected members
            (*c).increaseNumConnections();

            numConnections++;
        }

        if (numConnections > (*c).members.size())
            numConnections--;

        // Channel doesnt have any members, so close it
        if ((*c).numConnections == 0) {
            for (deque<Channel>::iterator ptr = (*channels).begin(); ptr < (*channels).end(); ptr++) {
                if ((*ptr).name.compare((*c).name) == 0) {
                    (*channels).erase(ptr);
                    break;
                }
            }
            std::cout << "\n\nCanal " << (*c).name << " desfeito!\n";
            return;
        }
    }
}

int main(int argc, char const *argv[]) {

    // Creating um sockaddr_in which describes the internet protocol version 4 (ipv4)
    int port = 60000;
    string ipAddress = "0.0.0.0";

    sockaddr_in server_address;
    socklen_t serverSize = sizeof(server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &server_address.sin_addr);

    std::cout << "Servidor aberto no IP " << ipAddress << " porta " << port << "\n";

    // Creating um socket with IPv4 Internet protocols as communication domain and TCP as communication semantics.
    // AF_INET = IPv4 protocol, SOCK_STREAM = TCP.
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);

    // Checando for erros while criando um socket
    if (socket == -1) {
        cerr << "Não foi possível criar socket.";
        return -1;
    }

    // Binding the created socket 'socket' to an available address 'server_address' of size 'sizeof(server_address)' bytes.
    // This step is commonly named as 'lqassigning a name to a socketrq'.
    if (bind(socket, (sockaddr *)&server_address, serverSize) != 0) {
        cerr << "Não se conectou ao IP/port.\n";
        return -2;
    }

    // Listenning for connections by marking 'socket' as a passive connection, i.e., 'a socket
    // that will be used to accept incoming connection requests using accept()' later.
    if (listen(socket, SOMAXCONN) != 0) {
        cerr << "Não é possível ouvir esta porta\n";
        return -3;
    }

    string nickname;
    int clientSocket;
    sockaddr_in client;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    socklen_t clientSize = sizeof(client);

    char buffer[4096];

    deque<Channel> channels; //list of channels

    bool exists;       // flag para control if a channel already exists
    int bytesReceived; // variável para control num of received bytes from recv function
    string channelName, auxID;

    vector<thread> channelThreads; // Will armazenar the threads created to the new connections on the server

    while (true) {

        exists = false;
        nickname.clear();
        channelName.clear();
        memset(buffer, 0, 4096);
        clientSize = sizeof(client);

        clientSocket = accept(socket, (sockaddr *)&client, &clientSize);

        if (clientSocket == -1) {
            cerr << "Erro ao se conectar ao socket.";
            return -4;
        }

        // Getting computer information
        int info = getnameinfo((sockaddr *)&server_address, serverSize, host, NI_MAXHOST, service, NI_MAXSERV, 0);

        if (info)
            std::cout << host << " conectado no " << service << "\n";
        else {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);

            // Getting user IP
            getpeername(socket, (sockaddr *)&client, &clientSize);

            std::cout << "\n"
                      << inet_ntoa(client.sin_addr) << " conectado na porta " << ntohs(server_address.sin_port) << "\n";
        }

        // Receiving the nickname of the client
        bytesReceived = recv(clientSocket, buffer, 50, 0); // Esperando command from client

        if (bytesReceived < 1) {
            std::cout << "Algo deu errado ao receber nick do cliente...\n";
            return 1;
        }

        else if (strcmp(buffer, "Usuário saiu da aplicação") == 0) {
            std::cout << "\nUma tentativa de conexão foi feita, mas o cliente não digitou um nick.\n";
            continue;
        }

        nickname = buffer;

        // Trying to create a new Channel or add a new user to an existing channel
        while (true) {

            memset(buffer, 0, 4096);

            // Receiving the name of the channel
            bytesReceived = recv(clientSocket, buffer, 26, 0);

            if (bytesReceived < 1) {
                std::cout << "Algo deu errado enquanto recebia o nome do canal...\n";
                return 1;
            }

            // Command /list
            else if (strcmp(buffer, "/list") == 0) {

                for (deque<Channel>::iterator ptr = channels.begin(); ptr < channels.end(); ptr++) {
                    strcpy(buffer, (*ptr).name.c_str());
                    send(clientSocket, buffer, 4096, 0);
                }

                strcpy(buffer, "terminou"); // sending a flag indicating that the reading ended
                send(clientSocket, buffer, 4096, 0);
            }

            // Command /join
            else if (strlen(buffer) > 7 && (strncmp(buffer, "/join &", 7) == 0 || strncmp(buffer, "/join #", 7) == 0)) {

                int k = 0;

                int size = channels.size(); // how many channels exists

                exists = false;
                channelName = buffer;
                channelName.erase(0, 6); // Leaving only the name of the channel in the string

                // Check if the channel already exists
                for (k = 0; k < size; k++) {
                    if (channels[k].name.compare(channelName.c_str()) == 0) {
                        exists = true;
                        break;
                    }
                }

                // If channel already exists
                if (exists) {

                    if (channels[k].addUser(nickname, clientSocket, inet_ntoa(client.sin_addr))) {
                        ::send(clientSocket, "Cliente adicionado com sucesso!\n", 86, 0);
                        break;
                    }

                    else {

                        std::cout << "Um membro tentou entrar no canal " << channels[k].name << " mas já tem um membro com esse nick...\n";

                        ::send(clientSocket, "Já tem um membro nesse canal com este mesmo nick. Por favor, entre com outro.", 86, 0);
                        continue;
                    }
                }

                // If channel doesnt exists
                else if (!exists) {

                    // Creating and adding aux channel to deque
                    Channel aux(channelName, nickname, clientSocket, inet_ntoa(client.sin_addr));
                    channels.push_back(aux);

                    // Calling a function that will create a thread for the new channel
                    channelThreads.push_back(thread(newChannel, &channels[channels.size() - 1], server_address, client, nickname, &channels, host));

                    // Detach the new thread from the main thread
                    channelThreads[channelThreads.size() - 1].detach();

                    ::send(clientSocket, "Cliente adicionado com sucesso!\n", 86, 0);
                    break;
                }
            }
        }
    }

    close(socket);

    return 1;
}
