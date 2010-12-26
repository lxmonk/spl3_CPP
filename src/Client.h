/*
 * Client.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef CLIENT_H_
#define CLIENT_H_

//#include "CStompClient.h"
#include "Poco/HashMap.h"
#include "boost/shared_ptr.hpp"
#include <list>
class CStompClient;

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


public:
	Client();
	Client(std::string _name) : mName(_name) {}
	virtual ~Client();
	bool Connect(std::string *broker);
	bool Disconnect();
	bool CommitOrder(order);
	void CheckPrices();
	void PostDealUpdate();
	void CloseDay();
	std::string GetName() { return mName;}



};

#endif /* CLIENT_H_ */
