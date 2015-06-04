#include <iostream>
#include <windows.h>

int main()
{
	HANDLE outcon = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_FONT_INFOEX font;
	GetCurrentConsoleFontEx(outcon, false, &font);
	font.dwFontSize.X = 10;
	font.dwFontSize.Y = 18;
	SetCurrentConsoleFontEx(outcon, false, &font);

	SetConsoleTextAttribute(outcon, 0x0C);
	std::cout << "I'm red";
	std::cin.get();

	return 0;
}
