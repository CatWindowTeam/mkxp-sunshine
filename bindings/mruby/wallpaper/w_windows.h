#pragma once
#include <SDL3/SDL_platform_defines.h>
#ifdef SDL_PLATFORM_WINDOWS
	#include <windows.h>
	#include <mruby.h>
	#include <string>
	#include <SDL3/SDL_stdinc.h>
	#include "etc.h"
	#include "sharedstate.h"
	#include "config.h"
	#include "oneshot.h"
	#include "debugwriter.h"
	
	static WCHAR szStyle[8] = {0};
	static WCHAR szTile[8] = {0};
	static WCHAR szFile[MAX_PATH+1] = {0};
	static DWORD oldcolor = 0;
	static DWORD szStyleSize = sizeof(szStyle) - 1;
	static DWORD szTileSize = sizeof(szTile) - 1;
	static bool setStyle = false;
	static bool setTile = false;
	static bool isCached = false;

	static mrb_value wallpaperSet(mrb_state *mrb, mrb_value self){
		const char *name;
		int color;
		mrb_get_args(mrb, "zi", &name, &color);
		std::string path;
		path = conf.gameFolder + "\\Wallpaper\\" + name + ".bmp";
		#ifndef NDEBUG
			Debug() << "Setting wallpaper to " << path;
		#endif
		// Crapify the slashes
		size_t index = 0;
		for(;;){
			index = path.find("/", index);
			if(index == std::string::npos){
				break;
			}
			path.replace(index, 1, "\\");
			index += 1;
		}
		WCHAR imgnameW[MAX_PATH];
		WCHAR imgnameFull[MAX_PATH];
		MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, imgnameW, MAX_PATH);
		GetFullPathNameW(imgnameW, MAX_PATH, imgnameFull, NULL);

		int colorId = COLOR_BACKGROUND;
		WCHAR zero[2] = L"0";
		DWORD zeroSize = 4;
		HKEY hKey = NULL;
		if(RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey) != ERROR_SUCCESS){
			if(hKey){
				RegCloseKey(hKey);
			}
		}

		if(!isCached) {
			// QUERY
			// Style
			setStyle = RegQueryValueExW(hKey, L"WallpaperStyle", 0, NULL, (LPBYTE)(szStyle), &szStyleSize) == ERROR_SUCCESS;
			// Tile
			setTile = RegQueryValueExW(hKey, L"TileWallpaper", 0, NULL, (LPBYTE)(szTile), &szTileSize) == ERROR_SUCCESS;
			// File path
			if(!SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, (PVOID)szFile, 0)){
				if(hKey){
					RegCloseKey(hKey);
				}
			}

			// Color
			oldcolor = GetSysColor(COLOR_BACKGROUND);
			isCached = true;
		}

		RegCloseKey(hKey);
		hKey = NULL;
		if(RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_WRITE, &hKey) != ERROR_SUCCESS){
			if(hKey){
				RegCloseKey(hKey);
			}
		}
			
		// SET
		// Set the style
		if(RegSetValueExW(hKey, L"WallpaperStyle", 0, REG_SZ, (const BYTE*)zero, zeroSize) != ERROR_SUCCESS){
			if(hKey){
				RegCloseKey(hKey);
			}
		}

		if(RegSetValueExW(hKey, L"TileWallpaper", 0, REG_SZ, (const BYTE*)zero, zeroSize) != ERROR_SUCCESS){
			if(hKey){
				RegCloseKey(hKey);
			}
		}

		// Set the wallpaper
		if(!SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)imgnameFull, SPIF_UPDATEINIFILE)){
			if(hKey){
				RegCloseKey(hKey);
			}
		}

		// Set the color
		if(!SetSysColors(1, &colorId, (const COLORREF *)&color)){
			if(hKey){
				RegCloseKey(hKey);
			}
		}
		return mrb_nil_value();
	}

	static mrb_value wallpaperReset(mrb_state *mrb, mrb_value self){
		if (isCached) {
			int colorId = COLOR_BACKGROUND;
			HKEY hKey = NULL;
			if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_WRITE, &hKey) != ERROR_SUCCESS){
				if(hKey){
					RegCloseKey(hKey);
				}
			}

			// Set the style
			if (setStyle){
				RegSetValueExW(hKey, L"WallpaperStyle", 0, REG_SZ, (const BYTE*)szStyle, szStyleSize);
			}

			if (setTile){
				RegSetValueExW(hKey, L"TileWallpaper", 0, REG_SZ, (const BYTE*)szTile, szTileSize);
			}

			// Set the wallpaper
			if (!SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)szFile, SPIF_UPDATEINIFILE)){
				if(hKey){ RegCloseKey(hKey); }
			}

			// Set the color
			if (!SetSysColors(1, &colorId, (const COLORREF *)&oldcolor)){
				if(hKey){ RegCloseKey(hKey); }
			}
		}
		return mrb_nil_value();
	}
#endif
