/*
 * ClientWrapper.cpp
 *
 *  Created on: Dec 25, 2010
 *      Author: Tom and Aviad
 */

#include "ClientWrapper.h"

ClientWrapper::ClientWrapper() {
	// TODO Auto-generated constructor stub

}

ClientWrapper::~ClientWrapper() {
	// TODO Auto-generated destructor stub
}

ClientWrapper& ClientWrapper::operator=(const ClientWrapper& _cw) {
	if (!(this == &_cw)) { // protection from self copy
		client = _cw.client;
	}
	return *this;
}
