/*
  This file is not licenced under the GPL like the rest of the code.
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2007 Red Hat, Inc.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "xdg-user-dir-lookup.h"

#include <SDL3/SDL_stdinc.h>
/**
 * xdg_user_dir_lookup_with_fallback:
 * @type: a string specifying the type of directory
 * @fallback: value to use if the directory isn't specified by the user
 * @returns: a newly allocated absolute pathname
 *
 * Looks up a XDG user directory of the specified type.
 * Example of types are "DESKTOP" and "DOWNLOAD".
 *
 * In case the user hasn't specified any directory for the specified
 * type the value returned is @fallback.
 *
 * The return value is newly allocated and must be freed with
 * SDL_free(). The return value is never NULL if @fallback != NULL, unless
 * out of memory.
 **/
char * xdg_user_dir_lookup_with_fallback (const char *type, const char *fallback) {
  FILE *file;
  char *home_dir, *config_home, *config_file;
  char buffer[512];
  char *user_dir;
  char *p, *d;
  int len;
  int relative;

  home_dir = SDL_getenv("HOME");

  if (home_dir == NULL)
    goto error;

  config_home = SDL_getenv("XDG_CONFIG_HOME");
  if (config_home == NULL || config_home[0] == 0){
      config_file = (char*)SDL_malloc(SDL_strlen(home_dir) + 23);
      if (config_file == NULL)
        goto error;

      SDL_strlcpy(config_file, home_dir, sizeof(config_file));
      SDL_strlcat(config_file, "/.config/user-dirs.dirs", sizeof(config_file) + 22);
    }
  else{
      config_file = (char*)SDL_malloc(SDL_strlen(config_home) + SDL_strlen("/user-dirs.dirs") + 1);
      if (config_file == NULL)
        goto error;

      SDL_strlcpy(config_file, config_home, sizeof(config_file));
      SDL_strlcat(config_file, "/user-dirs.dirs", sizeof(config_file) + 16);
    }

  file = fopen(config_file, "r");
  SDL_free(config_file);
  if (file == NULL)
    goto error;

  user_dir = NULL;
  while(fgets (buffer, sizeof (buffer), file)){
      /* Remove newline at end */
      len = SDL_strlen(buffer);
      if (len > 0 && buffer[len-1] == '\n')
	buffer[len-1] = 0;

      p = buffer;
      while (*p == ' ' || *p == '\t')
	p++;

      if(SDL_strncmp(p, "XDG_", 4) != 0)
	continue;
      p += 4;
      if(SDL_strncmp (p, type, SDL_strlen(type)) != 0)
	continue;
      p += SDL_strlen(type);
      if(SDL_strncmp(p, "_DIR", 4) != 0)
	continue;
      p += 4;

      while (*p == ' ' || *p == '\t')
	p++;

      if (*p != '=')
	continue;
      p++;
      
      while (*p == ' ' || *p == '\t')
	p++;

      if (*p != '"')
	continue;
      p++;
      
      relative = 0;
      if(SDL_strncmp(p, "$HOME/", 6) == 0){
	  p += 6;
	  relative = 1;
	}
      else if (*p != '/')
	continue;
      
      if (relative){
	  user_dir = (char*)SDL_malloc(SDL_strlen(home_dir) + 1 + SDL_strlen(p) + 1);
          if (user_dir == NULL)
            goto error2;

	  SDL_strlcpy(user_dir, home_dir, sizeof(user_dir));
	  SDL_strlcat(user_dir, "/", sizeof(user_dir) + 2);
	}
      else{
	  user_dir = (char*)SDL_malloc(SDL_strlen(p) + 1);
          if (user_dir == NULL)
            goto error2;

	  *user_dir = 0;
	}
      
      d = user_dir + SDL_strlen(user_dir);
      while (*p && *p != '"'){
	  if ((*p == '\\') && (*(p+1) != 0))
	    p++;
	  *d++ = *p++;
	}
      *d = 0;
    }
error2:
  fclose(file);

  if (user_dir)
    return user_dir;

 error:
  if (fallback)
    return SDL_strdup(fallback);
  return NULL;
}

/**
 * xdg_user_dir_lookup:
 * @type: a string specifying the type of directory
 * @returns: a newly allocated absolute pathname
 *
 * Looks up a XDG user directory of the specified type.
 * Example of types are "DESKTOP" and "DOWNLOAD".
 *
 * The return value is always != NULL (unless out of memory),
 * and if a directory
 * for the type is not specified by the user the default
 * is the home directory. Except for DESKTOP which defaults
 * to ~/Desktop.
 *
 * The return value is newly allocated and must be freed with
 * SDL_free().
 **/
char *xdg_user_dir_lookup (const char *type){
  char *dir, *home_dir, *user_dir;
	  
  dir = xdg_user_dir_lookup_with_fallback(type, NULL);
  if (dir != NULL)
    return dir;
  
  home_dir = SDL_getenv("HOME");
  
  if (home_dir == NULL)
    return SDL_strdup("/tmp");
  
  /* Special case desktop for historical compatibility */
  if (SDL_strcmp(type, "DESKTOP") == 0){
      user_dir = (char*)SDL_malloc(SDL_strlen(home_dir) + SDL_strlen("/Desktop") + 1);
      if (user_dir == NULL)
        return NULL;

      SDL_strlcpy(user_dir, home_dir, sizeof(user_dir));
      SDL_strlcat(user_dir, "/Desktop", sizeof(user_dir) + 9);
      return user_dir;
    }
  return SDL_strdup(home_dir);
}
