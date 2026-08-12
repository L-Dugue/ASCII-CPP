
#include <iostream>

#include <Windows.h>

// Screen Dimensions
int _screenWidth = 120;
int _screenHeight = 40;

// Player Position/Rotation
float fPlayerX = 0.f;
float fPlayerY = 0.f;
float fPlayerA = 0.f;

// Map Dimensions
float _mapWidth = 16;
float _mapHeight = 16;


int main()
{
	// Screen Buffer Creation
	wchar_t* screen = new wchar_t[_screenWidth * _screenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// Map creation
	std::wstring map;
	const wchar_t* wall = L"################";
	const wchar_t* emptySpace = L"#..............#";

	// Iterate through it all, create a big room.
	for (int i = 0; i < _mapWidth; i++) 
	{
		if (i == 0 || i == 15) 
		{
			map += wall;
		}
		else
		{
			map += emptySpace;
		}
	}
	

	while (true) // Game Loop
	{
		// Computation for each column of the screen along the X.
		for (int i = 0; i < _screenWidth; i++)
		{
			// Computation
		}

		// Write to the Screen
		screen[_screenWidth * _screenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, _screenWidth * _screenWidth, { 0, 0 }, &dwBytesWritten);
	}
	
	return 0;
}