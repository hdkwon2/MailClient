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
	Message(vmime::ref <vmime::header> hd, vmime::ref <vmime::body> bdy); // used when reading messages in from the mail boxes

	//getter functions
	std::string getSubject();
	std::string getSenderName();
	std::string getSenderAddr();
    std::string getBody();

    //builder functions
    void buildHeader(std::string subject, std::string from, std::string tos[], std::string txt);
};


#endif /* MAIL_H_ */
