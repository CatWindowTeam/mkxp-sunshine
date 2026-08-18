#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_net/SDL_net.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_thread.h>

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "debugwriter.h"

#include "icon.png.xxd"
#include "default.png.xxd"

static bool quit = false;
static char is_changed = false;
static char image[256] = "default";
static char image_full_path[1024] = "default";
static char gamedir_path[256] = "";
static char address[64] = "127.0.0.1";

Uint16 server_port = 23821;
static void trim_crlf_inplace(char* s) {
    if (!s) return;
    size_t n = SDL_strlen(s);
    while (n > 0 && (s[n - 1] == '\r' || s[n - 1] == '\n')) {
        s[n - 1] = '\0';
        n--;
    }
}

static int network_thread(void* /*data*/) {
    NET_Address* server_addr = NET_ResolveHostname("127.0.0.1");
    if (!server_addr || (NET_WaitUntilResolved(server_addr, -1) != NET_SUCCESS)) {
        if (server_addr) { NET_UnrefAddress(server_addr); }
        return 1;
    }

    NET_Server* server = NET_CreateServer(server_addr, server_port, 0);
    if (!server) {
        SDL_Log("Failed to create server: %s", SDL_GetError());
        return 1;
    }

    SDL_Log("Port %d!", (int)server_port);

    int num_vsockets = 1;
    void* vsockets[128];
    SDL_zeroa(vsockets);
    vsockets[0] = server;
    int wait_timeout_ms = quit ? 10 : 50;

    while (!quit && NET_WaitUntilInputAvailable(vsockets, num_vsockets, wait_timeout_ms) >= 0) {
        NET_StreamSocket* streamsocket = nullptr;
        if (NET_AcceptClient(server, &streamsocket)) {
            if (!quit && streamsocket) {
                SDL_Log("New connection from %s!", NET_GetAddressString(NET_GetStreamSocketAddress(streamsocket)));
                if (num_vsockets >= (int)(SDL_arraysize(vsockets) - 1)) {
                    SDL_Log("  (too many connections, though, so dropping immediately.)");
                    NET_DestroyStreamSocket(streamsocket);
                } else {
                    vsockets[num_vsockets++] = streamsocket;
                }
            } else if (streamsocket) {
                NET_DestroyStreamSocket(streamsocket);
            }
        }

        for (int i = 1; i < num_vsockets; i++) {
            if (quit) break;

            bool kill_socket = false;
            streamsocket = (NET_StreamSocket*)vsockets[i];

            if (!streamsocket) {
                continue;
            }

            char buffer[1024];
            const int br = NET_ReadFromStreamSocket(
                streamsocket, buffer, (int)sizeof(buffer) - 1
            );

            if (br < 0) {
                kill_socket = true;
            } else if (br > 0) {
                buffer[br] = '\0';
                trim_crlf_inplace(buffer);

                if (buffer[0] != '\0') {
                    if (SDL_snprintf(image, sizeof(image), "%s", buffer) >= 0) {
                        is_changed = true;
                    } else {
                        Debug() << "SDL_snprintf error";
                    }
                }
                kill_socket = true;
            }

            if (kill_socket) {
                SDL_Log("Dropping connection to '%s'", NET_GetAddressString(NET_GetStreamSocketAddress(streamsocket)));
                NET_DestroyStreamSocket(streamsocket);
                vsockets[i] = nullptr;
                if (i < (num_vsockets - 1)) {
                    SDL_memmove(&vsockets[i], &vsockets[i + 1], sizeof(vsockets[0]) * ((num_vsockets - i) - 1));
                }
                num_vsockets--;
                i--;
            }
        }
    }
    for (int i = 1; i < num_vsockets; i++) {
        if (vsockets[i]) {
            NET_DestroyStreamSocket((NET_StreamSocket*)vsockets[i]);
            vsockets[i] = nullptr;
        }
    }
    NET_DestroyServer(server);
    return 0;
}


