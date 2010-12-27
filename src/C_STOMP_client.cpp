/*
 * CStompClient.cpp
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#include "C_STOMP_client.h"
#include <iostream>
#include "Poco/Mutex.h"
#include "boost/shared_ptr.hpp"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

CStompClient::CStompClient() :
	mHost("localhost"), mPort(61613), mSocketAddress(mHost, mPort), mSocket(mSocketAddress), mSocketStream(mSocket),
			mClientName("NONAME") {

}

CStompClient::CStompClient(ClientWrapper _client) :
	mHost("localhost"), mPort(61613), mSocketAddress(mHost, mPort), mSocket(mSocketAddress), mSocketStream(mSocket),
			mClientName((_client.getClientPtr())->GetName()) {
	mClientWrapper = _client; //FIXME chnage to mClientWrapper(_client) after implementing
	//	Client. also in the next ctor.
	//	RunnableAdapter<CStompClient> sender;
	//	RunnableAdapter<CStompClient> receiver(*this, &CStompClient::ReceiveLoop);
	//	mThreadPool.start(receiver);
}

CStompClient::CStompClient(string _host, UInt16 _port,
		ClientWrapper _client) :
	mHost(_host), mPort(_port), mSocketAddress(mHost, mPort), mSocket(mSocketAddress), mSocketStream(mSocket) {
	mClientWrapper = _client;
	//	RunnableAdapter<CStompClient> sender;
	//	RunnableAdapter<CStompClient> receiver(*this, &CStompClient::ReceiveLoop);
	//	mThreadPool.start(receiver);
}

CStompClient::~CStompClient() {
	// TODO Auto-generated dtor stub
}

void CStompClient::Connect() {
	try {
		mSocket.connect(mSocketAddress);
	} catch (Exception e) {
		//TODO: log this exception (should not happen)
	}
	string msg("CONNECT");
	msg += '\32';
	msg += '\n';
	msg += "login:" + mClientName + "\n";
	msg += string("passcode: dontcare");
	msg += "\n";
	msg += "\n";
	//	msg += '\0';
	Send(msg);
	cout << "trying to connect (STOMP)" << endl; //FIXME: move to log.
	char buf[1024];
	ClearBuf(buf, 1024);
	UInt16 len = -1;
	len = mSocket.receiveBytes(&buf, 1023); //recieve up to 1023 chars.
	UInt16 command = StompCommandToBuf(buf);
	if (command == NO_STOMP_COMMAND) { //this is not a legal command, ignore it.
		cout << "bad command. C_STOMP_client.cpp:67\n";//TODO: add some logging to track this error.
	} else if (command == STOMP_CONNECTED) {
		ConnectedRecv(buf, len);
	} else { // these commands are illegal for the mClientWrapper to ReceiveLoop
		// ignore them. TODO: add some logging to track this event.
	}
}
/**
 * Send this MessageRecv to the STOMP server.
 * This function is in charge of translating the MessageRecv to the trading protocol
 * and later to the STOMP protocol. */
void CStompClient::Send(string msg) {
	mSocketStream << msg << '\0';
	mSocketStream.flush();
	//	mSocketStream << '\0';
	//	mSocketStream.flush();
}

void CStompClient::ReceiveLoop() {
	cout << "starting to recieve\n";
	char buffer[1024];
	for (UInt16 i; i < 1024; i++) {
		buffer[i] = '~'; //this will be the "null" character
	}
	UInt16 len = -1;
	while (len != 0) { //0 means a greacefull shutdown on the server side.
		//this is where the program should be most of the time, waiting for input.
		len = mSocket.receiveBytes(&buffer, 1023); //recieve up to 1023 chars.

		buffer[len] = '~'; //return "my" null char
		/* //in case decoding is needed, uncomment this block.
		 for (UInt16 chr = 0; chr < len; chr++) {
		 buffer[chr] = decode(buffer[chr]);
		 }*/// end of commented block
		cout << "got message\n"; //FIXME: delete this
		UInt16 command = StompCommandToBuf(buffer);
		if (command == NO_STOMP_COMMAND) { //this is not a legal command, ignore it.
			break; //TODO: add some logging to track this error.
		} else if (command == STOMP_CONNECTED) {
			ConnectedRecv(buffer, len);
			break;
		} else if (command == MESSAGE) {
			MessageRecv(buffer, len);
			break;
			//		} else if (command == ACK) {
			//			ack(buffer, len);
		} else { // these commands are illegal for the mClientWrapper to ReceiveLoop
			break; // ignore them. TODO: add some logging to track this event.
		}
		//
	}

}

