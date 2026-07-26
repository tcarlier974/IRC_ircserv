#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "Client.hpp"
# include <vector>
# include <string>

class Channel
{
    private:
        std::string _name;
        std::string _topic;
        std::vector<Client*> _members;
        std::vector<Client*> _operators;

    public:
        Channel(std::string name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
        ~Channel();

        std::string GetName() const;
        
        void AddMember(Client *client);
        void RemoveMember(Client *client);
        
        void BroadcastMessage(std::string message, Client *exclude = NULL);
};

#endif