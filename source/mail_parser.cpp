#include "mail_parser.h"

static bool isSubjectContain(Message & mail, std::string keyword){
	return std::regex_match(mail.getSubject(), std::regex("(.*)"+keyword+"(.*)"));

}
