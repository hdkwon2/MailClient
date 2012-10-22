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
	vmime::ref <vmime::net::store> store;
	std::vector <vmime::ref <vmime::net::message> > messages;

public:
	~ConnectionHandler();
	void connectStore();
	void connectTransport();
	void sendMessage(Message& mail);
	std::vector<vmime::ref<vmime::message> > getUnreadMessages();
	std::vector<vmime::ref<vmime::message> > getSampleMessages(int to); //for testing only
};



#endif /* CONNECTION_HANDLER_H_ */
