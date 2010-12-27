/*
 * Client.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef CLIENT_H_
#define CLIENT_H_

enum StompCommand {
	NO_STOMP_COMMAND = -1,
	CONNECT = 0,
	STOMP_CONNECTED,
	DISCONNECT,
	SUBSCRIBE,
	UNSUBSCRIBE,
	SEND,
	MESSAGE,
	ACK
};

enum TradingCommand {
	NONE = -1,
	LOGIN = 0,
	TRADING_CONNECTED,
	CONNECT_FAILED,
	BUY_ORDER,
	SELL_ORDER,
	ERROR,
	CLOSE_DAY,
	TRADING_DISCONNECT,
	TRADING_DISCONNECTED,
	NEW_DAY,
	PRICES,
	DEAL_SOLD,
	DEAL_BOUGHT,
	STATE
//only from user
};

//#include "CStompClient.h"
#include "Poco/HashMap.h"
#include "boost/shared_ptr.hpp"
#include <list>
#include <iostream>
#include <stdlib.h>
#include "Poco/StringTokenizer.h"

class CStompClient;
class Repl;

struct order {
	std::string share_name;
	long amount;
	int order_id;
};

class Client {
private:
	std::string mName;
	boost::shared_ptr<CStompClient> mpStompClient;
	Poco::HashMap<std::string, long> mPortfolio;
	double mCash;
	double mObligations;
	std::string mBrokerName;
	std::list<order> mPendingOrders;
//	boost::shared_ptr<Repl> mpRepl;

public:
	Client();
	Client(std::string);
	virtual ~Client();
	void SetReplPtr();
	bool Connect(std::string broker);
	bool Disconnect();
	bool CommitOrder(order);
	void CheckPrices();
	void PostDealUpdate();
	void CloseDay();
	std::string GetName() {
		return mName;
	}
	bool Login(const std::string& clientName);
	bool Buy(const std::string&, long int, double);
	bool Sell(const std::string&, long int, double);
	void ReportState();
	void StartRepl() {

	}
};

class Repl {
public:
	Repl() {
	}
	Repl(Client* client) :
		mpCleint(client) {
	}
	virtual ~Repl();

	void ParseNextLine() {
		std::string input;
		std::getline(std::cin, input);
		Poco::StringTokenizer line(input, " ");
		std::string command(line[0]);
		size_t len = line.count();
		if (command == "login" && len == 2)
			mpCleint->Login(line[1]);
		else if (command == "disconnect" && len == 1)
			mpCleint->Disconnect();
		else if (command == "prices" && len == 1)
			mpCleint->CheckPrices();
		else if (command == "buyOrder" && len == 4)
			mpCleint->Buy(line[1], strtol(line[2].c_str(), NULL, 10), strtod(
					line[3].c_str(), NULL));
		else if (command == "sellOrder " && len == 4)
			mpCleint->Sell(line[1], strtol(line[2].c_str(), NULL, 10), strtod(
					line[3].c_str(), NULL));
		else if (command == "closeDay" && len == 0)
			mpCleint->CloseDay();
		else if (command == "state" && len == 0)
			mpCleint->ReportState();
		else if (command == "BYE")
			return;
		else {
			std::cout << "Error: \"" << input << "\""
					<< " is not a valid command invocation. This REPL is case-sensitive and"
					<< " arg-sensitive. Use the exact number of arguments."
					<< std::endl;
		}
	}

private:
	boost::shared_ptr<Client> mpCleint;
};

#endif /* CLIENT_H_ */
