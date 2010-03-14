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

#ifndef ABSTRACT_CONFIG_HANDLER_H
#define ABSTRACT_CONFIG_HANDLER_H

#include <string>
#include <list>
#include "purple.h"
#include "gloox/tag.h"

using namespace gloox;

class AbstractConfigInterfaceHandler {
	public:
		virtual ~AbstractConfigInterfaceHandler() {}

		// Returns true if handler will handle this tag.
		virtual bool handleCondition(Tag *tag) = 0;

		// Handles Tag*. ConfigInterface will send the response if it's not NULL.
		virtual Tag *handleTag(Tag *tag) = 0;
};

#endif