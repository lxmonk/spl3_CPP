/*
 * Client.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "C_STOMP_client.h"
#include "Poco/HashMap.h"
#include <list>

struct order {
	std::string share_name;
	long amount;
	int order_id;
};

class Client {
private:
	std::string name;
	C_STOMP_client conn;
	Poco::HashMap<std::string, long> portfolio;
	double cash;
	double obligations;
	std::string broker;
	std::list<order> pending;


public:
	Client();
	virtual ~Client();
	std::string connect();
	bool disconnect();
	bool commit_order(order);
	void check_prices();
	void deal_struck();
	void close_day();


};

#endif /* CLIENT_H_ */
