/*
 * ClientWrapper.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef CLIENTWRAPPER_H_
#define CLIENTWRAPPER_H_
#include "Client.h"
#include "Poco/Mutex.h"
#include "boost/shared_ptr.hpp"

//class Client;
using Poco::Mutex;

class ClientWrapper {

public:
	ClientWrapper();
	ClientWrapper(boost::shared_ptr<Client> client);
	virtual ~ClientWrapper();
	ClientWrapper(const ClientWrapper& clientWrapper) {
		*this = clientWrapper;
	}
	ClientWrapper& operator=(const ClientWrapper& clientWrapper);
	boost::shared_ptr<Client> getClientPtr();
	Mutex mutex;

private:
	boost::shared_ptr<Client> mpClient;
};

#endif /* CLIENTWRAPPER_H_ */
