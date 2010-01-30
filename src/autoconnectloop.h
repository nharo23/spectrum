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

#ifndef AUTOCONNECTLOOP_H
#define AUTOCONNECTLOOP_H

#include <string>
#include <vector>
#include <map>
#include "glib.h"
#include "purple.h"
#include "account.h"
#include "configfile.h"
#include "spectrumtimer.h"

// Connects accounts automatically after Spectrum start.
// Accounts are connected one by one by timer.
class AutoConnectLoop {
	public:
		AutoConnectLoop();
		~AutoConnectLoop();

		// Restore next account connection.
		// Returns true if there are more accounts to be connected.
		bool restoreNextConnection();

	private:
		std::vector <std::string> m_users;
		SpectrumTimer *m_timer;
};

#endif
