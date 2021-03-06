/**
 * XMPP - libpurple transport
 *
 * Copyright (C) 2009, Jan Kaluza <hanzz@soc.pidgin.im>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "hon.h"
#include "../main.h"
#include "transport.h"

HoNProtocol::HoNProtocol() {
	m_transportFeatures.push_back("jabber:iq:register");
	m_transportFeatures.push_back("jabber:iq:gateway");
	m_transportFeatures.push_back("http://jabber.org/protocol/disco#info");
	m_transportFeatures.push_back("http://jabber.org/protocol/caps");
// 	m_transportFeatures.push_back("http://jabber.org/protocol/chatstates");
// 	m_transportFeatures.push_back("http://jabber.org/protocol/activity+notify");
	m_transportFeatures.push_back("http://jabber.org/protocol/commands");

	m_buddyFeatures.push_back("http://jabber.org/protocol/disco#info");
	m_buddyFeatures.push_back("http://jabber.org/protocol/caps");
// 	m_buddyFeatures.push_back("http://jabber.org/protocol/chatstates");
	m_buddyFeatures.push_back("http://jabber.org/protocol/commands");

	// no filetransfer in HoN now :)
// 	m_buddyFeatures.push_back("http://jabber.org/protocol/si/profile/file-transfer");
// 	m_buddyFeatures.push_back("http://jabber.org/protocol/bytestreams");
// 	m_buddyFeatures.push_back("http://jabber.org/protocol/si");
}

HoNProtocol::~HoNProtocol() {}


std::list<std::string> HoNProtocol::transportFeatures(){
	return m_transportFeatures;
}

std::list<std::string> HoNProtocol::buddyFeatures(){
	return m_buddyFeatures;
}

std::string HoNProtocol::text(const std::string &key) {
	if (key == "instructions")
		return _("Enter your HoN username and password:");
	else if (key == "username")
		return _("HoN Username:");
	return "not defined";
}

void HoNProtocol::makePurpleUsernameRoom(User *user, const JID &to, std::string &name) {
	std::string username = to.username();
// 	// "spectrum%conference.spectrum.im@irc.spectrum.im/HanzZ" -> "spectrum@conference.spectrum.im/HanzZ"
// 	if (!to.resource().empty()) {
// 		std::for_each( username.begin(), username.end(), replaceJidCharacters() );
// 		name.assign(username + "/" + to.resource());
// 	}
	// "spectrum%conference.spectrum.im@irc.spectrum.im" -> "spectrum@conference.spectrum.im"
// 	else {
		// Hon\2030@conference.spectrum.im/something" -> "Hon 30"
		name.assign(JID::unescapeNode(to.username()));
// 	}
}

void HoNProtocol::makeRoomJID(User *user, std::string &name) {
	// Hon 30" -> "Hon\2030@hon.domain.tld"
	std::string name_safe = JID::escapeNode(name);
	name.assign(name_safe + "@" + Transport::instance()->jid());
	std::transform(name.begin(), name.end(), name.begin(),(int(*)(int)) std::tolower);
// 	if (!j.resource().empty()) {
// 		name += "/" + j.resource();
// 	}
	std::cout << "ROOMJID: " << name << "\n";
}

void HoNProtocol::makeUsernameRoom(User *user, std::string &name) {
	// "spectrum@conferece.spectrum.im/HanzZ" -> "HanzZ"
	name = JID(name).resource();
}


void HoNProtocol::onPurpleAccountCreated(PurpleAccount *account) {
}

SPECTRUM_PROTOCOL(hon, HoNProtocol)
