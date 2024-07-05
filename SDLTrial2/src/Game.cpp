#include <iostream>

#include "Game.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int THICKNESS = 20;
const float PADDLE_SPEED = 300.0f;
const int PADDLE_WIDTH = THICKNESS;
const int PADDLE_HEIGHT = 3 * THICKNESS;

Game::Game() : m_Window(nullptr), m_IsRunning(true), 
	m_TopWall { 0, 0, SCREEN_WIDTH, THICKNESS }, 
	m_BottomWall { 0, SCREEN_HEIGHT - THICKNESS, SCREEN_WIDTH, THICKNESS },
	m_RightWall { SCREEN_WIDTH - THICKNESS, 0, THICKNESS, SCREEN_HEIGHT },
	m_LeftPaddlePosition{ 20, SCREEN_HEIGHT / 2 },
	m_RightPaddlePosition{ SCREEN_WIDTH - 20, SCREEN_HEIGHT / 2 },
	m_BallPosition{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
	currentTime(0),
	prevTime(0),
	deltaTime(0.0f), 
	m_BallSpeed{ -200.0f, 235.0f } {}

bool Game::Initialize() {
	int sdlres = SDL_Init(SDL_INIT_VIDEO);
	if (sdlres) {
		std::cout << "Error initializing SDL: " << SDL_GetError() << "\n";
		return false;
	}

	m_Window = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!m_Window) {
		std::cout << "Error creating window: " << SDL_GetError() << "\n";
		return false;
	}

	m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	return true;
}

void Game::RunLoop() {
	while (m_IsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown() {
	SDL_DestroyWindow(m_Window);
	SDL_DestroyRenderer(m_Renderer);
	SDL_Quit();
}

void Game::ProcessInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			m_IsRunning = false;
		}		
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_ESCAPE]) {
		m_IsRunning = false;
	}

	else {
		m_LeftPaddleDirection = 0;

		if (keys[SDL_SCANCODE_W]) {
			m_LeftPaddleDirection -= 1;
		}

		if (keys[SDL_SCANCODE_S]) {
			m_LeftPaddleDirection += 1;
		}

		m_RightPaddleDirection = 0;

		if (keys[SDL_SCANCODE_UP]) {
			m_RightPaddleDirection -= 1;
		}

		if (keys[SDL_SCANCODE_DOWN]) {
			m_RightPaddleDirection += 1;
		}
	}
}

