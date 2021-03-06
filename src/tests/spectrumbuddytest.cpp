#include "spectrumbuddytest.h"

struct replaceBadJidCharacters {
	void operator()(char& c) { if(c == '@') c = '%';}
};

SpectrumBuddyTest::SpectrumBuddyTest(long id, PurpleBuddy *buddy) : AbstractSpectrumBuddy(id) {
}

std::string SpectrumBuddyTest::getAlias() {
	return m_alias;
}

std::string SpectrumBuddyTest::getName() {
	return m_name;
}

std::string SpectrumBuddyTest::getIconHash() {
	return m_iconHash;
}

bool SpectrumBuddyTest::getStatus(PurpleStatusPrimitive &status, std::string &statusMessage) {
	if (m_status == -1)
		return false;
	status = m_status;
	statusMessage = m_statusMessage;
	return true;
}

void SpectrumBuddyTest::setAlias(const std::string &alias) {
	m_alias = alias;
}

void SpectrumBuddyTest::setName(const std::string &name) {
	m_name = name;
}

void SpectrumBuddyTest::setStatus(PurpleStatusPrimitive status) {
	m_status = status;
}

void SpectrumBuddyTest::setStatusMessage(const std::string &statusMessage) {
	m_statusMessage = statusMessage;
}

void SpectrumBuddyTest::setIconHash(const std::string &iconHash) {
	m_iconHash = iconHash;
}

std::string SpectrumBuddyTest::getSafeName() {
	std::string name = getName();
	std::for_each( name.begin(), name.end(), replaceBadJidCharacters() );
	return name;
}


