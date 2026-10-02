#pragma once
#include "trstr.h"

void unloadLocale();
void loadLocale(const char* locale);
void decodeEscapeChars(char* s);
const char* findtext(unsigned int msgid, const char* fallback);
void loadLanguageMetadata();
void unloadLanguageMetadata();
int getFontSize();
char* getFontName();
