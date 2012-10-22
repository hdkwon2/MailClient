#include "mail_parser.h"
#include <iostream>

bool mailParser::isStringContain(std::string subject, std::string keyword){
	return std::regex_match(subject, std::regex("(.*)"+keyword+"(.*)"));
}

//returns true if mail.date is in between from and to
//example date string : "25 Apr 2012 12:44:03"
bool mailParser::isDateInBetween(Message& mail, std::string from, std::string to){
	vmime::ref <vmime::datetime> msg = vmime::create <vmime::datetime>(mail.getDate());
	vmime::ref <vmime::datetime> fr = vmime::create <vmime::datetime>(from);
	vmime::ref <vmime::datetime> t;

	if(to == "") t = vmime::create <vmime::datetime> (vmime::datetime::now());
	else t = vmime::create <vmime::datetime> (to);

	if(*msg >= *fr && *msg <= *t) return true;
	else return false;
}
