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

#include "twitter.h"
#include "../transport.h"
#include "Poco/Format.h"

TwitterProtocol::TwitterProtocol() {
	m_transportFeatures.push_back("jabber:iq:register");
	m_transportFeatures.push_back("jabber:iq:gateway");
	m_transportFeatures.push_back("http://jabber.org/protocol/disco#info");
	m_transportFeatures.push_back("http://jabber.org/protocol/caps");
	m_transportFeatures.push_back("http://jabber.org/protocol/chatstates");
// 	m_transportFeatures.push_back("http://jabber.org/protocol/activity+notify");
	m_transportFeatures.push_back("http://jabber.org/protocol/commands");

	m_buddyFeatures.push_back("http://jabber.org/protocol/disco#info");
	m_buddyFeatures.push_back("http://jabber.org/protocol/caps");
	m_buddyFeatures.push_back("http://jabber.org/protocol/chatstates");
	m_buddyFeatures.push_back("http://jabber.org/protocol/commands");

// 	m_buddyFeatures.push_back("http://jabber.org/protocol/si/profile/file-transfer");
// 	m_buddyFeatures.push_back("http://jabber.org/protocol/bytestreams");
// 	m_buddyFeatures.push_back("http://jabber.org/protocol/si");
	Transport::instance()->getConfiguration().username_mask = "$username@api.twitter.com";
	purple_prefs_set_string("TC_CONSUMER_KEY", "5mFePMiJi0KpeURONkelg");
	purple_prefs_set_string("TC_CONSUMER_SECRET", "YFZCDJwRhbkccXEnaYr1waCQejTJcOY8F7l5Wim3FA");
}

TwitterProtocol::~TwitterProtocol() {}

std::list<std::string> TwitterProtocol::transportFeatures(){
	return m_transportFeatures;
}

std::list<std::string> TwitterProtocol::buddyFeatures(){
	return m_buddyFeatures;
}

void TwitterProtocol::onUserCreated(AbstractUser *user) {
	PurpleValue *value;
	if ( (value = user->getSetting("twitter_oauth_secret")) == NULL ) {
		Transport::instance()->sql()->addSetting(user->storageId(), "twitter_oauth_secret", "", PURPLE_TYPE_STRING);
		value = purple_value_new(PURPLE_TYPE_STRING);
		purple_value_set_string(value, "");
		g_hash_table_replace(user->settings(), g_strdup("twitter_oauth_secret"), value);
	}
	if ( (value = user->getSetting("twitter_oauth_token")) == NULL ) {
		Transport::instance()->sql()->addSetting(user->storageId(), "twitter_oauth_token", "", PURPLE_TYPE_STRING);
		value = purple_value_new(PURPLE_TYPE_STRING);
		purple_value_set_string(value, "");
		g_hash_table_replace(user->settings(), g_strdup("twitter_oauth_token"), value);
	}
	if ( (value = user->getSetting("twitter_sent_msg_ids")) == NULL ) {
		Transport::instance()->sql()->addSetting(user->storageId(), "twitter_sent_msg_ids", "", PURPLE_TYPE_STRING);
		value = purple_value_new(PURPLE_TYPE_STRING);
		purple_value_set_string(value, "");
		g_hash_table_replace(user->settings(), g_strdup("twitter_sent_msg_ids"), value);
	}
	if ( (value = user->getSetting("twitter_last_msg_id")) == NULL ) {
		Transport::instance()->sql()->addSetting(user->storageId(), "twitter_last_msg_id", "", PURPLE_TYPE_STRING);
		value = purple_value_new(PURPLE_TYPE_STRING);
		purple_value_set_string(value, "");
		g_hash_table_replace(user->settings(), g_strdup("twitter_last_msg_id"), value);
	}
}

