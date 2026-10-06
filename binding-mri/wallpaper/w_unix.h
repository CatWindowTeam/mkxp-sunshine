#pragma once
#include "define.h"
#ifdef unix_like
	#include <cstdio>
	#include <iostream>
	#include <fstream>
	#include <vector>
	#include <map>
	#include <SDL3/SDL_stdinc.h>
	#include <SDL3/SDL_filesystem.h>
	#include "etc.h"
	#include "sharedstate.h"
	#include "../binding-util.h"
	#include "../binding-types.h"
	#include "config.h"
	#include "oneshot.h"
	#include "debugwriter.h"

	#include <gio/gio.h>
	#ifdef API_ONESHOT_EXTENSIONS_XFCE
		#include <xfconf/xfconf.h>
	#endif
	#include <unistd.h>
	#include <algorithm>
	#include <iostream>
	#include <string>
	#include <sstream>
	#include <cstdlib>
	#include <sys/stat.h>
	static std::string desktop = "uninitialized";
	// GNOME settings
	static GSettings *bgsetting;
	static std::string defPictureURI, defPictureOptions, defPrimaryColor, defColorShading;
	static std::string defPictureURIDark;
	static bool hasPictureURIDark = false;
	// KDE settings
	static std::map<std::string, std::string> defPlugins, defPictures, defColors, defModes;
	static std::map<std::string, bool> defBlurs;
	// XFCE settings
	#ifdef API_ONESHOT_EXTENSIONS_XFCE
		static XfconfChannel* bgchannel;
		static std::vector<std::string> xfceMonitorPrefixes;
		static std::map<std::string, std::string> defXfcePictureURI;
		static std::map<std::string, int> defXfcePictureStyle;
		static std::map<std::string, int> defXfceColorStyle;
		static std::map<std::string, GValue> defXfceColor;
		static std::map<std::string, bool> defXfceColorExists;
		static bool xfceHasSingleWorkspaceProps = false;
		static bool xfceSingleWorkspaceMode = false;
		static int xfceSingleWorkspaceNumber = 0;
	#endif
		
	// LXDE settings
	static std::string originalBgPath = "";
	static std::string originalBgMode = "";
	// LXQT
	static std::string DBUS_SESSION_BUS_ADDRESS = "";
	// Wallpaper utility (feh/nitrogen), used when no known DE is detected
	static std::string wpTool = "";
	static std::string originalFehbgCmd = "";
	static bool originalFehbgExists = false;
	// Fallback settings
	static std::string fallbackPath;

	static bool gsettingsHasKey(GSettings *settings, const char *key){
		GSettingsSchema *schema = NULL;
		g_object_get(settings, "settings-schema", &schema, NULL);
		if (!schema)
			return false;
		bool has = g_settings_schema_has_key(schema, key);
		g_settings_schema_unref(schema);
		return has;
	}

	static void execCommand(const std::string& c){
		int status = std::system(c.c_str());
		if(status != 0) {
			Debug() << "Failed to exec " << c;
		}
	}

	//review it
	void replaceAll(std::string& text, std::string_view from, std::string_view to){
	    if (from.empty())
	        return;
	
	    std::size_t pos = 0;
	    while ((pos = text.find(from, pos)) != std::string::npos) {
	        text.replace(pos, from.size(), to);
	        pos += to.size();
	    }
	}

	void desktopEnvironmentInit(){
		if (desktop != "uninitialized")
    		return;

		desktop = shState->oneshot().desktopEnv;
		if (desktop == "nope") {
    			return;
		}
		if (desktop == "lxde"){
			const char* homeC = SDL_getenv("HOME");
			if (!homeC) return;
			std::string home(homeC);
			std::string path = home + "/.config/pcmanfm/LXDE/desktop-items-0.conf";

			std::ifstream infile(path);
			if (!infile) {
			   Debug() << "Can't open LXDE settings";
			   return;
			}
			
			std::string line;
			unsigned int lineNumber = 0;
			static bool first_found = false;
			static bool second_found = false;
			while (getline(infile, line)) {
				lineNumber++;
				if (line.find("wallpaper=") != std::string::npos) {
					auto pos = line.find("=");
					if (pos != std::string::npos){
					    originalBgPath = line.substr(pos+1);
					    first_found = true;
					}
			    }
			    if (line.find("wallpaper_mode=") != std::string::npos) {
			    	auto pos = line.find("=");
			    	if (pos != std::string::npos){
			    	    originalBgMode = line.substr(pos+1);
			    	    second_found = true;
			    	}
			    }
			    if(first_found && second_found){
			    	Debug() << originalBgPath;
			    	Debug() << originalBgMode;
			    	break;
			    }
			}
			infile.close();
		}
		//just reuse code :3
		if (desktop == "lxqt"){
			DBUS_SESSION_BUS_ADDRESS = SDL_getenv("DBUS_SESSION_BUS_ADDRESS");
			const char* homeC = SDL_getenv("HOME");
			if (!homeC) return;
			std::string home(homeC);
			std::string path = home + "/.config/pcmanfm-qt/lxqt/settings.conf";
			std::ifstream infile(path);
			if (!infile) {
			   Debug() << "Can't open LXDE settings";
			   return;
			}
			std::string line;
			unsigned int lineNumber = 0;
			static bool first_found = false;
			static bool second_found = false;
			while (getline(infile, line)) {
				lineNumber++;
				if (line.find("wallpaper=") != std::string::npos) {
					auto pos = line.find("=");
					if (pos != std::string::npos){
					    originalBgPath = line.substr(pos+1);
					    first_found = true;
					}
			    }
			    if (line.find("wallpaper_mode=") != std::string::npos) {
			    	auto pos = line.find("=");
			    	if (pos != std::string::npos){
			    	    originalBgMode = line.substr(pos+1);
			    	    second_found = true;
			    	}
			    }
			    if(first_found && second_found){
			    	Debug() << originalBgPath;
			    	Debug() << originalBgMode;
			    	break;
			    }
			}
			infile.close();
		}
		if (desktop == "cinnamon" || desktop == "gnome" || desktop == "mate" || desktop == "deepin" || desktop == "budgie" || desktop == "pantheon") {
			if (desktop == "cinnamon" || desktop == "gnome" || desktop == "deepin" || desktop == "budgie" || desktop == "pantheon") {
				if (desktop == "cinnamon") bgsetting = g_settings_new("org.cinnamon.desktop.background");
				else if (desktop == "deepin") bgsetting = g_settings_new("com.deepin.wrap.gnome.desktop.background");
				else bgsetting = g_settings_new("org.gnome.desktop.background");
				defPictureURI = g_settings_get_string(bgsetting, "picture-uri");
				hasPictureURIDark = gsettingsHasKey(bgsetting, "picture-uri-dark");
				if (hasPictureURIDark)
					defPictureURIDark = g_settings_get_string(bgsetting, "picture-uri-dark");
			} else {
				bgsetting = g_settings_new("org.mate.background");
				defPictureURI = g_settings_get_string(bgsetting, "picture-filename");
			}
			defPictureOptions = g_settings_get_string(bgsetting, "picture-options");
			defPrimaryColor = g_settings_get_string(bgsetting, "primary-color");
			defColorShading = g_settings_get_string(bgsetting, "color-shading-type");
		} else if (desktop == "xfce") {
			#ifdef API_ONESHOT_EXTENSIONS_XFCE
				GError *xferror = NULL;
				if(xfconf_init(&xferror)) {
					bgchannel = xfconf_channel_get("xfce4-desktop");

					GHashTable *props = xfconf_channel_get_properties(bgchannel, "/backdrop");
					if(props) {
						const std::string suffix = "/last-image";
						GHashTableIter iter;
						gpointer key, value;
						g_hash_table_iter_init(&iter, props);
						while(g_hash_table_iter_next(&iter, &key, &value)) {
							std::string propPath((const char*)key);
							if (propPath.size() > suffix.size() &&
						    	propPath.compare(propPath.size() - suffix.size(), suffix.size(), suffix) == 0) {
								xfceMonitorPrefixes.push_back(propPath.substr(0, propPath.size() - suffix.size() + 1));
							}
						}
						g_hash_table_destroy(props);
					}
					if (xfceMonitorPrefixes.empty()) {
						xfceMonitorPrefixes.push_back("/backdrop/screen0/monitor0/workspace0/");
					}

					xfceHasSingleWorkspaceProps = xfconf_channel_has_property(bgchannel, "/backdrop/single-workspace-mode");
					if (xfceHasSingleWorkspaceProps) {
						xfceSingleWorkspaceMode = xfconf_channel_get_bool(bgchannel, "/backdrop/single-workspace-mode", true);
						xfceSingleWorkspaceNumber = xfconf_channel_get_int(bgchannel, "/backdrop/single-workspace-number", 0);
					}
					if (xfceHasSingleWorkspaceProps && xfceSingleWorkspaceMode) {
						std::vector<std::string> extraPrefixes;
						for (const std::string &prefix : xfceMonitorPrefixes) {
							std::size_t workspacePos = prefix.rfind("/workspace");
							if (workspacePos == std::string::npos)
								continue;
							std::string monitorBase = prefix.substr(0, workspacePos + 1);
							std::string activePrefix = monitorBase + "workspace" + std::to_string(xfceSingleWorkspaceNumber) + "/";
							if (std::find(xfceMonitorPrefixes.begin(), xfceMonitorPrefixes.end(), activePrefix) == xfceMonitorPrefixes.end() &&
						    	std::find(extraPrefixes.begin(), extraPrefixes.end(), activePrefix) == extraPrefixes.end()) {
								extraPrefixes.push_back(activePrefix);
							}
						}
						xfceMonitorPrefixes.insert(xfceMonitorPrefixes.end(), extraPrefixes.begin(), extraPrefixes.end());
					}

					for (const std::string &prefix : xfceMonitorPrefixes) {
						defXfcePictureURI[prefix] = xfconf_channel_get_string(bgchannel, (prefix + "last-image").c_str(), "");
						defXfcePictureStyle[prefix] = xfconf_channel_get_int(bgchannel, (prefix + "image-style").c_str(), -1);
						defXfceColorStyle[prefix] = xfconf_channel_get_int(bgchannel, (prefix + "color-style").c_str(), -1);
						GValue colorVal = G_VALUE_INIT;
						defXfceColorExists[prefix] = xfconf_channel_get_property(bgchannel, (prefix + "color1").c_str(), &colorVal);
						defXfceColor[prefix] = colorVal;
					}
				} else {
					// Configuration failed to initialize, we won't set the wallpaper
					Debug() << "Configuration failed to initialize, we won't set the wallpaper";
					desktop = "xfce_error";
					g_error_free(xferror);
				}
			#else
				Debug() << "XFCE4 support disabled in this build!";
			#endif
		} else if (desktop == "kde") {
			std::ifstream configFile;
			configFile.open(std::string(getenv("HOME")) + "/.config/plasma-org.kde.plasma.desktop-appletsrc", std::ios::in);
			if (configFile.is_open()) {
				std::string line;
				std::vector<std::string> sections;
				std::size_t undefined = 999999999;
				bool readPlugin = false, readOther = false;
				std::string containment;
				while (getline(configFile, line)) {
					std::size_t index = undefined, lastIndex = undefined;
					if (line.size() == 0) {
						readPlugin = false;
						readOther = false;
					} else if (readPlugin) {
						index = line.find('=');
						if (line.substr(0, index) == "wallpaperplugin") {
							defPlugins[containment] = line.substr(index + 1);
						}
					} else if (readOther) {
						index = line.find('=');
						std::string key = line.substr(0, index);
						std::string val = line.substr(index + 1);
						if (key == "Image") {
							defPictures[containment] = val;
						} else if (key == "Color") {
							defColors[containment] = val;
						} else if (key == "FillMode") {
							defModes[containment] = val;
						} else if (key == "Blur") {
							defBlurs[containment] = (val == "true");
						}
					} else if (line.at(0) == '[') {
						sections.clear();
						while (true) {
							index = line.find(lastIndex == undefined ? '[' : ']', index == undefined ? 0 : index);
							if (index == std::string::npos) {
								break;
							}
							if (lastIndex == undefined) {
								lastIndex = index;
							} else {
								sections.push_back(line.substr(lastIndex + 1, index - lastIndex - 1));
								lastIndex = undefined;
							}
						}
						if (sections.size() == 2 && sections[0] == "Containments") {
							readPlugin = true;
							containment = sections[1];
						} else if (
							sections.size() == 5 &&
							sections[0] == "Containments" &&
							sections[2] == "Wallpaper" &&
							sections[3] == "org.kde.image" &&
							sections[4] == "General"
						) {
							readOther = true;
							containment = sections[1];
						}
					}
				}
				configFile.close();
			} else {
				Debug() << "FATAL: Cannot find desktop configuration!";
				desktop = "kde_error";
			}
		} else {
			const char* homeC = SDL_getenv("HOME");
			std::string home = homeC ? homeC : "";
			std::string fehbgPath = home + "/.fehbg";
			std::string nitrogenPath = home + "/.config/nitrogen/bg-saved.cfg";

			struct stat fehStat, nitrogenStat;
			bool fehExists = !home.empty() && stat(fehbgPath.c_str(), &fehStat) == 0;
			bool nitrogenExists = !home.empty() && stat(nitrogenPath.c_str(), &nitrogenStat) == 0;

			if (fehExists && (!nitrogenExists || fehStat.st_mtime >= nitrogenStat.st_mtime)) {
				wpTool = "feh";
				std::ifstream infile(fehbgPath);
				if (infile) {
					std::stringstream buffer;
					buffer << infile.rdbuf();
					originalFehbgCmd = buffer.str();
					originalFehbgExists = !originalFehbgCmd.empty();
					infile.close();
				}
			} else if (nitrogenExists) {
				wpTool = "nitrogen";
			} else if (std::system("command -v feh >/dev/null 2>&1") == 0) {
				wpTool = "feh";
			} else if (std::system("command -v nitrogen >/dev/null 2>&1") == 0) {
				wpTool = "nitrogen";
			} else {
				fallbackPath = home + "/Desktop/ONESHOT_hint.png";
			}
		}
	}


	static VALUE wallpaperSet(int argc, VALUE *argv, VALUE self){
		const char *name;
		int color;
		rb_get_args(argc, argv, "zi", &name, &color RB_ARG_END);
		std::string path;

		std::string nameFix(name);
		std::size_t found = nameFix.find("w32");
		if(found != std::string::npos) {
			nameFix.replace(nameFix.end()-3, nameFix.end(), "unix");
		}
		path = "/Wallpaper/" + nameFix + ".png";
		#ifndef NDEBUG
			Debug() << "Setting wallpaper to " << path;
		#endif
		char gameDir[PATH_MAX];

		std::string gameDirStr(gameDir);
		desktopEnvironmentInit();
		if(desktop == "cinnamon" || desktop == "gnome" || desktop == "mate" || desktop == "deepin" || desktop == "budgie" || desktop == "pantheon") {
			std::stringstream hexColor;
			hexColor << "#" << std::hex << color;
			g_settings_set_string(bgsetting, "picture-options", "scaled");
			g_settings_set_string(bgsetting, "primary-color", hexColor.str().c_str());
			g_settings_set_string(bgsetting, "color-shading-type", "solid");
			if(desktop == "cinnamon" || desktop == "gnome" || desktop == "deepin" || desktop == "budgie" || desktop == "pantheon") {
				g_settings_set_string(bgsetting, "picture-uri", ("file://" + gameDirStr + path).c_str());
				if(hasPictureURIDark)
					g_settings_set_string(bgsetting, "picture-uri-dark", ("file://" + gameDirStr + path).c_str());
				}else{
					g_settings_set_string(bgsetting, "picture-filename", (gameDirStr + path).c_str());
				}
			} else if (desktop == "xfce") {
				#ifdef API_ONESHOT_EXTENSIONS_XFCE
				int r = (color >> 16) & 0xFF;
				int g = (color >> 8) & 0xFF;
				int b = color & 0xFF;
				unsigned int ur = r * 256 + r;
				unsigned int ug = g * 256 + g;
				unsigned int ub = b * 256 + b;
				unsigned int alpha = 65535;
				std::string concatPath(gameDirStr + path);
				for(const std::string &prefix : xfceMonitorPrefixes) {
					std::string optionImage = prefix + "last-image";
					std::string optionColor = prefix + "color1";
					std::string optionImageStyle = prefix + "image-style";
					std::string optionColorStyle = prefix + "color-style";
					xfconf_channel_set_string(bgchannel, optionImage.c_str(), concatPath.c_str());
					xfconf_channel_set_int(bgchannel, optionColorStyle.c_str(), 0);
					xfconf_channel_set_int(bgchannel, optionImageStyle.c_str(), 4);
					GPtrArray *colorArr = g_ptr_array_sized_new(4);
					GValue *vr = g_new0(GValue, 1);
					GValue *vg = g_new0(GValue, 1);
					GValue *vb = g_new0(GValue, 1);
					GValue *va = g_new0(GValue, 1);
					g_value_init(vr, G_TYPE_UINT);
					g_value_init(vg, G_TYPE_UINT);
					g_value_init(vb, G_TYPE_UINT);
					g_value_init(va, G_TYPE_UINT);
					g_value_set_uint(vr, ur);
					g_value_set_uint(vg, ug);
					g_value_set_uint(vb, ub);
					g_value_set_uint(va, alpha);
					g_ptr_array_add(colorArr, vr);
					g_ptr_array_add(colorArr, vg);
					g_ptr_array_add(colorArr, vb);
					g_ptr_array_add(colorArr, va);
					if(!xfconf_channel_set_arrayv(bgchannel, optionColor.c_str(), colorArr)) {
						Debug() << "WALLPAPER ERROR: xfconf_channel_set_arrayv failed for" << optionColor;
					}
					g_value_unset(vr);
					g_value_unset(vg);
					g_value_unset(vb);
					g_value_unset(va);
					g_free(vr);
					g_free(vg);
					g_free(vb);
					g_free(va);
					g_ptr_array_free(colorArr, TRUE);
				}
				execCommand("xfdesktop --reload &");
			#else
				Debug() << "XFCE support disabled in this build";
			#endif
		} else if (desktop == "kde") {
			std::stringstream command;
			std::string concatPath(gameDirStr + path);
			replaceAll(concatPath, "\\", "\\\\");
			replaceAll(concatPath, "\"", "\\\"");
			replaceAll(concatPath, "'", "\\x27");
			command << "qdbus6 org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string:" <<
				"var allDesktops = desktops();" <<
				"for (var i = 0, l = allDesktops.length; i < l; ++i) {" <<
					"var d = allDesktops[i];" <<
					"d.wallpaperPlugin = \"org.kde.image\";" <<
					"d.currentConfigGroup = [\"Wallpaper\", \"org.kde.image\", \"General\"];" <<
					"d.writeConfig(\"Image\", \"file://" << concatPath << "\");" <<
					"d.writeConfig(\"FillMode\", \"6\");" <<
					"d.writeConfig(\"Blur\", false);" <<
					"d.writeConfig(\"Color\", [\"" <<
						std::to_string((color >> 16) & 0xFF) << "\", \"" <<
						std::to_string((color >> 8) & 0xFF) << "\", \"" <<
						std::to_string(color & 0xFF) <<
					"\"]);" <<
				"}" <<
			"'";
			execCommand(command.str().c_str());
		} else if (desktop == "lxde") {
			std::string concatPath = gameDirStr + path;
			std::string cmd = "pcmanfm -w \"" + concatPath + "\"" + " --wallpaper-mode=center";
			execCommand(cmd);
		} else if (desktop == "lxqt") {
			std::string concatPath = gameDirStr + path;
			std::string cmd = "DBUS_SESSION_BUS_ADDRESS=" + DBUS_SESSION_BUS_ADDRESS + " pcmanfm-qt -w \"" + concatPath + "\"" + " --wallpaper-mode=center";
			execCommand(cmd);
		} else if (wpTool == "feh") {
			std::string concatPath = gameDirStr + path;
			std::string cmd = "feh --bg-scale \"" + concatPath + "\"";
			execCommand(cmd);
		} else if (wpTool == "nitrogen") {
			std::string concatPath = gameDirStr + path;
			std::string cmd = "nitrogen --set-scaled \"" + concatPath + "\"";
			execCommand(cmd);
		} else {
			std::ifstream srcHint(gameDirStr + path);
			std::ofstream dstHint(fallbackPath);
			dstHint << srcHint.rdbuf();
			srcHint.close();
			dstHint.close();
		}
		return Qnil;
	}

	static VALUE wallpaperReset(VALUE self){
		desktopEnvironmentInit();
		if(desktop == "cinnamon" || desktop == "gnome" || desktop == "mate" || desktop == "deepin" || desktop == "budgie" || desktop == "pantheon") {
			if (desktop == "cinnamon" || desktop == "gnome" || desktop == "deepin" || desktop == "budgie" || desktop == "pantheon") {
				g_settings_set_string(bgsetting, "picture-uri", defPictureURI.c_str());
				if (hasPictureURIDark)
					g_settings_set_string(bgsetting, "picture-uri-dark", defPictureURIDark.c_str());
				}else{
					g_settings_set_string(bgsetting, "picture-filename", defPictureURI.c_str());
				}
				g_settings_set_string(bgsetting, "picture-options", defPictureOptions.c_str());
				g_settings_set_string(bgsetting, "primary-color", defPrimaryColor.c_str());
				g_settings_set_string(bgsetting, "color-shading-type", defColorShading.c_str());
			}else if (desktop == "xfce") {
				#ifdef API_ONESHOT_EXTENSIONS_XFCE
					for(const std::string &prefix : xfceMonitorPrefixes) {
						std::string optionImage = prefix + "last-image";
						std::string optionColor = prefix + "color1";
						std::string optionImageStyle = prefix + "image-style";
						std::string optionColorStyle = prefix + "color-style";

						if (defXfceColorExists[prefix]) {
							xfconf_channel_set_property(bgchannel, optionColor.c_str(), &defXfceColor[prefix]);
						} else {
							xfconf_channel_reset_property(bgchannel, optionColor.c_str(), false);
						}
						if (defXfcePictureURI[prefix] == "") {
							xfconf_channel_reset_property(bgchannel, optionImage.c_str(), false);
						} else {
							xfconf_channel_set_string(bgchannel, optionImage.c_str(), defXfcePictureURI[prefix].c_str());
						}
						if (defXfcePictureStyle[prefix] == -1) {
							xfconf_channel_reset_property(bgchannel, optionImageStyle.c_str(), false);
						} else {
							xfconf_channel_set_int(bgchannel, optionImageStyle.c_str(), defXfcePictureStyle[prefix]);
						}
						if (defXfceColorStyle[prefix] == -1) {
							xfconf_channel_reset_property(bgchannel, optionColorStyle.c_str(), false);
						} else {
							xfconf_channel_set_int(bgchannel, optionColorStyle.c_str(), defXfceColorStyle[prefix]);
						}
					}
					if (xfceHasSingleWorkspaceProps && xfceSingleWorkspaceMode) {
						xfconf_channel_set_bool(bgchannel, "/backdrop/single-workspace-mode", false);
						xfconf_channel_set_bool(bgchannel, "/backdrop/single-workspace-mode", true);
					}
					execCommand("xfdesktop --reload &");
				#else
					Debug() << "XFCE support disabled in this build";
				#endif
			} else if (desktop == "kde") {
				std::stringstream command;
				command << "qdbus6 org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string:" <<
						"var allDesktops = desktops();" <<
						"var data = {";
				// Plugin, picture, color, mode, blur
				for (auto const& x : defPlugins) {
					command << "\"" << x.first << "\": {"
							<< "plugin: \"" << x.second << "\"";
					if (defPictures.find(x.first) != defPictures.end()) {
						std::string picture = defPictures[x.first];
						replaceAll(picture, "\\", "\\\\");
						replaceAll(picture, "\"", "\\\"");
						replaceAll(picture, "'", "\\x27");
						command << ", picture: \"" << picture << "\"";
					}
					if (defColors.find(x.first) != defColors.end()) {
						command << ", color: \"" << defColors[x.first] << "\"";
					}
					if (defModes.find(x.first) != defModes.end()) {
						command << ", mode: \"" << defModes[x.first] << "\"";
					}
					if (defBlurs.find(x.first) != defBlurs.end() && defBlurs[x.first]) {
						command << ", blur: true";
					}
					command << "},";
				}
				command << "\"no\": {}};" <<
					"for (var i = 0, l = allDesktops.length; i < l; ++i) {" <<
						"var d = allDesktops[i];" <<
						"var dat = data[d.id];" <<
						"d.wallpaperPlugin = dat.plugin;" <<
						"d.currentConfigGroup = [\"Wallpaper\", \"org.kde.image\", \"General\"];" <<
						"if (dat.picture) {" <<
						"d.writeConfig(\"Image\", dat.picture);" <<
						"}" <<
						"if (dat.color) {" <<
						"d.writeConfig(\"Color\", dat.color.split(\",\"));" <<
						"}" <<
						"if (dat.mode) {" <<
						"d.writeConfig(\"FillMode\", dat.mode);" <<
						"}" <<
						"if (dat.blur) {" <<
						"d.writeConfig(\"Blur\", dat.blur);" <<
						"}" <<
					"}" <<
				"'";		
				execCommand(command.str().c_str());
			} else if(desktop == "lxde"){
				if (originalBgPath != "" && originalBgMode != ""){
					std::string cmd = "pcmanfm -w \"" + originalBgPath + "\"" + " --wallpaper-mode=" + originalBgMode;
					execCommand(cmd);
				}
			} else if(desktop == "lxqt"){
				if (originalBgPath != "" && originalBgMode != ""){
					std::string cmd = "DBUS_SESSION_BUS_ADDRESS=" + DBUS_SESSION_BUS_ADDRESS + " pcmanfm-qt -w \"" + originalBgPath + "\"" + " --wallpaper-mode=" + originalBgMode;
					execCommand(cmd);
				}
			} else if (wpTool == "feh") {
				std::string cmd = originalFehbgExists ? originalFehbgCmd : "xsetroot -solid black";
				execCommand(cmd);
			} else if (wpTool == "nitrogen") {
				execCommand("nitrogen --restore");
			}
		return Qnil;
	}

	void wallpaperBindingTerminate(){
		// Clean up.
		#ifdef API_ONESHOT_EXTENSIONS_XFCE
			if(desktop == "xfce") {
				xfconf_shutdown();
			}
		#endif
	}
#endif
