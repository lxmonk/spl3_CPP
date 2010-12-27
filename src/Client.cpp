/*
 * Client.cpp
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#include "Client.h"

Client::Client() {
	// TODO Auto-generated constructor stub

}
Client::Client(std::string name) :
	mName(name), mCash(0), mObligations(0), mBrokerName() {
}

void Client::SetReplPtr() {
	boost::shared_ptr<Repl> mpRepl(new Repl(this));
}

Client::~Client() {
	// TODO Auto-generated destructor stub
}

/*
 * std::string mName;
 boost::shared_ptr<CStompClient> mpStompClient;
 Poco::HashMap<std::string, long> mPortfolio;
 double mCash;
 double mObligations;
 std::string mBrokerName;
 std::list<order> mPendingOrders;
 boost::shared_ptr<Repl> mpRepl;
 */
