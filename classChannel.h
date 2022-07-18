#include <netdb.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

// Struct para um membro no canal
struct Member {

    int socket;
    string name;
    bool isMuted;
    string host;

    bool operator()(Member &m) const {
        if (name == m.name && socket == m.socket)
            return true;
        else
            return false;
    }
};

// Struct usado como busca do membro no vetor <Member>
struct find_name {
    string name;
    find_name(string name) : name(name) {}
    bool operator()(const Member &m) const {
        return name == m.name;
    }
};

class Channel {

public:
    Member admin;       // Administrador do canal
    std::string name;   // Nome do canal (máximo 200 chars)
    int numConnections; // Número de membors conectados no canal

    std::vector<Member> members; // Vetor de membros

    // Construtor da classe Channel (Canal)
    Channel(string name, string nickname, int admin, char host[NI_MAXHOST]);

    // Função para buscar um usuário no canal utilizando seu socket como chave
    int findMember(string nickname);

    // Função para buscar um usuário no canal usando seu nickname como chave e retornando seu iterador
    vector<Member>::iterator getMembersIterator(string nickname);

    // Função para adicionar um usuário no canal adicionando seu socket na lista de membros
    int addUser(string nickname, int clientSocket, char host[NI_MAXHOST]);

    // Função para remover um usuário no canal removendo seu socket na lista de membros
    int removeUser(string nickname);

    // Função para pegar o IP do usuário
    string getUserHost(string nickname);

    // Função para pegar o primeiro membro da lista do canal
    Member getFirstMember();

    // Função para verify if the admin of the channel still the same since the creation of this channel
    bool verifyAdmin();

    // Função para pegar o admin do canal
    Member getAdmin();

    // Função para mudar o admin do canal
    void changeAdmin();

    // Função para aumentar o número de membros conectados depois de um novo membro entrar
    void increaseNumConnections();

    // Função para mutar um usuário 
    int muteUser(string nickname);

    // Função para desmutar um usuário 
    int unmuteUser(string nickname);

    // Função para checar se um usuário está mutado
    int isMuted(string nickname);
};