void CStompClient::ConnectedRecv(char* buffer, Poco::UInt16 len) {
	cout << "connection established" << endl;
	IsConnected = true;
	string header;
	// the mSessionId identifier will start after the "session: " header
	// ( |STOMP_CONNECTED\n| = 10, |session: | = 9)
	UInt16 c;
	for (c = 10; c < len && buffer[c] != ':'; c++) {
		header += buffer[c];
	}
	c++; // leave the ':' behind.
	while (buffer[c] == ' ') c++; // go over whitespaces
	if (header == "session") { //no session-id is given
		for (; c < len && buffer[c] != '\n' && buffer[c] != ' '; c++) {
			mSessionId += buffer[c];
		}
	}
	// start the receiver as a separate thread
	RunnableAdapter<CStompClient> receiver(*this, &CStompClient::ReceiveLoop);
	mThreadPool.start(receiver);
}
void CStompClient::MessageRecv(char* buffer, Poco::UInt16 len) {
	string msg(buffer, len);
	string content("");
	size_t start = msg.find("\n\n");
	if (start != string::npos) { // this MessageRecv contains content. it's only 1 line - get it!
		start += 2; //leave the "\n\n" behind.
		for (; start < len && buffer[start] != '\n'; start++) {
			content += buffer[start];
		}
		content += '\n';//the trading protocol requires '\n' at the end.
		//TODO: check that this does not miss anything.
	}
	HandleTradingMessage(content);

}
//void CStompClient::ack(char* buffer, Poco::UInt16 len) {
//}

void CStompClient::HandleTradingMessage(string raw_content) {
	TradingCommand command(NONE);
	list<string> args;
	//parse command to actions
	ParseTradingMessage(raw_content, &command, &args);
//	switch (command) {
//		case :
//
//			break;
//		default:
//			break;
//	}
	Poco::Mutex::ScopedLock lock(mClientWrapper.mutex);
	mClientWrapper.getClientPtr()/*->make some changes */;
	// Here the Mutex is implicitly released and the Client is "unlocked".
}
void CStompClient::ParseTradingMessage(string content,
		TradingCommand* command, list<string>* args) {

}
UInt16 CStompClient::StompCommandToBuf(char *buffer) {
	string command("");
	UInt16 len = 13; // longest (relevant) STOMP command is |UNSUBSCRIBE| = 11
	for (UInt16 c = 0; c < len && buffer[c] != '\n'; c++) {
		command += buffer[c];
	}
	UInt16 ret = Str2Command(command);
	return ret; //This might be -1, which is an error!

}

bool CStompClient::IsAckNeeded(char *buffer, UInt16 len) {
	string msg(buffer, len);
	size_t found = msg.find("ack: client");
	if (found != string::npos)
		return true;
	return false;
}

UInt16 CStompClient::Str2Command(string command) {
	if (command == "CONNECT")
		return CONNECT;
	else if (command == "CONNECTED")
		return STOMP_CONNECTED;
	else if (command == "DISCONNECT")
		return DISCONNECT;
	else if (command == "SUBSCRIBE")
		return SUBSCRIBE;
	else if (command == "UNSUBSCRIBE")
		return UNSUBSCRIBE;
	else if (command == "SEND")
		return SEND;
	else if (command == "MESSAGE")
		return MESSAGE;
	else if (command == "ACK")
		return ACK;
	else
		return -1;
}

void CStompClient::ClearBuf(char *buf, int len) {
	for (UInt16 i = 0; i < len; i++) {
		buf[i] = '\1';
	}
}

int main(int argc, char** argv) {
	/*char buffer[10] = { 'a', 'b', 'c', '\n', '\n', 'd', 'b', 'c', '\n', 'a' };
	 string command("");
	 for (int c = 0; c < 10 && buffer[c] != '\n'; c++) {
	 command += buffer[c];
	 }
	 command += "_";
	 cout << command << endl;
	 cout << char(0) << "_" << endl;
	 cout << command << endl;
	 //TODO: implement main
	 string s(buffer);
	 string news("");
	 size_t found = s.find("\n\n");
	 if (found == string::npos) {
	 cout << "couldn't find it!" << endl;
	 } else {
	 found += 2;
	 for (found; found < 10 && buffer[found] != '\n'; found++) {
	 news += buffer[found];
	 }
	 //	}
	 cout << news << endl;
	 }*/

	//	Client c();
	//	Client *p_c;
	//	*p_c = &c;
	boost::shared_ptr<Client> p_c(new Client("TEST"));
	//	Client *p_c = &c;
	ClientWrapper cw(p_c);
	CStompClient conn(cw);
	//	cout << string("CONNECT\nlogin:mpStompClient\npasscode:dont\n\n" + '\0') << endl;
	//	mpStompClient.Send("CONNECT\nlogin:mpStompClient\npasscode:dont\n\n" + '\0');
	//	string s("");
	//	s += '\0';
	//	mpStompClient.Send(s);
	conn.Connect();
	cout << "here\n";
	//	mpStompClient.ReceiveLoop();
	cout << "here2\n";
}