void Game::UpdateGame() {
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), prevTime + 16));

	float deltaTime = (SDL_GetTicks() - prevTime) / 1000.0f;

	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}

	prevTime = SDL_GetTicks();

	// MOVEMENT OF LEFT PADDLE
	if (m_LeftPaddleDirection) {
		m_LeftPaddlePosition.y += m_LeftPaddleDirection * PADDLE_SPEED * deltaTime;

		if (m_LeftPaddlePosition.y < (PADDLE_HEIGHT / 2.0f + THICKNESS)) {
			m_LeftPaddlePosition.y = PADDLE_HEIGHT / 2.0f + THICKNESS;
		}

		else if (m_LeftPaddlePosition.y > (SCREEN_HEIGHT - PADDLE_HEIGHT / 2.0f - THICKNESS)) {
			m_LeftPaddlePosition.y = SCREEN_HEIGHT - PADDLE_HEIGHT / 2.0f - THICKNESS;
		}
	}

	// MOVEMENT OF RIGHT PADDLE
	if (m_RightPaddleDirection) {
		m_RightPaddlePosition.y += m_RightPaddleDirection * PADDLE_SPEED * deltaTime;

		if (m_RightPaddlePosition.y < (PADDLE_HEIGHT / 2.0f + THICKNESS)) {
			m_RightPaddlePosition.y = PADDLE_HEIGHT / 2.0f + THICKNESS;
		}

		else if (m_RightPaddlePosition.y > (SCREEN_HEIGHT - PADDLE_HEIGHT / 2.0f - THICKNESS)) {
			m_RightPaddlePosition.y = SCREEN_HEIGHT - PADDLE_HEIGHT / 2.0f - THICKNESS;
		}
	}
	
	// CONTINUOUS MOVEMENT OF BALL
	m_BallPosition.x += m_BallSpeed.x * deltaTime;
	m_BallPosition.y += m_BallSpeed.y * deltaTime;
	
	// DETECT COLLISION WITH LEFT PADDLE 
	float diffY = m_LeftPaddlePosition.y - m_BallPosition.y;
	diffY = (diffY > 0 ? diffY : -diffY);
	float diffX = m_LeftPaddlePosition.x - m_BallPosition.x;
	diffX = (diffX > 0 ? diffX : -diffX);
	if ((diffY <= PADDLE_HEIGHT / 2 + THICKNESS / 2) && (diffX <= THICKNESS) && (m_BallSpeed.x < 0.0f)) {
		m_BallSpeed.x = -m_BallSpeed.x;
	}

	// DETECT COLLISION WITH RIGHT PADDLE
	diffY = m_RightPaddlePosition.y - m_BallPosition.y;
	diffY = (diffY > 0 ? diffY : -diffY);
	diffX = m_RightPaddlePosition.x - m_BallPosition.x;
	diffX = (diffX > 0 ? diffX : -diffX);
	if ((diffY <= PADDLE_HEIGHT / 2 + THICKNESS / 2) && (diffX <= THICKNESS) && (m_BallSpeed.x > 0.0f)) {
		m_BallSpeed.x = -m_BallSpeed.x;
	}

	// DETECT COLLISION WITH TOP AND BOTTOM WALLS
	else if ((m_BallPosition.y <= THICKNESS) || (m_BallPosition.y >= SCREEN_HEIGHT - THICKNESS)) {
		m_BallSpeed.y = -m_BallSpeed.y;
	}

	// DETECT COLLISION WITH RIGHT WALL
	//else if ((m_BallPosition.x >= SCREEN_WIDTH - THICKNESS) && (m_BallSpeed.x > 0.0f)) {
		//m_BallSpeed.x = -m_BallSpeed.x;
	//}
	
}

void Game::GenerateOutput() {
	SDL_SetRenderDrawColor(m_Renderer, 0, 25, 255, 255);
	SDL_RenderClear(m_Renderer);

	// RENDER AND COLOR THE WALLS
	SDL_SetRenderDrawColor(m_Renderer, 214, 214, 214, 255);
	SDL_RenderFillRect(m_Renderer, &m_TopWall);
	SDL_RenderFillRect(m_Renderer, &m_BottomWall);
	//SDL_RenderFillRect(m_Renderer, &m_RightWall);

	// DRAW RECTS FOR THE PADDLES AND BALL AT THE RIGHT POSITIONS WITH THEIR RIGHT SIZES
	SDL_Rect leftPaddle{ static_cast<int>(m_LeftPaddlePosition.x - THICKNESS / 2), static_cast<int>(m_LeftPaddlePosition.y - (3 * THICKNESS / 2)), THICKNESS, PADDLE_HEIGHT };
	SDL_Rect rightPaddle{ static_cast<int>(SCREEN_WIDTH - (3 * THICKNESS / 2)), static_cast<int>(m_RightPaddlePosition.y - (3 * THICKNESS / 2)), THICKNESS, PADDLE_HEIGHT };

	SDL_Rect ball{ static_cast<int>(m_BallPosition.x - THICKNESS / 2), static_cast<int>(m_BallPosition.y - THICKNESS / 2), THICKNESS, THICKNESS };

	// FILL THE PADDLES AND BALL WITH COLOR
	SDL_RenderFillRect(m_Renderer, &leftPaddle);
	SDL_RenderFillRect(m_Renderer, &rightPaddle);

	SDL_RenderFillRect(m_Renderer, &ball);

	// DISPLAY
	SDL_RenderPresent(m_Renderer);
}