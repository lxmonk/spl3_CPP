/*
 * C_STOMP_client.h
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#ifndef C_STOMP_CLIENT_H_
#define C_STOMP_CLIENT_H_

#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/ThreadPool.h"
#include "Poco/RunnableAdapter.h"
#include <string>
#include <iostream>
#include "ClientWrapper.h"


enum STOMP_command {
	NOCOMMAND = -1,
	CONNECT = 0,
	CONNECTED,
	DISCONNECT,
	SUBSCRIBE,
	UNSUBSCRIBE,
	SEND,
	MESSAGE,
	ACK
};

class C_STOMP_client {
private:
	std::string host;
	Poco::UInt16 port;
	Poco::Net::SocketAddress sa;
	Poco::Net::StreamSocket socket;
	Poco::Net::SocketStream str;
	Poco::ThreadPool pool;
	ClientWrapper client;
	bool is_connected;
	std::string session_id;

public:
	C_STOMP_client();

	C_STOMP_client(ClientWrapper _client);

			C_STOMP_client(std::string _host, Poco::UInt16 _port,
					ClientWrapper _client);

	virtual ~C_STOMP_client();

	void send(std::string msg);

	/**
	 * Receive a message from the STOMP server.
	 * This is blocking.
	 */
	void receive();

	Poco::UInt16 get_STOMP_command(char* buffer);

	Poco::UInt16 str2command(std::string command);
	void connect();
	void connected(char* buffer, Poco::UInt16 len);
	void message(char* buffer, Poco::UInt16 len);
//	void ack(char* buffer, Poco::UInt16 len);
	bool need_ack(char *buffer, Poco::UInt16 len);
	void parse_trading_message(std::string raw_input, STOMP_command *command);
	void handle_trading_message(std::string input);


};

#endif /* C_STOMP_CLIENT_H_ */

