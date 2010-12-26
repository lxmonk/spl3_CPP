/*
 * ClientWrapper.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef CLIENTWRAPPER_H_
#define CLIENTWRAPPER_H_

#include "Poco/Mutex.h"
class Client;
using Poco::Mutex;

class ClientWrapper {

public:
	ClientWrapper();
	ClientWrapper(Client *_client);
	virtual ~ClientWrapper();
	ClientWrapper(const ClientWrapper& _cw) {
		*this = _cw;
	}
	ClientWrapper& operator=(const ClientWrapper& _cw);
	Client* getClient();
	Mutex mutex;

private:
	Client *client;
};

#endif /* CLIENTWRAPPER_H_ */
