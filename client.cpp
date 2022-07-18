/*
    Establishing a socket to the client side of application.
*/

#include <algorithm>
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

bool fsigint = false; // bool to check if we caught a SIGINT

struct sigaction sigIntHandler; // struct that handles with SIGINT

// Method that handles /quit requestes from users
int quit() {

    char c;

    while (true) {

        fsigint = false;
        fflush(stdin);
        std::cin.clear();

        std::cin >> c;

        if (c == 's' || c == 'S' || (std::cin.eof() && !fsigint)) {
            std::cout << "Até a próxima!!\n";
            return 0;
        }

        else if (c == 'n' || c == 'N') {
            std::cout << "Ok. Digite /help se quiser ver suas opções novamente.\n";
            return 1;
        }

        else {
            std::cout << "Não válido. Digite 's' se você quiser sair da aplicação, e 'n' caso queira ficar.\n";
            continue;
        }
    }
}

// Method to show help menu options to clients
void help() {

    std::cout << "Comandos que você pode utilizar:\n\n"
              << "1) /connect - te conecta a um servidor, você precisará de um endereço IP e o número da porta que o servidor está ouvindo\n"
              << "2) /nickname seuNick - é necessário um nick para enviar mensagens num servidor\n"
              << "3) /ping - use este comando para testar se você está conectado ao servidor, caso esteja tudo ok, você verá um 'pong' na tela retornado pelo servidor\n"
              << "4) /quit ou apert ctrl+D no teclado - desconecta do servidor\n"
              << "5) /help - para obter ajuda, conectado ou não a um servidor\n\n"
              << "P.S.: Quando você estiver conectado a um servidor, para mandar mensagens e usar estes comandos, providencie seu nick e, se estiver tudo correto, você poderá enviar mensagens! Para fazer isso, digite o comando e então pressione a tecla Enter, e então digite /send e então pressione Enter novamente.\n\n"
                 "Divirta-se!\n\n";
    return;
}

// Method that handles ctrl+c (SIGINT)
void my_handler(int s) {

    if (s == SIGINT) {
        std::cout << "\nFoi recebido um sinal 2 (SIGINT). Você deseja realmente sair? Se quiser digite /quit.\n";
        fsigint = true;
        fflush(stdin);    // clearing stdin
        std::cin.clear(); // clearing std::cin

    } else
        std::cout << "\nRecebeu sinal " << s << "\n";
}