void TwitterProtocol::onDestroy(AbstractUser *user) {
	PurpleAccount *account = user->account();
	if (account == NULL)
		return;
	PurpleValue *value = NULL;
	if ( (value = user->getSetting("twitter_oauth_secret")) != NULL ) {
		purple_value_set_string(value, purple_account_get_string(account, "twitter_oauth_secret", ""));
		user->updateSetting("twitter_oauth_secret", value);
	}
	if ( (value = user->getSetting("twitter_oauth_token")) != NULL ) {
		purple_value_set_string(value, purple_account_get_string(account, "twitter_oauth_token", ""));
		user->updateSetting("twitter_oauth_token", value);
	}
	if ( (value = user->getSetting("twitter_sent_msg_ids")) != NULL ) {
		purple_value_set_string(value, purple_account_get_string(account, "twitter_sent_msg_ids", ""));
		user->updateSetting("twitter_sent_msg_ids", value);
	}
	if ( (value = user->getSetting("twitter_last_msg_id")) != NULL ) {
		purple_value_set_string(value, purple_account_get_string(account, "twitter_last_msg_id", ""));
		user->updateSetting("twitter_last_msg_id", value);
	}
}

void TwitterProtocol::onPurpleAccountCreated(PurpleAccount *account) {
	AbstractUser *user = (AbstractUser *) account->ui_data;
	PurpleValue *value = NULL;
	if ( (value = user->getSetting("twitter_oauth_secret")) != NULL ) {
		purple_account_set_string(account, "twitter_oauth_secret", purple_value_get_string(value));
	}
	if ( (value = user->getSetting("twitter_oauth_token")) != NULL ) {
		purple_account_set_string(account, "twitter_oauth_token", purple_value_get_string(value));
	}
	if ( (value = user->getSetting("twitter_sent_msg_ids")) != NULL ) {
		purple_account_set_string(account, "twitter_sent_msg_ids", purple_value_get_string(value));
	}
	if ( (value = user->getSetting("twitter_last_msg_id")) != NULL ) {
		purple_account_set_string(account, "twitter_last_msg_id", purple_value_get_string(value));
	}
}

std::string TwitterProtocol::text(const std::string &key) {
	if (key == "instructions")
		return _("Enter your Twitter username and password:");
	else if (key == "username")
		return _("Twitter username");
	return "not defined";
}

bool TwitterProtocol::onNotifyUri(const char *uri) {
	m_lastUri = uri;
	return true;
}

bool TwitterProtocol::onPurpleRequestInput(void *handle, AbstractUser *user, const char *title, const char *primary,const char *secondary, const char *default_value,gboolean multiline, gboolean masked, gchar *hint,const char *ok_text, GCallback ok_cb,const char *cancel_text, GCallback cancel_cb, PurpleAccount *account, const char *who,PurpleConversation *conv, void *user_data) {
	if (title == NULL)
		return false;
	std::string t(title);
	if (t == "Input your PIN") {
		std::string text = Poco::format(_("Please allow Spectrum access to your Twitter account. Open following url and send the PIN number from the web page back to Spectrum.\n%s"), m_lastUri);
		Message s(Message::Chat, user->jid(), tr(user->getLang(), text));
		s.setFrom(Transport::instance()->jid());
		Transport::instance()->send(s.tag());
		m_callbacks[handle].ok_cb = ok_cb;
		m_callbacks[handle].user_data = user_data;
		user->setProtocolData(handle);
		return true;
	}
	return false;
}

void TwitterProtocol::onRequestClose(void *handle) {
	m_callbacks.erase(handle);
}

void TwitterProtocol::onXMPPMessageReceived(AbstractUser *user, const Message &msg) {
	void *handle = user->protocolData();
	if (m_callbacks.find(handle) == m_callbacks.end())
		return;
	((PurpleRequestInputCb) m_callbacks[handle].ok_cb)(m_callbacks[handle].user_data, msg.body().c_str());
}


SPECTRUM_PROTOCOL(twitter, TwitterProtocol)
