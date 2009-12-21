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

#include "configfile.h"
#include "main.h"
#include "capabilityhandler.h"
#include "log.h"
#include "spectrum_util.h"
#include "transport.h"

Configuration DummyConfiguration;

ConfigFile::ConfigFile(const std::string &config) {

	m_loaded = true;
	m_jid = "";
	m_protocol = "";

	keyfile = g_key_file_new ();
	if (!config.empty())
		loadFromFile(config);
}

void ConfigFile::loadFromFile(const std::string &config) {
	int flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file (keyfile, config.c_str(), (GKeyFileFlags)flags, NULL)) {
		if (!g_key_file_load_from_file (keyfile, std::string("/etc/spectrum/" + config + ".cfg").c_str(), (GKeyFileFlags)flags, NULL))
		{
			Log("loadConfigFile", "Can't load config file!");
			Log("loadConfigFile", std::string("/etc/spectrum/" + config + ".cfg") << " or ./" << config);
			m_loaded = false;
		}
	}
}

void ConfigFile::loadFromData(const std::string &data) {
	int flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_data (keyfile, data.c_str(), (int) data.size(), (GKeyFileFlags) flags, NULL)) {
		Log("loadConfigFile", "Bad data");
		m_loaded = false;
	}
}

bool ConfigFile::loadString(std::string &variable, const std::string &section, const std::string &key, const std::string &def) {
	char *value;
	if ((value = g_key_file_get_string(keyfile, section.c_str(), key.c_str(), NULL)) != NULL) {
		variable.assign(value);
		if (!m_jid.empty())
			replace(variable, "$jid", m_jid.c_str());
		if (!m_protocol.empty())
			replace(variable, "$protocol", m_protocol.c_str());
		g_free(value);
	}
	else {
		if (def == "required") {
			Log("loadConfigFile", "You have to specify `" << key << "` in [" << section << "] section of config file.");
			return false;
		}
		else
			variable = def;
	}
	return true;
}

bool ConfigFile::loadInteger(int &variable, const std::string &section, const std::string &key, int def) {
	if (g_key_file_has_key(keyfile, section.c_str(), key.c_str(), NULL)) {
		GError *error = NULL;
		variable = (int) g_key_file_get_integer(keyfile, section.c_str(), key.c_str(), &error);
		if (error) {
			if (error->code == G_KEY_FILE_ERROR_INVALID_VALUE)
				Log("loadConfigFile", "Value of key `" << key << "` in [" << section << "] section is not integer.");
			g_error_free(error);
			return false;
		}
	}
	else {
		if (def == INT_MAX) {
			Log("loadConfigFile", "You have to specify `" << key << "` in [" << section << "] section of config file.");
			return false;
		}
		else
			variable = def;
	}
	return true;
}

bool ConfigFile::loadBoolean(bool &variable, const std::string &section, const std::string &key, bool def, bool required) {
	if (g_key_file_has_key(keyfile, section.c_str(), key.c_str(), NULL))
		variable = g_key_file_get_boolean(keyfile, section.c_str(), key.c_str(), NULL);
	else {
		if (required) {
			Log("loadConfigFile", "You have to specify `" << key << "` in [" << section << "] section of config file.");
			return false;
		}
		else
			variable = def;
	}
	return true;
}

