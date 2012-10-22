/*
 * mail_parser.h
 *
 *  Created on: Oct 13, 2012
 *      Author: don
 */

#ifndef MAIL_PARSER_H_
#define MAIL_PARSER_H_

#include "mail.h"
#include <regex>


class mailParser{
	//Getter functions
public:
	static bool isStringContain(std::string subject, std::string keyword);
	static bool isDateInBetween(Message& mail, std::string from, std::string to = "");
};




#endif /* MAIL_PARSER_H_ */
