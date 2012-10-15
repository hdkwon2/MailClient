/*******************************************************************
* Referenced from vmime/examples/example6.cpp
* Written by Hyuk Don Kwon (hdkwon2@illinois.edu)
* Functions to connect to an IMAP server, and view information about
*   mail boxes, and retrieve messages.
********************************************************************/

#include "vmime/vmime.hpp"
#include "vmime/platforms/posix/posixHandler.hpp"
#include "mail.h"
#include "mail_parser.h"
#include "connection_handler.h"
#include <iostream>
using namespace std;
/*********************************************************************************/
static string findAvailableProtocols(const vmime::net::service::Type type){

		vmime::net::serviceFactory* sf = vmime::net::serviceFactory::getInstance();
		ostringstream res;
		int count =0;
		for (int i = 0 ; i < sf->getServiceCount() ; ++i){
			const vmime::net::serviceFactory::registeredService& serv = *sf->getServiceAt(i);

			if (serv.getType() == type)
			{
				if (count != 0)
					res << ", ";

				res << serv.getName();
				++count;
			}
		}

		return res.str();
}

// VMIME Exception helper
   static std::ostream& operator<<(std::ostream& os, const vmime::exception& e)
   {
	os << "* vmime::exceptions::" << e.name() << std::endl;
	os << "    what = " << e.what() << std::endl;

	// More information for special exceptions
	if (dynamic_cast <const vmime::exceptions::command_error*>(&e))
	{
		const vmime::exceptions::command_error& cee =
			dynamic_cast <const vmime::exceptions::command_error&>(e);

		os << "    command = " << cee.command() << std::endl;
		os << "    response = " << cee.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::invalid_response*>(&e))
	{
		const vmime::exceptions::invalid_response& ir =
			dynamic_cast <const vmime::exceptions::invalid_response&>(e);

		os << "    response = " << ir.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::connection_greeting_error*>(&e))
	{
		const vmime::exceptions::connection_greeting_error& cgee =
			dynamic_cast <const vmime::exceptions::connection_greeting_error&>(e);

		os << "    response = " << cgee.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::authentication_error*>(&e))
	{
		const vmime::exceptions::authentication_error& aee =
			dynamic_cast <const vmime::exceptions::authentication_error&>(e);

		os << "    response = " << aee.response() << std::endl;
	}

	if (dynamic_cast <const vmime::exceptions::filesystem_exception*>(&e))
	{
		const vmime::exceptions::filesystem_exception& fse =
			dynamic_cast <const vmime::exceptions::filesystem_exception&>(e);

		os << "    path = " << vmime::platform::getHandler()->
			getFileSystemFactory()->pathToString(fse.path()) << std::endl;
	}

	if (e.other() != NULL)
		os << *e.other();

	return os;
   }

/*******************************************************************************/
#if VMIME_HAVE_SASL_SUPPORT

// SASL authentication handler
class interactiveAuthenticator : public vmime::security::sasl::defaultSASLAuthenticator
{
	const std::vector <vmime::ref <vmime::security::sasl::SASLMechanism> > getAcceptableMechanisms
		(const std::vector <vmime::ref <vmime::security::sasl::SASLMechanism> >& available,
		 vmime::ref <vmime::security::sasl::SASLMechanism> suggested) const
	{
		std::cout << std::endl << "Available SASL mechanisms:" << std::endl;

		for (unsigned int i = 0 ; i < available.size() ; ++i)
		{
			std::cout << "  " << available[i]->getName();

			if (suggested && available[i]->getName() == suggested->getName())
				std::cout << "(suggested)";
		}

		std::cout << std::endl << std::endl;

		return defaultSASLAuthenticator::getAcceptableMechanisms(available, suggested);
	}

	void setSASLMechanism(vmime::ref <vmime::security::sasl::SASLMechanism> mech)
	{
		std::cout << "Trying '" << mech->getName() << "' authentication mechanism" << std::endl;

		defaultSASLAuthenticator::setSASLMechanism(mech);
	}

	const vmime::string getUsername() const
	{
		if (m_username.empty())
			m_username = getUserInput("Username");

		return m_username;
	}

	const vmime::string getPassword() const
	{
		if (m_password.empty())
			m_password = getUserInput("Password");

		return m_password;
	}

	static const vmime::string getUserInput(const std::string& prompt)
	{
		std::cout << prompt << ": ";
		std::cout.flush();

		vmime::string res;
		std::getline(std::cin, res);

		return res;
	}

private:

	mutable vmime::string m_username;
	mutable vmime::string m_password;
};

#else // !VMIME_HAVE_SASL_SUPPORT

// Simple authentication handler
class interactiveAuthenticator : public vmime::security::defaultAuthenticator
{
	const vmime::string getUsername() const
	{
		if (m_username.empty())
			m_username = getUserInput("Username");

		return m_username;
	}

	const vmime::string getPassword() const
	{
		if (m_password.empty())
			m_password = getUserInput("Password");

		return m_password;
	}

	static const vmime::string getUserInput(const std::string& prompt)
	{
		std::cout << prompt << ": ";
		std::cout.flush();

		vmime::string res;
		std::getline(std::cin, res);

		return res;
	}

private:

	mutable vmime::string m_username;
	mutable vmime::string m_password;
};

#endif // VMIME_HAVE_SASL_SUPPORT


