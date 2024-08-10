#include "Console.h"

Console::Console(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
}

Console::~Console(void)
{
}

void Console::update(void)
{
}

void Console::write(const ExitGames::Common::JString& str)
{
	wprintf(L"%ls", str.cstr());
#if defined _EG_MICROSOFT_PLATFORM
	OutputDebugStringW(str.cstr());
#endif
}

void Console::writeLine(const ExitGames::Common::JString& str)
{
	wprintf(L"%ls\n", str.cstr());
#if defined _EG_MICROSOFT_PLATFORM
	OutputDebugStringW((str+L"\n").cstr());
#endif
}

Console& Console::get(void)
{
	static Console console;
	return console;
}