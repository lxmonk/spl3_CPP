/*
 * ClientWrapper.cpp
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#include "ClientWrapper.h"
#include "Poco/Mutex.h"

using boost::shared_ptr;
using Poco::Mutex;

ClientWrapper::ClientWrapper() {
	// TODO Auto-generated constructor stub

}
ClientWrapper::ClientWrapper(boost::shared_ptr<Client> _client) {
	mpClient = _client;
}

ClientWrapper::~ClientWrapper() {
	// TODO Auto-generated destructor stub
}

ClientWrapper& ClientWrapper::operator=(const ClientWrapper& _cw) {
	if (!(this == &_cw)) { // protection from self copy
		mpClient = _cw.mpClient;
	}
	return *this;
}

shared_ptr<Client> ClientWrapper::getClientPtr() {
//	Mutex::ScopedLock lock(mutex);
	return mpClient;

}
