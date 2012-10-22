
#include "mail.h"
#include <iostream>

	vmime::charset ch = vmime::charsets::UTF_8;

	Message::Message(){
		msg = vmime::create <vmime::message>();
		header = msg->getHeader();
		body = msg->getBody();
	}

	Message::Message(vmime::ref <vmime::message> message){
		header = message->getHeader();
		body = message->getBody();
	}


	//getter functions
	std::string Message::getSubject(){
		if(header == NULL){
			return NULL;
		}
		return header->Subject()->getValue().dynamicCast <vmime::text>()->getConvertedText(ch);
	}

	std::string Message::getSenderName(){
		if(header == NULL){
			return NULL;
		}
		return header->From()->getValue().dynamicCast <vmime::mailbox>()->getName().getConvertedText(ch);
	}

	std::string Message::getSenderAddr(){
		if(header == NULL){
			return NULL;
		}
		return header->From()->getValue().dynamicCast <vmime::mailbox>()->getEmail();
	}

	std::string Message::getBody(){
		if(body == NULL){
			return NULL;
		}
		vmime::string str;
		vmime::utility::outputStreamAdapter out(std::cout);
		vmime::utility::charsetFilteredOutputStream fout(body->getCharset(), vmime::charset("utf-8"), out);

		if(body->getPartCount() > 0){
			// Why the fuck can't I do this?
			body->getPartAt(0)->getBody()->getContents()->extract(fout);
		}
		else{
			body->getContents()->extract(fout);
		}

		return str;
	}

	std::string Message::getDate(){
		if(header == NULL){
			return NULL;
		}
		vmime::string str;
		vmime::utility::outputStreamStringAdapter sout(str);
		header->Date()->generate(sout);
		return str;
	}

	vmime::ref <vmime::message> Message::getMessage(){
		return msg;
	}
	//Builder functions
	void Message::buildMessage(std::string subject, std::string from, std::vector<std::string>& tos, std::string txt){

		try{
			vmime::messageBuilder mb;
			//setting fields
			mb.setSubject(vmime::text(subject));
			mb.setExpeditor(vmime::mailbox(from));
			for(int i=0; i < tos.size(); i++){
				std::cout << tos[i] <<std::endl;
				mb.getRecipients().appendAddress(vmime::create <vmime::mailbox>(tos[i]));
			}
			mb.getTextPart()->setCharset(vmime::charsets::ISO8859_15);
			mb.getTextPart()->setText(vmime::create <vmime::stringContentHandler>(txt));
			msg = mb.construct();
		}catch(vmime::exception& e){
			std::cerr << "vmime::exception: " << e.what() << std::endl;
		}catch(std::exception& e){
			std::cerr << "std::exception: " << e.what() << std::endl;
		}

	}
