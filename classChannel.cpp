#include "classChannel.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int id;

// Documentação das funções estão no arquivo classChannel.h

Channel::Channel(string name, string nickname, int admin, char *ip) {

    struct Member aux;

    // Atribuindo infos de um novo membro
    aux.name = nickname;
    aux.socket = admin;
    aux.isMuted = false;
    aux.host.assign(ip);

    // Atribuindo infos de um novo canal
    this->name = name;
    this->members.push_back(aux);
    this->admin = aux;
    this->numConnections = 0;
}

int Channel::findMember(string nickname) {

    // Checando se membro já está no canal
    if (find_if(this->members.begin(), this->members.end(), find_name(nickname)) != this->members.end()) {
        // cout << "\nMembro já está no canal.\n";
        return 1;
    } else
        return 0;
}

vector<Member>::iterator Channel::getMembersIterator(string nickname) {
    return find_if(this->members.begin(), this->members.end(), find_name(nickname));
}

int Channel::addUser(string nickname, int clientSocket, char *ip) {

    struct Member aux;

    // Atribuindo infos de um novo membro
    aux.name = nickname;
    aux.socket = clientSocket;
    aux.isMuted = false;
    aux.host.assign(ip);

    // Checando se membro já está no canal
    if (find_if(this->members.begin(), this->members.end(), find_name(nickname)) != this->members.end()) {
        cout << "\nJá tem um membro com o nome " << nickname << " no canal " << this->name << ".\n";
        return 0;
    }
    this->members.push_back(aux); // adiciona novo cliente ao final da lista de membros do canal
    return 1;
}

int Channel::removeUser(string nickname) {

    vector<Member>::iterator it;

    it = find_if(this->members.begin(), this->members.end(), find_name(nickname));

    // Checando se membro já está neste canal
    if (it != this->members.end()) {
        this->members.erase(it); // removendo cliente da lista de memebros do canal 
        this->numConnections--;
        if (this->numConnections != 0 && !this->verifyAdmin())
            this->changeAdmin();
        return 1;
    }

    // Membro não está no canal. Retorna 0 para indicar erro.
    return 0;
}

bool Channel::verifyAdmin() {
    if (this->admin(this->members.front()))
        return true;
    else
        return false;
}

Member Channel::getAdmin() {
    return this->admin;
}

void Channel::changeAdmin() {
    this->admin = this->members.front();
}

void Channel::increaseNumConnections() {
    this->numConnections++;
}

string Channel::getUserHost(string nickname) {

    vector<Member>::iterator it;

    it = find_if(this->members.begin(), this->members.end(), find_name(nickname));

    // Checando se membro já está neste canal
    if (it != this->members.end()) {
        return (*it).host;
    }
    return "não encontrado";
}

int Channel::isMuted(string nickname) {

    // Checando se o usuário existe neste canal
    if (this->findMember(nickname)) {
        vector<Member>::iterator it;

        it = this->getMembersIterator(nickname);

        if ((*it).isMuted)
            return 1;
        else
            return 0;
    }

    else
        return -1;
}

int Channel::muteUser(string nickname) {

    // Checa se membro existe, e então o muta.
    if (this->findMember(nickname)) {
        vector<Member>::iterator it;

        it = this->getMembersIterator(nickname);

        (*it).isMuted = true;
        return 1;
    }

    return 0;
}

int Channel::unmuteUser(string nickname) {

    // Checa se membro existe, e então o desmuta.
    if (this->findMember(nickname)) {
        vector<Member>::iterator it;

        it = this->getMembersIterator(nickname);

        (*it).isMuted = false;
    }

    return 0;
}
