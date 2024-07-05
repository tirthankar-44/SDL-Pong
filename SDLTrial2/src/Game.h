#pragma once

#include <SDL.h>

struct Vector2 {
	float x;
	float y;
};

class Game {
private:
	SDL_Window* m_Window;
	bool m_IsRunning;
	SDL_Renderer* m_Renderer;

	SDL_Rect m_TopWall, m_BottomWall, m_RightWall;
	Vector2 m_LeftPaddlePosition, m_RightPaddlePosition, m_BallPosition;
	int currentTime, prevTime, deltaTime;

	int m_LeftPaddleDirection, m_RightPaddleDirection;

	Vector2 m_BallSpeed;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();	

public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();
};