void threadReceive(int socket, string nicknameClient, int *receiveReturn) {

    char buff[4096];       // All messages from the server will be stored here
    char nickname[50];     // Var to store nickname of client
    int bytesReceived = 0; // Variable responsible to store results of recv()
    string serverResponse; // Stores server response in order to do a later verification

    while (true) {

        memset(nickname, 0, 50); // Cleaning the buffer
        memset(buff, 0, 4096);   // Cleaning the buffer

        bytesReceived = recv(socket, nickname, sizeof(nickname), 0); // Receiving nickname from server

        if (bytesReceived < 0)
            std::cout << "Erro ao tentar receber resposta do servidor.\n\n";

        // else if (bytesReceived == 0 || nick.length() == 0) {
        else if (bytesReceived == 0 || strlen(nickname) == 0) {
            continue;
        }

        else if (bytesReceived > 0) {

            // if the nickname received is the same as current client then is /ping or /quit or /kick or /whois command
            if (nicknameClient.compare(nickname) == 0) {

                memset(buff, 0, 4096); // Cleaning the buffer

                bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

                // Checks if user entered CTRL+D or wants to quit
                if (strcmp(buff, "Você se desconectou! Até a próxima.") == 0) {
                    std::cout << "Você se desconectou! Até a próxima.\n";
                    return;
                }

                // Checking if user is muted
                else if (strcmp(buff, "Você está mutado...") == 0) {
                    std::cout << "Você está mutado...\n\n";
                    memset(buff, 0, 4096); // Cleaning the buffer
                    continue;
                }

                else if (strcmp(buff, "Você foi mutado!") == 0) {
                    std::cout << "Você foi mutado!\n\n";
                    memset(buff, 0, 4096); // Cleaning the buffer
                    continue;
                }

                else if (strcmp(buff, "Você foi desmutado!") == 0) {
                    std::cout << "Você foi desmutado!\n\n";
                    memset(buff, 0, 4096); // Cleaning the buffer
                    continue;
                }

                // Checking if user has been kicked
                else if (strcmp(buff, "Você foi banido!") == 0) {
                    std::cout << "Você foi banido! Até a próxima.\n";
                    ::send(socket, "Usuário banido", 4096, 0); // Send the message to server
                    *receiveReturn = 1;
                    memset(buff, 0, 4096); // Cleaning the buffer

                    return;
                }

                // Checks if user is testing connection with server
                else if (strcmp(buff, "pong") == 0) {
                    std::cout << "pong\n";
                    memset(buff, 0, 4096); // Cleaning the buffer
                }

                // Checking for errors
                else if (strcmp(buff, "ERROR0") == 0) {
                    std::cout << "Não é possível banir você mesmo.\n\n";
                    continue;
                }

                else if (strcmp(buff, "ERROR1") == 0) {
                    std::cout << "Você não está conectado a um canal para usar este comando.\n\n";
                    continue;
                }

                else if (strcmp(buff, "ERROR2") == 0) {
                    std::cout << "Nick não encontrado no canal.\n\n";
                    continue;
                }

                else if (strcmp(buff, "ERROR3") == 0) {
                    std::cout << "Comando para admins somente.\n\n";
                    continue;
                }

                else if (strcmp(buff, "SUCCES") == 0) {
                    std::cout << "Usuário banido com sucesso!\n\n";
                    continue;
                }

                // Checking if server response is the ip address from someone
                else if (strncmp(buff, "Endereço IP do usuário", 22) == 0) {
                    std::cout << buff << "\n\n";
                    // se começar a dar erro é pq por algum motivo eu corrigi com esses dois "\n\n"
                    memset(buff, 0, 4096); // Cleaning the buffer
                    continue;
                }
            }

            else {

                serverResponse = "";
                memset(buff, 0, 4096);                       // Cleaning the buffer
                bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

                if (strcmp(buff, " desconectado!\n") == 0)
                    std::cout << nickname << buff << "\n\n";

                else if (strcmp(buff, " banido!\n") == 0)
                    std::cout << nickname << buff << "\n\n";

                else if (strcmp(buff, "Você foi mutado!") == 0)
                    std::cout << "Você foi mutado!\n\n";

                else if (strcmp(buff, "Você foi desmutado!") == 0)
                    std::cout << "Você foi desmutado!\n\n";

                else {
                    while (buff[4095] == 4) {

                        buff[4095] = 0;
                        serverResponse += buff;

                        memset(buff, 0, 4096); // Cleaning the buffer
                        bytesReceived = 0;

                        bytesReceived = recv(socket, buff, 4096, 0); // Receiving message from server

                        if (bytesReceived == -1)
                            std::cout << "Erro ao receber resposta do servidor\n\n";
                    }

                    serverResponse += buff;
                    string tmpNick = nickname;

                    // Avoiding receiving special and non-readable characters
                    if (tmpNick.empty() || tmpNick.find_first_not_of("!@#$%¨&*()abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-+=/[]~´`;ªº ") == std::string::npos || serverResponse.empty() || serverResponse.find_first_not_of("!@#$%¨&*()abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_-+=/[]~´`;ªº ") == std::string::npos) {
                        // Printing the messages received fom the server
                        std::cout << "\n> " << nickname << ": ";
                        std::cout << serverResponse + "\n";
                    }
                }
            }
        }
    }
}

