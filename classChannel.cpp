#include "classChannel.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int id;

// All documentation of the functions below are on classChannel.h

Channel::Channel(string name, string nickname, int admin, char *ip) {

    struct Member aux;

    // Assigning info of new member
    aux.name = nickname;
    aux.socket = admin;
    aux.isMuted = false;
    aux.host.assign(ip);

    // Assigning info of the new channel
    this->name = name;
    this->members.push_back(aux);
    this->admin = aux;
    this->numConnections = 0;
}

int Channel::findMember(string nickname) {

    // Checking if member is already in this channel
    if (find_if(this->members.begin(), this->members.end(), find_name(nickname)) != this->members.end()) {
        // cout << "\nMember already in this channel.\n";
        return 1;
    } else
        return 0;
}

vector<Member>::iterator Channel::getMembersIterator(string nickname) {
    return find_if(this->members.begin(), this->members.end(), find_name(nickname));
}

int Channel::addUser(string nickname, int clientSocket, char *ip) {

    struct Member aux;

    // Assigning new member's info
    aux.name = nickname;
    aux.socket = clientSocket;
    aux.isMuted = false;
    aux.host.assign(ip);

    // Checking if member is already in this channel
    if (find_if(this->members.begin(), this->members.end(), find_name(nickname)) != this->members.end()) {
        cout << "\nThere's already a member with nickname " << nickname << " on channel " << this->name << ".\n";
        return 0;
    }
    this->members.push_back(aux); // adds new client to the end of members list of this channel
    return 1;
}

int Channel::removeUser(string nickname) {

    vector<Member>::iterator it;

    it = find_if(this->members.begin(), this->members.end(), find_name(nickname));

    // Checking if member is in this channel
    if (it != this->members.end()) {
        this->members.erase(it); // deleting client from channel members vector
        this->numConnections--;
        if (this->numConnections != 0 && !this->verifyAdmin())
            this->changeAdmin();
        return 1;
    }

    // Member is not on this channel. Returning 0 to indicate error.
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

    // Checking if member is in this channel
    if (it != this->members.end()) {
        return (*it).host;
    }
    return "not found";
}

int Channel::isMuted(string nickname) {

    // Checking if user exists on this channel
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

    // Checking if member exists. If so, mute him/she.
    if (this->findMember(nickname)) {
        vector<Member>::iterator it;

        it = this->getMembersIterator(nickname);

        (*it).isMuted = true;
        return 1;
    }

    return 0;
}

int Channel::unmuteUser(string nickname) {

    // Checking if member exists. If so, unmute him/she.
    if (this->findMember(nickname)) {
        vector<Member>::iterator it;

        it = this->getMembersIterator(nickname);

        (*it).isMuted = false;
    }

    return 0;
}