/*
 * mail.h
 *
 *  Created on: Oct 9, 2012
 *      Author: don
 */

#ifndef MAIL_H_
#define MAIL_H_

#include <vmime/vmime.hpp>

class Message{
private:
	vmime::ref <vmime::message> msg;
	vmime::ref <vmime::header> header;
	vmime::ref <vmime::body> body;

public:
	Message();
	Message(vmime::ref <vmime::message> message); // used when reading messages in from the mail boxes

	//getter functions
	std::string getSubject();
	std::string getSenderName();
	std::string getSenderAddr();
    std::string getBody();
    std::string getDate();
    std::string getExpeditor();
    vmime::ref <vmime::message> getMessage();
    //builder functions
    void buildMessage(std::string subject, std::string from, std::vector<std::string>& tos, std::string txt);
};


#endif /* MAIL_H_ */