void threadSend(int socket, int *receiveReturn, string nickname) {

    int send = 0; // Variable responsible for store results of send()
    string input; // All user input messages will be stored here
    string aux2;  // Second auxiliar string
    string user;  // String to store user that admin wants to kick

    char buff[4096];   // All messages from the server will be stored here
    string auxCompare; // Aux string to verify server messages. It stores messages received in the program start

    string serverResponse;   // Stores server response in order to do a later verification
    bool bigMessage = false; // Flag to know if we're sending a big message (more than 4096 bytes)
    int bytesReceived = 0;   // Variable responsible to store results of recv()

    while (true) {

        // Reset below vars at each loop
        send = 0;
        input.clear();
        aux2.clear();
        memset(buff, 0, 4096);
        auxCompare.clear();
        serverResponse.clear();
        bigMessage = false;
        bytesReceived = 0;
        fsigint = false;
        user.clear();

        //the current client has been kicked form the channel
        if (*receiveReturn == 1) {
            // std::cout << "\n\npercebeu q foi kikado - thread send\n\n";
            // ::send(socket, "User kicked", 4096, 0); // Send the message to server
            return;
        }

        // Now, client app will read everything until it founds a /send, which its the command to send the message.
        // This allows the user to send more than a paragraph per message
        while (true) {

            fsigint = false;
            fflush(stdin);
            std::cin.clear();

            if (*receiveReturn == 1) {
                // std::cout << "\n\npercebeu q foi kikado - thread send\n\n";
                ::send(socket, "Usuário banido", 4096, 0); // Send the message to server
                return;
            }

            std::getline(std::cin >> std::ws, aux2);

            if (std::cin.eof() && !fsigint) {
                ::send(socket, "/quit", 4096, 0); // Send the message to server
                // std::cout << "Você se desconectou! Até a próxima.";
                input += "/quit";
                break;
                // return;
            }

            else if (aux2.compare("/send") != 0) {
                input += aux2;
                input += "\n";
            }

            else {
                if (input.size() == 0) {
                    std::cout << "Não é possível enviar mensagens vazias, escreva algo.\n";
                    continue;
                }
                input.erase(input.size() - 1); // Removing the last "\n" that was added before
                break;
            }
        }

        std::cout << "\n< You: " << input << "\n\n";

        // User is trying to get help
        if (input.compare("/help") == 0) {
            help();
            continue;
        }

        // User is trying to /kick someone
        else if (strncmp(input.c_str(), "/kick", 5) == 0) {

            user = input.substr(6, input.length()); // user to be kicked

            // Checks if user is trying to self kick
            if (user.compare(nickname.c_str()) == 0) {
                std::cout << "Não dá para você se banir né...\n\n";
                continue;
            }

            // Checks if user is trying to enter an empty nickname
            else if (all_of(user.begin(), user.end(), ::isspace)) {
                std::cout << "Nick inválido. Não insira um nick vazio. Tente novamente.\n\n";
                continue;
            }
        }

        // User is trying to /mute someone
        else if (strncmp(input.c_str(), "/mute", 5) == 0) {

            user = input.substr(6, input.length()); // user to be muted

            // Checks if user is trying to self mute
            if (user.compare(nickname.c_str()) == 0) {
                std::cout << "Não dá para se mutar...\n\n";
                continue;
            }

            // Checks if user is trying to enter an empty nickname
            if (all_of(user.begin(), user.end(), ::isspace)) {
                std::cout << "Nick inválido. Não insira um nick vazio. Tente novamente.\n\n";
                continue;
            }
        }

        // User is trying to /unmute someone
        else if (strncmp(input.c_str(), "/unmute", 7) == 0) {

            user = input.substr(8, input.length()); // user to be muted

            // Checks if user is trying to self unmute
            if (user.compare(nickname.c_str()) == 0) {
                std::cout << "Não para se desmutar...\n\n";
                continue;
            }

            // Checks if user is trying to enter an empty nickname
            if (all_of(user.begin(), user.end(), ::isspace)) {
                std::cout << "Nick inválido. Não insira um nick vazio. Tente novamente.\n\n";
                continue;
            }
        }

        // User is trying to discover /whois someone (get someone's ip address)
        else if (strncmp(input.c_str(), "/whois", 6) == 0) {

            user = input.substr(7, input.length()); // user to be kicked

            // Checks if user is trying to enter an empty nickname
            if (all_of(user.begin(), user.end(), ::isspace)) {
                std::cout << "Nick inválido. Não insira um nick vazio. Tente novamente.\n\n";
                continue;
            }
        }

        // Checking if it's a big message (more than 4096 bytes)
        if (input.size() > 4096)
            bigMessage = true;

        auxCompare = input; // Storing input to check if the sent message was correctly received

        while (input.size() > 4096) {

            memset(buff, 0, 4096); // Cleaning the buffer
            bytesReceived = 0;

            strncpy(buff, input.c_str(), 4095); // Copying 'input' to buff

            buff[4095] = 4; // Adding a '4' just to identify that the message isn't finish yet

            send = ::send(socket, buff, 4096, 0); // Send the message to server

            if (send == -1) {
                std::cout << "Não foi possível enviar ao servidor. Tente novamente.";
                continue;
            }

            if (buff[4095] == 4)
                buff[4095] = 0;

            serverResponse += buff;

            if (bytesReceived == -1)
                std::cout << "Erro ao receber resposta do servidor\n\n";

            input.erase(0, 4095); // Erases first 4096 bytes of auxBuffer
        }

        memset(buff, 0, 4096); // Cleaning buffer to send data to server

        strncpy(buff, input.c_str(), input.size()); // Copying input to buff

        send = ::send(socket, buff, 4096, 0); // Sending bytes from auxBuffer that are in buffer already

        if (send == -1) {

            if (bigMessage == true)
                continue; // If messages if bigger than 4096 chars, we've showed the message already

            cerr << "Mensagem não enviada! Tente novamente!\n";
            continue;
        }

        if (bytesReceived == -1) {
            cerr << "Erro na conexão!";
            break;
        }

        // User wants to quit the application
        if (input.compare("/quit") == 0) {
            *receiveReturn = 0;
            return;
        }
    }
}

