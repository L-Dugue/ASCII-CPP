
#include <iostream>
#include <chrono>
#include <Windows.h>

// Rotation and Movement Speed
float rotationSpeed = 1.f;
float movementSpeed = 5.f;



// Screen Dimensions
int _screenWidth = 120;
int _screenHeight = 40;

// Player Position/Rotation
float fPlayerX = 8.f;
float fPlayerY = 8.f;
float fPlayerA = 0.f;

// Map Dimensions
float _mapWidth = 16;
float _mapHeight = 16;

// FOV
float fFOV = 3.14159 / 4.0;
float drawLimit = 16.0;

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
	
	// DeltaTime timepoints
	auto currentFrameTime = std::chrono::system_clock::now();
	auto previousFrameTime = std::chrono::system_clock::now();

	while (true) // Game Loop
	{
		// Delta Time Calculation
		currentFrameTime = std::chrono::system_clock::now();
		std::chrono::duration<float> d_deltaTime = currentFrameTime - previousFrameTime;
		previousFrameTime = currentFrameTime;
		float fdeltaTime = d_deltaTime.count(); // Get the time inbetween in floating point

		// Player Controls
		// 
		// ROTATION
		if(GetAsyncKeyState((unsigned short)'Q') & 0x8000) // Rotation counter clockwise
		{
			fPlayerA -= (rotationSpeed * fdeltaTime);
		}
		else if(GetAsyncKeyState((unsigned short)'E') & 0x8000) // Rotation clockwise
		{
			fPlayerA += (rotationSpeed * fdeltaTime);
		}

		// MOVEMENT
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) // Forwards movement
		{
			// Calculating how much speed is applied to each axis
			fPlayerX += sinf(fPlayerA) * movementSpeed * fdeltaTime;
			fPlayerY += cosf(fPlayerA) * movementSpeed * fdeltaTime;

			// Collision deflection
			if (map[(int)fPlayerY * _mapWidth + (int)fPlayerX] == '#') 
			{
				fPlayerX -= sinf(fPlayerA) * movementSpeed * fdeltaTime;
				fPlayerY -= cosf(fPlayerA) * movementSpeed * fdeltaTime;
			}

		}
		else if (GetAsyncKeyState((unsigned short)'S') & 0x8000) // Backwards movement
		{
			// Calculating how much speed is applied to each axis
			fPlayerX -= sinf(fPlayerA) * movementSpeed * fdeltaTime;
			fPlayerY -= cosf(fPlayerA) * movementSpeed * fdeltaTime;

			if (map[(int)fPlayerY * _mapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX += sinf(fPlayerA) * movementSpeed * fdeltaTime;
				fPlayerY += cosf(fPlayerA) * movementSpeed * fdeltaTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) // Left Movement
		{
			fPlayerX -= cosf(fPlayerA) * movementSpeed * fdeltaTime;
			fPlayerY += sinf(fPlayerA) * movementSpeed * fdeltaTime;

			// Collision deflection
			if (map[(int)fPlayerY * _mapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX += cosf(fPlayerA) * movementSpeed * fdeltaTime;
				fPlayerY -= sinf(fPlayerA) * movementSpeed * fdeltaTime;
			}
		} 
		else if(GetAsyncKeyState((unsigned short)'D') & 0x8000)
		{
			fPlayerX += cosf(fPlayerA) * movementSpeed * fdeltaTime;
			fPlayerY -= sinf(fPlayerA) * movementSpeed * fdeltaTime;

			// Collision deflection
			if (map[(int)fPlayerY * _mapWidth + (int)fPlayerX] == '#')
			{
				fPlayerX -= cosf(fPlayerA) * movementSpeed * fdeltaTime;
				fPlayerY += sinf(fPlayerA) * movementSpeed * fdeltaTime;
			}
		}



		// Computation for each column of the screen along the X.
		for (int x = 0; x < _screenWidth; x++)
		{
			// Calculate the projected ray angle into world space.
			float fRayAngle = (fPlayerA - fFOV / 2.f) + ((float)x / (float)_screenWidth) * fFOV;

			// Distance from the wall.
			float distanceToWall = 0;
			bool hitWall = false;

			// Unit Vector for the ray from Player
			float eyeX = sinf(fRayAngle);
			float eyeY = cosf(fRayAngle);

			while (!hitWall && distanceToWall < drawLimit) 
			{
				distanceToWall += 0.01;

				// Create line from a given distance (distanceToWall) from the player's position and direction
				int testLineX = (int)(fPlayerX + eyeX * distanceToWall);
				int testLineY = (int)(fPlayerY + eyeY * distanceToWall);

				// Test for out of bounds
				if (testLineX < 0 || testLineX >= _mapWidth || testLineY < 0 || testLineY >= _mapHeight)
				{
					hitWall = true;
					distanceToWall = drawLimit;
				}
				else // Check individuals cells on the map
				{
					// Is it a wall?
					if (map[testLineY * _mapWidth + testLineX] == '#')
					{
						hitWall = true;
					}
				}
			}

			// Calculate the distance to ceiling and floor.
			float correctedDistanceToWall = distanceToWall * cosf(fPlayerA - fRayAngle); // Stops fisheye effect

			int ceiling = (float)(_screenHeight / 2.f) - _screenHeight / ((float)correctedDistanceToWall);
			int floor = _screenHeight - ceiling;

			short shade = ' ';

			// Sade Calculation (From OneLoneCoder.com)

			if (correctedDistanceToWall <= drawLimit / 4.0f) shade = 0x2588; // Directly in your face
			else if (correctedDistanceToWall <= drawLimit / 3.0f) shade = 0x2593;
			else if (correctedDistanceToWall <= drawLimit / 2.0f) shade = 0x2592;
			else if (correctedDistanceToWall <= drawLimit) shade = 0x2591;
			else shade = ' '; // Too far to render

			for (int y = 0; y < _screenHeight; y++)
			{
				// Must be apart of the ceiling, so shade it such
				if(y < ceiling)
				{
					screen[y * _screenWidth + x] = ' ';
				}
				else if(y > ceiling && y <= floor) // must be a wall.
				{
					screen[y * _screenWidth + x] = shade;
				}
				else // must be the floor
				{
					short floorShade = ' ';
					float floorShadeDist = 1.f - (((float)y - _screenHeight / 2.f) / ((float)_screenHeight / 2.f));
					if (floorShadeDist < .25f) floorShade = '#'; // Close up
					else if (floorShadeDist < .5f) floorShade = 'x';
					else if (floorShadeDist < .75f) floorShade = '.';
					else if (floorShadeDist < .9f) floorShade = '-';
					else floorShade = ' '; // Too far to see

					screen[y * _screenWidth + x] = floorShade;
				}
			}

		}
		
		// Write to the Screen
		screen[_screenWidth * _screenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, _screenWidth * _screenHeight, { 0, 0 }, &dwBytesWritten);
	}
	
	return 0;
}