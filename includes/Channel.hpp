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
		std::string _password;
		int			_limit;

    public:
        Channel(std::string name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
        ~Channel();

        std::string GetName() const;
		std::string GetMemberList();
        
        void AddMember(Client *client, std::string pass = "");
        void RemoveMember(Client *client);

        void SetTopic(std::string topic);
        std::string GetTopic() const;

		bool IsOPbyNick(std::string nick);
		bool SetOP(std::string nick);
		bool DeOP(std::string nick);
		void SetPassword(std::string pass);
		void SetLimit(int limit);
		int GetLimit() const;

		bool CheckPassword(std::string pass) const;
        
        void BroadcastMessage(std::string message, Client *exclude = NULL);
};

#endif