int connectToServer() {

    // Creating a client socket to connect to the server
    int socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (socket == -1) {
        std::cout << "Não foi possível criar socket\n";
        return -1;
    }

    // Creating a hint for the server
    int port;
    string ipAddress;
    sockaddr_in server;

    // Getting server's ip address
    std::cout << "Digite o IP: ";
    std::getline(std::cin >> std::ws, ipAddress);

    if (std::cin.eof() && !fsigint) { // checking for errors on reading input
        std::cout << "Até a próxima!!\n";
        return 0;
    }

    // Getting server's port
    std::cout << "Digite a porta: ";
    std::cin >> port;

    if (std::cin.eof() && !fsigint) { // checking for errors on reading input
        std::cout << "Até a próxima!!\n";
        return 0;
    }

    // Adressing some info in order to connect to the server
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &server.sin_addr);

    // Connecting to the server on 'socket'
    int connect = ::connect(socket, (sockaddr *)&server, sizeof(server));

    if (connect == -1) {
        std::cout << "Não foi possível conectar\n";
        return -2;
    }

    string input;          // All user input messages will be stored here
    int bytesReceived = 0; // Variable responsible to store results of recv()

    char buff[4096]; // All messages from the server will be stored here
    string nickname; // Variable to store user nickname
    char bigNickname[67] = "\nNick maior que 50 letras, digite um menor aí Dom Pedro: ";

    std::cout << "\nOlá! ";

    while (true) {

        std::cin.clear();
        input.clear();
        nickname.clear();

        std::cout << "Antes de continuar, escolha um nick. Para inseri-lo digite /nickname seuNick.\n\n";

        std::getline(std::cin >> std::ws, input);

        if (std::cin.eof() && !fsigint) {
            ::send(socket, "Usuário saiu da aplicação", 50, 0); // Sending error
            close(socket);
            return 0;
        }

        // Checks if user is trying to enter his nickname
        else if (input.size() > 10 && strncmp(input.c_str(), "/nickname", 9) == 0) {

            nickname = input.substr(10, input.length());

            // Checks if user entered a nickname full of spaces
            if (all_of(nickname.begin(), nickname.end(), ::isspace)) {
                std::cout << "Nick inválido. Não insira um nick vazio. Tente novamente.\n\n";
                continue;
            }

            // Checking nickname length (must contain less than 50 letters)
            else if (nickname.size() > 50) {
                std::cout << bigNickname; // Printing error message
                continue;
            } else
                break;
        }

        // Checks if user is trying to quit the application
        else if ((input.compare("/quit")) == 0) {
            std::cout << "Você está conectado. Quer sair da aplicação? Digite 's' se sim, ou 'n' caso queira ficar.\n";
            if (quit() == 0) {
                ::send(socket, "Usuário saiu da aplicação", 50, 0); // Sending error
                close(socket);
                return 0;
            } else
                continue;
        }

        // Checks if user is trying to get help
        else if ((input.compare("/help")) == 0) {
            help();
            continue;
        } else {
            cout << "Comando inválido. Se precisar, digite /help ou saia da aplicação com /quit.\n";
        }
    }

    ::send(socket, nickname.c_str(), 50, 0); // Sending nickname

    string ch, user, retMessage;
    int receiveReturn = 2;

    cout << "\nMuito bom! :)\n";

    while (true) {

        // Trying to get a channel name to join
        while (true) {

            std::cout << "Comandos que você pode utilizar:\n\n"
                      << "1) /join *nomeDoCanal - (substitua o * por # ou &)\n"
                      << "2) /list para mostrar os canais rodando neste servidor\n"
                      << "4) /quit ou apert ctrl+D no teclado - desconecta do servidor\n"
                      << "5) /help - para obter ajuda, conectado ou não a um servidor\n\n"
                         "Divirta-se!\n\n";

            input.clear();
            std::cin.clear();
            memset(buff, 0, 4096);

            std::getline(std::cin >> std::ws, input);

            if (std::cin.eof() && !fsigint) {
                std::cout << "Até a próxima!\n";
                close(socket);
                return 0;
            }

            // Checks if client is trying to list available channels
            else if (input.compare("/list") == 0) {
                std::cout << "\nCanais disponíveis:\n\n";
                strcpy(buff, input.c_str());

                ::send(socket, buff, 26, 0);

                bytesReceived = recv(socket, buff, 4096, 0);

                int i = 0;
                string canal = buff;
                while (strcmp(buff, "terminou") != 0) {
                    i++;
                    std::cout << "\t" << i << ") " << canal << "\n";
                    bytesReceived = recv(socket, buff, 4096, 0);
                    canal = buff;
                }

                memset(buff, 0, 4096);
                std::cout << "\n";
                break;

            }

            // Checks if user is trying to join a channel
            else if (input.size() > 6 && (strncmp(input.c_str(), "/join", 5) == 0)) {

                ch = input.substr(6, input.length());

                // Checks if user entered a channel name full of spaces
                if (all_of(ch.begin(), ch.end(), ::isspace)) {
                    std::cout << "Nome de canal inválido. Por favor não insira um nome vazio. Tente novamente.\n\n";
                    continue;
                }

                else if (input[6] != '&' && input[6] != '#') {
                    std::cout << "Nome de canal inválido. Tente novamente.\n\n";
                    continue;
                }

                // Checks if user enterd a channel name bigger than 20 characters
                else if (ch.length() > 200) {
                    std::cout << "Por favor, utilize um nome de canal começando com '&' ou '#' e menor que 200 caracteres.\n";
                    fflush(stdin);
                    std::cin.clear();
                    ch.clear();
                    continue;
                }

                else {

                    std::cout << "\nVocê está tentando entrar no canal \"" << ch.c_str() << "\"\n";

                    ::send(socket, input.c_str(), 26, 0); // sending the join command

                    bytesReceived = recv(socket, buff, 86, 0); // Checking if everything went right while adding user to channel

                    if (bytesReceived != 86) {
                        std::cout << "\nErro! É necessário primeiro estar conectado a um servidor.\n";
                    }

                    if (bytesReceived > 0) {

                        retMessage = buff;

                        if (retMessage.compare("Já tem um membro com esse nick neste canal, escolha outro pro favor!") == 0) {
                            std::cout << retMessage.c_str();
                            continue;
                        }

                        else {
                            std::cout << "Você foi adicionado ao canal " << ch.c_str() << ".\n";
                            // break;
                        }
                    } else {
                        std::cout << "\nErro enquanto tentava receber mensagem de confirmação. Abortando operação...\n";
                        return -1;
                    }

                    bytesReceived = recv(socket, buff, 46, 0); // Receiving welcome message from server

                    std::cout << "\n" + string(buff, bytesReceived); // Printing welcome message from server

                    bytesReceived = recv(socket, buff, 56, 0); // Receiving confirmation message from server

                    if (bytesReceived > 0) {

                        std::cout << "\n" + string(buff, bytesReceived) << "\n"; // Displaying received message

                        thread sending(threadSend, socket, &receiveReturn, nickname);
                        thread receiving(threadReceive, socket, nickname, &receiveReturn);

                        sending.join();
                        receiving.join();
                    }

                    close(socket);

                    break;
                }
            }

            // Checks if user is trying to quit the application
            else if ((input.compare("/quit")) == 0) {
                std::cout << "Voçê está conectado. Deseja sair da aplicação? Digite 's' se sim ou 'n' caso deseje ficar.\n";

                if (quit() == 0) {
                    close(socket);
                    return 0;
                } else
                    continue;
            }

            // Checks if user is trying to get help
            else if ((input.compare("/help")) == 0) {
                help();
                continue;
            }

            // Invalid option
            else {
                cout << "Não é válido. ";
                continue;
            }
        }

        // User kicked from channel
        if (receiveReturn == 1)
            continue;

        // User exited the application
        else if (receiveReturn == 0)
            break;
    }

    return 0;
}

