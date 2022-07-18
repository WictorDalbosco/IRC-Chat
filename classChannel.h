#include <netdb.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

// Struct representing a member in a channel
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

// Struct to use as search condition of Member in a vector<Member>
struct find_name {
    string name;
    find_name(string name) : name(name) {}
    bool operator()(const Member &m) const {
        return name == m.name;
    }
};

class Channel {

public:
    Member admin;       // Adminin of the channel
    std::string name;   // Name of channel (max of 200 chars)
    int numConnections; // Num of connected members on this channel

    std::vector<Member> members; // Vector of members

    // Constructor of Channel class
    Channel(string name, string nickname, int admin, char host[NI_MAXHOST]);

    // Method to search by an user in a channel using its socket as key
    int findMember(string nickname);

    // Method to search by an user in a channel using its nickname as key and return its iterator
    vector<Member>::iterator getMembersIterator(string nickname);

    // Method to add an user to a channel by adding its socket to members list
    int addUser(string nickname, int clientSocket, char host[NI_MAXHOST]);

    // Method to remove an user from a channel by removing its socket from members list
    int removeUser(string nickname);

    // Method to get user ip
    string getUserHost(string nickname);

    // Method to get the first member on channel list of members
    Member getFirstMember();

    // Method to verify if the admin of the channel still the same since the creation of this channel
    bool verifyAdmin();

    // Method to get the admin of the channel
    Member getAdmin();

    // Method to set a new admin of the channel
    void changeAdmin();

    // Method to increase the num of connected members of this channel before a member joined
    void increaseNumConnections();

    // Method to mute an user given its nickname
    int muteUser(string nickname);

    // Method to unmute an user given its nickname
    int unmuteUser(string nickname);

    // Method to check if an user is muted or not
    int isMuted(string nickname);
};