/*
 * C_STOMP_client.cpp
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#include "C_STOMP_client.h"
#include <iostream>
#include "Poco/Mutex.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

C_STOMP_client::C_STOMP_client() :
	host("localhost"), port(61613), sa(host, port), socket(sa), str(socket) {

}

C_STOMP_client::C_STOMP_client(ClientWrapper _client) :
	host("localhost"), port(61613), sa(host, port), socket(sa), str(socket) {
	client = _client; //FIXME chnage to client(_client) after implementing
	//	Client. also in the next ctor.
	//	RunnableAdapter<C_STOMP_client> sender;
	RunnableAdapter<C_STOMP_client> receiver(*this, &C_STOMP_client::receive);
	pool.start(receiver);
}

C_STOMP_client::C_STOMP_client(string _host, UInt16 _port,
		ClientWrapper _client) :
	host(_host), port(_port), sa(host, port), socket(sa), str(socket) {
	client = _client;
	//	RunnableAdapter<C_STOMP_client> sender;
	RunnableAdapter<C_STOMP_client> receiver(*this, &C_STOMP_client::receive);
	pool.start(receiver);
}

C_STOMP_client::~C_STOMP_client() {
	// TODO Auto-generated dtor stub
}

void C_STOMP_client::connect() {
	try {
		socket.connect(sa);
	} catch (Exception e) {
		//TODO: log this exception (should not happen)
	}
	string msg("CONNECT");
	msg += '\n';
	msg += "login:dontcare" + '\n';
	msg += "passcode:dontcare" + '\n';
	msg += '\n';
	msg += '\0';
	send(msg);
}
/**
 * Send this message to the STOMP server.
 * This function is in charge of translating the message to the trading protocol
 * and later to the STOMP protocol. */
void C_STOMP_client::send(string msg) {
	str << msg;
	str.flush();
}

void C_STOMP_client::receive() {
	char buffer[1024];
	for (UInt16 i; i < 1024; i++) {
		buffer[i] = '~'; //this will be the "null" character
	}
	UInt16 len = -1;
	while (len != 0) { //0 means a greacefull shutdown on the server side.
		//this is where the program should be most of the time, waiting for input.
		len = socket.receiveBytes(&buffer, 1023); //recieve up to 1023 chars.

		buffer[len] = '~'; //return "my" null char
		/* //in case decoding is needed, uncomment this block.
		 for (UInt16 chr = 0; chr < len; chr++) {
		 buffer[chr] = decode(buffer[chr]);
		 }*/// end of commented block
		UInt16 command = get_STOMP_command(buffer);
		if (command == NOCOMMAND) { //this is not a legal command, ignore it.
			break; //TODO: add some logging to track this error.
		} else if (command == CONNECTED) {
			connected(buffer, len);
			break;
		} else if (command == MESSAGE) {
			message(buffer, len);
			break;
			//		} else if (command == ACK) {
			//			ack(buffer, len);
		} else { // these commands are illegal for the client to receive
			break; // ignore them. TODO: add some logging to track this event.
		}
		//
	}

}

void C_STOMP_client::connected(char* buffer, Poco::UInt16 len) {
	is_connected = true;
	string header;
	// the session_id identifier will start after the "session: " header
	// ( |CONNECTED\n| = 10, |session: | = 9)
	for (UInt16 c = 10; c < len && buffer[c] != ':'; c++) {
		header += buffer[c];
	}
	if (header != "session") { //no session-id is given
		return;
	} else {
		for (UInt16 c = 19; c < len && buffer[c] != '\n'; c++) {
			session_id += buffer[c];
		}
	}
}
void C_STOMP_client::message(char* buffer, Poco::UInt16 len) {
	string msg(buffer, len);
	string content("");
	size_t start = msg.find("\n\n");
	if (start != string::npos) { // this message contains content. it's only 1 line - get it!
		start += 2; //leave the "\n\n" behind.
		for (start; start < len && buffer[start] != '\n'; start++) {
			content += buffer[start];
		}
		content += '\n';//the trading protocol requires '\n' at the end.
		//TODO: check that this does not miss anything.
	}
	handle_trading_message(content);

}
//void C_STOMP_client::ack(char* buffer, Poco::UInt16 len) {
//}

void C_STOMP_client::handle_trading_message(string raw_content) {
	STOMP_command command(NOCOMMAND);
	parse_trading_message(raw_content, &command);
	//parse command to actions
	Poco::Mutex::ScopedLock lock(client.mutex);
	client.getClient()/*->make some changes */;
	// Here the Mutex is implicitly released and the Client is "unlocked".
}
void C_STOMP_client::parse_trading_message(string content, STOMP_command *command) {

}
UInt16 C_STOMP_client::get_STOMP_command(char *buffer) {
	string command("");
	UInt16 len = 13; // longest (relevant) STOMP command is |UNSUBSCRIBE| = 11
	for (UInt16 c = 0; c < len && buffer[c] != '\n'; c++) {
		command += buffer[c];
	}
	UInt16 ret = str2command(command);
	return ret; //This might be -1, which is an error!

}

bool C_STOMP_client::need_ack(char *buffer, UInt16 len) {
	string msg(buffer, len);
	size_t found = msg.find("ack: client");
	if (found != string::npos)
		return true;
	return false;
}

UInt16 C_STOMP_client::str2command(string command) {
	if (command == "CONNECT")
		return CONNECT;
	else if (command == "CONNECTED")
		return CONNECTED;
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

int main(int argc, char** argv) {
	char buffer[10] = { 'a', 'b', 'c', '\n', '\n', 'd', 'b', 'c', '\n', 'a' };
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
	}
}
