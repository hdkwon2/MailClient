OBJECTS = connection_handler.o mail.o mail_parser.o
CFLAGS = -I/usr/local/include/ -I/usr/local/include -I/usr/local/include/p11-kit-1  -L/usr/local/lib -lvmime -lgnutls -lgsasl -std=c++0x

all: imap

imap: ${OBJECTS}
	g++ -o imap ${OBJECTS} $(CFLAGS)

connection_handler.o : source/connection_handler.cpp source/connection_handler.h
	g++ -c source/connection_handler.cpp $(CFLAGS) 

mail.o : source/mail.cpp source/mail.h
	g++ -c source/mail.cpp $(CFLAGS)
	
mail_parser.o: source/mail_parser.cpp source/mail_parser.h
	g++ -c source/mail_parser.cpp $(CFLAGS)
	
clean : 
	rm -rf *o imap