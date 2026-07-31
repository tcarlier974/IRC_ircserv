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
		int Nuser;

    public:
        Channel(std::string name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
        ~Channel();

        std::string GetName() const;
		std::string GetMemberList();
        
        void AddMember(Client *client);
        void RemoveMember(Client *client);

        void SetTopic(std::string topic);
        std::string GetTopic() const;

		bool IsOPbyNick(std::string nick);
		void SetOP(std::string nick);
		void DeOP(std::string nick);
        
        void BroadcastMessage(std::string message, Client *exclude = NULL);
};

#endif