int main(int argc, char* argv[]) {
    // get game dir path
    std::ifstream file(std::filesystem::temp_directory_path() / "sunshine");
    if (file.is_open()) {
        std::string filePath;
        std::getline(file, filePath);
        SDL_strlcpy(gamedir_path, filePath.c_str(), sizeof(gamedir_path) - 1);
        gamedir_path[sizeof(gamedir_path) - 1] = '\0';
        file.close();
    } else {
        Debug() << "Unable to open file";
    }
    Debug() << gamedir_path;

	for (int i = 1; i < argc; i++) {
	    const char* arg = argv[i];
	    if ((SDL_strcmp(arg, "--port") == 0) && (i < (argc - 1))) {
	        server_port = (Uint16)SDL_atoi(argv[++i]);
	    }
	    if ((SDL_strcmp(arg, "--addr") == 0) && (i < (argc - 1))) {
	        strncpy(address, argv[++i], sizeof(address) - 1);
	        address[sizeof(address) - 1] = '\0';
	    }
	    if ((SDL_strcmp(arg, "--game-path") == 0) && (i < (argc - 1))) {
	        strncpy(gamedir_path, argv[++i], sizeof(gamedir_path) - 1);
	        gamedir_path[sizeof(gamedir_path) - 1] = '\0';
	    }
	}

    // initialization
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Debug() << "SDL_Init failed: " << SDL_GetError();
        return 1;
    }

    if (!NET_Init()) {
        Debug() << "NET_Init failed: " << SDL_GetError();
        SDL_Quit();
        return 1;
    }

    // window / renderer
    SDL_Window* win = SDL_CreateWindow("_______", 800, 600, SDL_WINDOW_TRANSPARENT);
    if (win == NULL) {
        Debug() << "SDL_CreateWindow Error: " << SDL_GetError();
        NET_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(win, NULL);
    if (ren == NULL) {
        Debug() << "SDL_CreateRenderer Error: " << SDL_GetError();
        SDL_DestroyWindow(win);
        NET_Quit();
        SDL_Quit();
        return 1;
    }

    // default image texture
    SDL_Surface* img0 = IMG_Load_IO(SDL_IOFromConstMem(assets_default_png, assets_default_png_len), true);
    if (!img0) {
        Debug() << "IMG_Load_IO default failed: " << SDL_GetError();
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        NET_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, img0);
    SDL_DestroySurface(img0);

    if (tex == NULL) {
        Debug() << "SDL_CreateTextureFromSurface Error: " << SDL_GetError();
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        NET_Quit();
        SDL_Quit();
        return 1;
    }

    // start network thread
    SDL_Thread* network_thread_pointer = SDL_CreateThread(network_thread, "network", NULL);
    if (!network_thread_pointer) {
        Debug() << "SDL_CreateThread failed";
        SDL_DestroyTexture(tex);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        NET_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }else if(e.type == SDL_EVENT_WINDOW_DESTROYED){
            	quit = true;
            }
        }

        if (is_changed) {
            char local_image[256];
            SDL_strlcpy(local_image, image, sizeof(local_image));
            SDL_Texture* new_tex = nullptr;
            if (SDL_strcmp(local_image, "default") == 0) {
                SDL_Surface* img = IMG_Load_IO(SDL_IOFromConstMem(assets_default_png, assets_default_png_len), true);
                if (!img) {
                    Debug() << "IMG_Load_IO default failed: " << SDL_GetError();
                } else {
                    new_tex = SDL_CreateTextureFromSurface(ren, img);
                    SDL_DestroySurface(img);
                    if (!new_tex) {
                        Debug() << "SDL_CreateTextureFromSurface Error: " << SDL_GetError();
                    }
                }
            } else {
                if (SDL_snprintf(image_full_path, sizeof(image_full_path), "%s/%s", gamedir_path, local_image) < 0) {
                    Debug() << "SDL_snprintf error";
                } else {
                    SDL_Surface* img = IMG_Load(image_full_path);
                    if (!img) {
                        Debug() << "Image loading Error: " << SDL_GetError();
                    } else {
                        new_tex = SDL_CreateTextureFromSurface(ren, img);
                        SDL_DestroySurface(img);
                        if (!new_tex) {
                            Debug() << "SDL_CreateTextureFromSurface Error: " << SDL_GetError();
                        }
                    }
                }
            }

            if (new_tex) {
                SDL_DestroyTexture(tex);
                tex = new_tex;
            }
            is_changed = false;
        }
        SDL_RenderClear(ren);
        SDL_RenderTexture(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }
	SDL_WaitThread(network_thread_pointer, NULL);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    NET_Quit();
    SDL_Quit();
    return 0;
}