Configuration ConfigFile::getConfiguration() {
	Configuration configuration;
	char **bind;
	int i;

	if (!loadString(configuration.protocol, "service", "protocol"))
		return DummyConfiguration;
	m_protocol = configuration.protocol;

	if (!loadString(configuration.jid, "service", "jid"))
		return DummyConfiguration;
	m_jid = configuration.jid;

	if (!loadString(configuration.discoName, "service", "name"))
		return DummyConfiguration;

	if (!loadString(configuration.server, "service", "server"))
		return DummyConfiguration;

	if (!loadString(configuration.password, "service", "password"))
		return DummyConfiguration;

	if (!loadInteger(configuration.port, "service", "port"))
		return DummyConfiguration;

	if (!loadString(configuration.filetransferCache, "service", "filetransfer_cache"))
		return DummyConfiguration;

	loadString(configuration.pid_f, "service", "pid_file", "/var/run/spectrum/" + configuration.jid);
	g_mkdir_with_parents(g_path_get_dirname(configuration.pid_f.c_str()), 0755);

	if (!loadString(configuration.sqlType, "database", "type"))
		return DummyConfiguration;

	if (!loadString(configuration.sqlHost, "database", "host", configuration.sqlType == "sqlite" ? "optional" :""))
		return DummyConfiguration;

	if (!loadString(configuration.sqlPassword, "database", "password", configuration.sqlType == "sqlite" ? "optional" :""))
		return DummyConfiguration;

	if (!loadString(configuration.sqlUser, "database", "user", configuration.sqlType == "sqlite" ? "optional" :""))
		return DummyConfiguration;

	if (!loadString(configuration.sqlDb, "database", "database"))
		return DummyConfiguration;
	g_mkdir_with_parents(g_path_get_dirname(configuration.sqlDb.c_str()), 0755);

	if (!loadString(configuration.sqlPrefix, "database", "prefix"))
		return DummyConfiguration;

	if (!loadString(configuration.userDir, "purple", "userdir"))
		return DummyConfiguration;
	g_mkdir_with_parents(g_path_get_dirname(configuration.userDir.c_str()), 0755);


	loadString(configuration.language, "service", "language", "en");
	loadString(configuration.logfile, "logging", "log_file", "");
	loadBoolean(configuration.onlyForVIP, "service", "only_for_vip", false);
	loadBoolean(configuration.VIPEnabled, "service", "vip_mode", false);
	loadBoolean(configuration.useProxy, "service", "use_proxy", false);

	if(g_key_file_has_key(keyfile,"service","transport_features",NULL)) {
		bind = g_key_file_get_string_list (keyfile,"service","transport_features",NULL, NULL);
		configuration.transportFeatures = 0;
		for (i = 0; bind[i]; i++){
			std::string feature(bind[i]);
			if (feature == "avatars")
				configuration.transportFeatures = configuration.transportFeatures | TRANSPORT_FEATURE_AVATARS;
			else if (feature == "chatstate")
				configuration.transportFeatures = configuration.transportFeatures | TRANSPORT_FEATURE_TYPING_NOTIFY;
			else if (feature == "filetransfer")
				configuration.transportFeatures = configuration.transportFeatures | TRANSPORT_FEATURE_FILETRANSFER;
		}
		g_strfreev (bind);
	}
	else configuration.transportFeatures = TRANSPORT_FEATURE_AVATARS | TRANSPORT_FEATURE_FILETRANSFER | TRANSPORT_FEATURE_TYPING_NOTIFY;

	if(g_key_file_has_key(keyfile,"service","vip_features",NULL)) {
		bind = g_key_file_get_string_list (keyfile,"service","vip_features",NULL, NULL);
		configuration.VIPFeatures = 0;
		for (i = 0; bind[i]; i++){
			std::string feature(bind[i]);
			if (feature == "avatars")
				configuration.VIPFeatures |= TRANSPORT_FEATURE_AVATARS;
			else if (feature == "chatstate")
				configuration.VIPFeatures |= TRANSPORT_FEATURE_TYPING_NOTIFY;
			else if (feature == "filetransfer")
				configuration.VIPFeatures |= TRANSPORT_FEATURE_FILETRANSFER;
		}
		g_strfreev (bind);
	}
	else configuration.VIPFeatures = TRANSPORT_FEATURE_AVATARS | TRANSPORT_FEATURE_FILETRANSFER | TRANSPORT_FEATURE_TYPING_NOTIFY;
	
	if (g_key_file_has_key(keyfile,"logging","log_areas",NULL)) {
		bind = g_key_file_get_string_list (keyfile,"logging","log_areas", NULL, NULL);
		configuration.logAreas = 0;
		for (i = 0; bind[i]; i++) {
			std::string feature(bind[i]);
			if (feature == "xml") {
				configuration.logAreas = configuration.logAreas | LOG_AREA_XML;
			}
			else if (feature == "purple")
				configuration.logAreas = configuration.logAreas | LOG_AREA_PURPLE;
		}
		g_strfreev (bind);
	}
	else configuration.logAreas = LOG_AREA_XML | LOG_AREA_PURPLE;
	
	if(g_key_file_has_key(keyfile,"purple","bind",NULL)) {
		bind = g_key_file_get_string_list (keyfile,"purple","bind",NULL, NULL);
		for (i = 0; bind[i]; i++){
			configuration.bindIPs[i] = (std::string)bind[i];
		}
		g_strfreev (bind);
	}

	if(g_key_file_has_key(keyfile,"service","allowed_servers",NULL)) {
		bind = g_key_file_get_string_list(keyfile, "service", "allowed_servers", NULL, NULL);
		for (i = 0; bind[i]; i++){
			configuration.allowedServers.push_back((std::string) bind[i]);
		}
		g_strfreev (bind);
	}

	if(g_key_file_has_key(keyfile,"service","admins",NULL)) {
		bind = g_key_file_get_string_list(keyfile, "service", "admins", NULL, NULL);
		for (i = 0; bind[i]; i++){
			configuration.admins.push_back((std::string) bind[i]);
		}
		g_strfreev (bind);
	}

	return configuration;
}

ConfigFile::~ConfigFile() {
	g_key_file_free(keyfile);
}

