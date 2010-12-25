/*
 * Client.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "C_STOMP_client.h"

class Client {
private:
	C_STOMP_client conn;


public:
	Client();
	virtual ~Client();
};

#endif /* CLIENT_H_ */
