/*
 * CStompClient.h
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

enum StompCommand {
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

class CStompClient {
private:
	std::string mHost;
	Poco::UInt16 mPort;
	Poco::Net::SocketAddress sa;
	Poco::Net::StreamSocket mSocket;
	Poco::Net::SocketStream str;
	Poco::ThreadPool pool;
	ClientWrapper client;
	bool isConnected;
	std::string mSessionId;
	std::string mClientName;

public:
	CStompClient();

	CStompClient(ClientWrapper _client);

			CStompClient(std::string _host, Poco::UInt16 _port,
					ClientWrapper _client);

	virtual ~CStompClient();

	void Send(std::string msg);

	/**
	 * Receive a MessageRecv from the STOMP server.
	 * This is blocking.
	 */
	void ReceiveLoop();

	Poco::UInt16 StompCommandToBuf(char* buffer);

	Poco::UInt16 Str2Command(std::string command);
	void Connect();
	void ConnectedRecv(char* buffer, Poco::UInt16 len);
	void MessageRecv(char* buffer, Poco::UInt16 len);
	//	void ack(char* buffer, Poco::UInt16 len);
	bool IsAckNeeded(char *buffer, Poco::UInt16 len);
	void ParseTradingMessage(std::string raw_input, StompCommand *command);
	void HandleTradingMessage(std::string input);
	void ClearBuf(char* buf, int len);

};

#endif /* C_STOMP_CLIENT_H_ */

