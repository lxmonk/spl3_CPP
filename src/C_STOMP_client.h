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
#include <list>
#include "ClientWrapper.h"

/*enum StompCommand {
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
	STATE //only from user
};*/

class CStompClient {
private:
	std::string mHost;
	Poco::UInt16 mPort;
	Poco::Net::SocketAddress mSocketAddress;
	Poco::Net::StreamSocket mSocket;
	Poco::Net::SocketStream mSocketStream;
	Poco::ThreadPool mThreadPool;
	ClientWrapper mClientWrapper;
	bool IsConnected;
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
	void ParseTradingMessage(std::string raw_input, TradingCommand* command, std::list<std::string>* args);
	void HandleTradingMessage(std::string input);
	void ClearBuf(char* buf, int len);

};

#endif /* C_STOMP_CLIENT_H_ */

