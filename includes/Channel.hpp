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
        std::list<Client*> _members;
        std::list<Client*> _operators;
        std::list<Client*> _invited;
        bool _inv_only;
		int Nuser;
		std::string _password;
		int			_limit;
		bool		_topic_onlyOP;

    public:
        Channel(std::string name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
        ~Channel();

        std::string GetName() const;
		std::string GetMemberList();
        
        bool IsEmpty() const;

        bool AddMember(Client *client, std::string pass = "");
        void RemoveMember(Client *client);

        void RemoveInv(Client *client);

        void SetTopic(std::string topic);
        std::string GetTopic() const;
        void SetInvOnly(bool inv_only);
        bool getInv_only() const;
        std::list<Client*> GetInvited() const;
        void setInvited(Client *client);

		bool IsOPbyNick(std::string nick);
		bool SetOP(std::string nick);
		bool DeOP(std::string nick);
		void SetPassword(std::string pass);
		void SetLimit(int limit);
		int GetLimit() const;
		void SetTopicOnlyOP(bool topic_onlyOP);
		bool getTopicOnlyOP() const;
        bool IsMember(Client *client) const;

		bool CheckPassword(std::string pass) const;
        
        void BroadcastMessage(std::string message, Client *exclude = NULL);
        bool operator==(const Channel& other) const;
};

#endif