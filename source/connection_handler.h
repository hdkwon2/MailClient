/*
 * connection_handler.h
 *
 *  Created on: Oct 9, 2012
 *      Author: don
 */

#ifndef CONNECTION_HANDLER_H_
#define CONNECTION_HANDLER_H_

#include "vmime/vmime.hpp"
static std::ostream& operator<<(std::ostream& os, const vmime::exception& e);
static std::string findAvailableProtocols(const vmime::net::service::Type type);

class ConnectionHandler{
private:
	vmime::ref <vmime::net::transport> tr;


public:
		vmime::ref <vmime::net::folder> connectStore();
		void connectTransport();
};



#endif /* CONNECTION_HANDLER_H_ */
