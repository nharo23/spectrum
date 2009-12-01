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

#include "spectrumbuddy.h"
#include "main.h"
#include "user.h"
#include "log.h"
#include "sql.h"
#include "usermanager.h"
#include "caps.h"

SpectrumBuddy::SpectrumBuddy(long id, PurpleBuddy *buddy) : m_id(id), m_buddy(buddy) {
}

SpectrumBuddy::~SpectrumBuddy() {
}

long SpectrumBuddy::getId() {
	return m_id;
}

void SpectrumBuddy::setId(long id) {
	m_id = id;
}

std::string SpectrumBuddy::getAlias() {
	std::string alias;
	if (purple_buddy_get_server_alias(m_buddy))
		alias = (std::string) purple_buddy_get_server_alias(m_buddy);
	else
		alias = (std::string) purple_buddy_get_alias(m_buddy);
	return alias;
}

std::string SpectrumBuddy::getName() {
	return std::string(purple_buddy_get_name(m_buddy));
}

std::string SpectrumBuddy::getSafeName() {
	std::string name = getName();
	std::for_each( name.begin(), name.end(), replaceBadJidCharacters() );
	return name;
}

std::string SpectrumBuddy::getJid() {
	return getSafeName() + "@" + GlooxMessageHandler::instance()->jid() + "/bot";
}



