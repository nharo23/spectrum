#include "transport.h"
#include "main.h"
#include "../usermanager.h"
#include "../abstractbackend.h"
#include "testingbackend.h"
#include "testingprotocol.h"

Configuration configuration;

Transport* Transport::m_pInstance = NULL;

Transport::Transport(const std::string jid) { m_jid = jid; m_pInstance = this;
	m_userManager = new UserManager();
	m_hash = "123";
}

Transport::~Transport() {
	delete m_userManager;
}

void Transport::send(Tag *tag) {
	m_tags.push_back(tag);
}

UserManager *Transport::userManager() {
	return m_userManager;
}

const std::string &Transport::hash() {
	return m_hash;
}

AbstractBackend *Transport::sql() {
	return (AbstractBackend *) TestingBackend::instance();
}

std::string Transport::getId() {
	return "id1";
}

GlooxParser *Transport::parser() {
	return TestingBackend::instance()->getParser();
}

AbstractProtocol *Transport::protocol() {
	return (AbstractProtocol *) TestingProtocol::instance();
}

Configuration &Transport::getConfiguration() {
	return TestingBackend::instance()->getConfiguration();
}

void Transport::registerStanzaExtension(StanzaExtension *stanza) {
	delete stanza;
}