#if VMIME_HAVE_TLS_SUPPORT

// Certificate verifier (TLS/SSL)
class interactiveCertificateVerifier : public vmime::security::cert::defaultCertificateVerifier
{
public:

	void verify(vmime::ref <vmime::security::cert::certificateChain> chain)
	{
		try
		{
			setX509TrustedCerts(m_trustedCerts);

			defaultCertificateVerifier::verify(chain);
		}
		catch (vmime::exceptions::certificate_verification_exception&)
		{
			// Obtain subject's certificate
			vmime::ref <vmime::security::cert::certificate> cert = chain->getAt(0);

			std::cout << std::endl;
			std::cout << "Server sent a '" << cert->getType() << "'" << " certificate." << std::endl;
			std::cout << "Do you want to accept this certificate? (Y/n) ";
			std::cout.flush();

			std::string answer;
			std::getline(std::cin, answer);

			if (answer.length() != 0 &&
			    (answer[0] == 'Y' || answer[0] == 'y'))
			{
				// Accept it, and remember user's choice for later
				if (cert->getType() == "X.509")
				{
					m_trustedCerts.push_back(cert.dynamicCast
						<vmime::security::cert::X509Certificate>());
				}

				return;
			}

			throw vmime::exceptions::certificate_verification_exception
				("User did not accept the certificate.");
		}
	}

private:

	static std::vector <vmime::ref <vmime::security::cert::X509Certificate> > m_trustedCerts;
};


std::vector <vmime::ref <vmime::security::cert::X509Certificate> >
	interactiveCertificateVerifier::m_trustedCerts;

static vmime::ref <vmime::net::session> g_session = vmime::create <vmime::net::session>();
#endif // VMIME_HAVE_TLS_SUPPORT
/*********************************************************************************/


vmime::ref <vmime::net::folder> ConnectionHandler::connectStore() {
	try {
		cout << "Enter an URL to connect to store service." << endl;
		cout << "Available protocols: "
				<< findAvailableProtocols(vmime::net::service::TYPE_STORE)
				<< endl;
		cout
				<< "(eg. pop3://user:pass@myserver.com, imaps://user:pass@imap.gmail.com:993)"
				<< endl;
		cout << "> ";
		cout.flush();

		vmime::string urlString;
		//getline(cin, urlString);
		urlString = "imaps://hyukdonkwon:eoqkr2ek@imap.gmail.com:993";
		vmime::utility::url url(urlString);

		vmime::ref < vmime::net::store > store;
		//If credential is not provided in the url
		if (url.getUsername().empty() || url.getPassword().empty())
			store = g_session->getStore(url,
					vmime::create<interactiveAuthenticator>());
		else
			store = g_session->getStore(url);

#if VMIME_HAVE_TLS_SUPPORT  //if vmime has tls support
		store->setProperty("connection.tls", true);

		store->setCertificateVerifier(
				vmime::create<interactiveCertificateVerifier>());
#endif
		store->connect();
		// Display some information about the connection
		vmime::ref < vmime::net::connectionInfos > ci =
				store->getConnectionInfos();

		cout << endl;
		cout << "Connected to '" << ci->getHost() << "' (port " << ci->getPort()
				<< ")" << endl;
		cout << "Connection is " << (store->isSecuredConnection() ? "" : "NOT ")
				<< "secured." << endl;
		vmime::ref <vmime::net::folder> folder;
		folder = store->getDefaultFolder();
		folder->open(vmime::net::folder::MODE_READ_WRITE);
		return folder;

	} catch (vmime::exception& e) {
		cerr << endl;
		cerr << e << endl;
		throw;
	} catch (exception& e) {
		cerr << endl;
		cerr << "std::exception: " << e.what() << endl;
		throw;
	}
}


void ConnectionHandler::connectTransport() {

	try {
		vmime::string urlString;
		getline(cin, urlString);

		vmime::utility::url url(urlString);

		tr = g_session->getTransport(url,
				vmime::create<interactiveAuthenticator>());
#if VMIME_HAVE_TLS_SUPPORT
		tr->setProperty("connection.tls", true);
		tr->setCertificateVerifier(
				vmime::create<interactiveCertificateVerifier>());
#endif
		tr->connect();
	} catch (vmime::exception& e) {
		cerr << endl;
		cerr << e << endl;
		throw;
	} catch (exception& e) {
		cerr << endl;
		cerr << "std::exception: " << e.what() << endl;
		throw;
	}
}




int main(int arc, const char* argv[]){
	//must set this platform handler before using any vmime objects
	vmime::platform::setHandler <vmime::platforms::posix::posixHandler>();

	ConnectionHandler *ch = new ConnectionHandler();
	vmime::ref <vmime::net::folder> folder = ch->connectStore();
	vmime::message parsedMsg= *(folder->getMessage(4)->getParsedMessage());
	vmime::utility::outputStreamAdapter out(std::cout);
	parsedMsg.getBody()->getContents()->extract(out);
	/*Message *mail = new Message(parsedMsg.getHeader(), parsedMsg.getBody());
	cout << mail->getSubject() << endl;
	cout << mail->getSenderName() << endl;
	cout << mail->getSenderAddr() << endl;
	cout << mail->getBody()<< endl;
	cout << "end of the list" << endl;*/


	return 1;
}