int main(int argc, char const *argv[]) {

    // Checking for SIGNINT
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    std::cout << "Bem vinde a aplicação!\n\n";

    help();

    while (true) {

        string input; // string to receive user input

        // Clearing stdin and std::cin buff and set fsigint flag to false to indicate we havent receive any SIGINT
        fflush(stdin);
        std::cin.clear();
        fsigint = false;

        std::getline(std::cin >> std::ws, input);

        // Checking for errors on reading input or CTRL+C
        if (std::cin.eof() && !fsigint) {
            std::cout << "Até a próxima!!\n";
            return 0;
        }

        // Checking if client is trying to input a nickname
        else if (strncmp(input.c_str(), "/nickname", 9) == 0)
            std::cout << "Você precisa se conectar a um servidor antes. Tente usar /connect para se conectar.\n";

        // Checking if client is trying to connect to a channel
        else if (input.compare("/connect") == 0) {

            int value = connectToServer(); // getting result of attempt to connect this client to server

            if (value == -1 || value == -2)
                std::cout << "Erro ao estabelecer conexão com o servidor. Tente novamente.\n\n";

            else {
                std::cout << "Obrigado por usar esta aplicação. Até a próxima.\n";
                break;
            }
        }

        // Checking if client is trying to test connection with server
        else if (input.compare("/ping") == 0)
            std::cout << "Você precisa se conectar a um servidor antes. Tente usar /connect para se conectar.\n";

        // Checking if client is trying to quit the chat
        else if (input.compare("/quit") == 0) {
            std::cout << "Voçê não está conectado. Deseja sair da aplicação? Digite 's' se sim ou 'n' caso deseje ficar\n";

            if (quit() == 0)
                return 0;
            else
                continue;
        }

        // Checking if client is trying to get help
        else if (input.compare("/help") == 0)
            help();

        // Checking if client is trying to input an invalid command
        else if (!fsigint && !input.empty())
            std::cout << "Comando inválido. Por favor tente novamente. Se precisar de ajuda digite /help.\n";
    }

    return 0;
}
