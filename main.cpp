/* This is a Pong game that's impossible to beat, mathematically speaking. */

#include <stdlib.h>
#include <iostream>
#include <string>
#include "raylib.h"
#include <math.h>

using namespace std;

Color lightBlue = Color{ 0, 254, 189, 255 };
int playerScore = 0;
int cpuScore = 0;
int ballOffScreen = 1;
int framesCounter = 0;

class Debug {
public:
	bool debugMode = true;

	void DebugState()
	{
		if (!debugMode){
			if (IsKeyPressed(KEY_R)) debugMode = true;
		}
		else
		{
			if (IsKeyPressed(KEY_R)) debugMode = false;
		}
	}
	void DrawFps(bool debugMode) {
		if (debugMode) {
			DrawFPS(10, GetScreenHeight() - 20); // Draw FPS
		}
	}
};

class Ball {
public:
	Vector2 position;
	float speed_x, speed_y;
	int radius;

	void Draw() 
	{
		// Draw ball
		DrawCircleV(position, radius, YELLOW);
	}

	void Update(float delta, bool horizontally, bool vertically, int screenWidth, int screenHeight)
	{
		// Move ball
		position.x += speed_x * delta;
		position.y += speed_y * delta;
		// Ball bounces off x wall
		if (horizontally)
			if (position.x + radius >= screenWidth || position.x - radius <= 0) speed_x *= -1;
		// Ball bounces off y wall
		if (vertically)
			if (position.y + radius >= screenHeight || position.y - radius <= 0) speed_y *= -1;
		// Reset game
		if (framesCounter == 120)
		{
			ballOffScreen = 1;
			framesCounter = 0;
			position.x = screenWidth / 2.0f;
			position.y = screenHeight / 2.0f;
			int speedChoices[2] = { -1, 1 };
			speed_x *= speedChoices[GetRandomValue(0, 1)];
			speed_y *= speedChoices[GetRandomValue(0, 1)];
		}

	}

	void BounceOffWall(bool horizontally, bool vertically) {
		// Ball bounces off x wall
		if (horizontally)
			if (position.x + radius >= GetScreenWidth() || position.x - radius <= 0) speed_x *= -1;

		// Ball bounces off y wall
		if (vertically)
			if (position.y + radius >= GetScreenHeight() || position.y - radius <= 0) speed_y *= -1;
	}
	void MoveBall() {
		// Inputs to move ball
		// This has nothing to do with the game, I just felt like making it
		if (IsKeyDown(KEY_RIGHT)) position.x += speed_x;
		if (IsKeyDown(KEY_LEFT)) position.x -= speed_x;
		if (IsKeyDown(KEY_UP)) position.y -= speed_y;
		if (IsKeyDown(KEY_DOWN)) position.y += speed_y;
	}
	void ScreenWrapping() {
		// Screen wrapping
		// This has nothing to do with the game, I just felt like making it
		if (position.x > (GetScreenWidth() + radius)) position.x -= (GetScreenWidth() + radius);
		if (position.x < (-radius)) position.x += (GetScreenWidth() + radius);
		if (position.y > (GetScreenHeight() + radius)) position.y -= (GetScreenHeight() + radius);
		if (position.y < (-radius)) position.y += (GetScreenHeight() + radius);
	}
};

class Paddle {
protected:
	void LimitMovement() {
		if (position.y <= 0) position.y = 0;
		if (position.y + scale.y >= GetScreenHeight()) position.y = GetScreenHeight() - scale.y;
	};
	public:
	Vector2 position;
	Vector2 scale;
	float speed; 
	int player1;

	void Draw() {
		// Draw Paddle
		DrawRectangleRoundedLines(Rectangle{ position.x, position.y, scale.x, scale.y }, 
			0.8f, 0, 4, BLACK);
		DrawRectangleRounded(Rectangle{ position.x, position.y, scale.x, scale.y }, 0.8f, 0, WHITE);
	}

	void Update(float delta, int player) {
		/* Move Paddle // player = true for left Paddle and player = false for right Paddle
		// Left Paddle Movement */
		if (IsKeyDown(KEY_W) && player == 1) {
			if (position.y >= 0) {
				position.y -= speed * delta;}
			else {
				position.y = 0;
			}
		}
		if (IsKeyDown(KEY_S) && player == 1) {
			if ((position.y + scale.y) <= GetScreenHeight()) {
				position.y += speed * delta;}
			else {
				position.y = GetScreenHeight() - scale.y;
			}
		}
		// Right Paddle Movement
		if (IsKeyDown(KEY_UP) && player == 2) {
			if (position.y >= 0) {
				position.y -= speed * delta;}
			else {
				position.y = 0;
			}
		}
		if (IsKeyDown(KEY_DOWN) && player == 2) {
			if ((position.y + scale.y) <= GetScreenHeight()) {
				position.y += speed * delta;}
			else {
				position.y = GetScreenHeight() - scale.y;
			}
			LimitMovement();
		}
	}
};

class CpuPaddle : public Paddle 
{
public:
	void Update(float ball_y, float delta) {
		if (position.y + scale.y / 2 > ball_y) position.y = position.y - speed * delta;
		if (position.y + scale.y / 2 <= ball_y) position.y = position.y + speed * delta;
		LimitMovement();
	}
};

Debug debug;
Ball ball;
Paddle leftPaddle;
Paddle rightPaddle;
CpuPaddle cpu;

int main(void)
{
	// Window Variables
	const int screenWidth = 1200;
	const int screenHeight = 720;
	// Left Paddle Variables
	// Unused unless disable the cpu
	leftPaddle.scale.x = 15;
	leftPaddle.scale.y = 100;
	leftPaddle.position.x = 10;
	leftPaddle.position.y = screenHeight / 2 - leftPaddle.scale.y / 2;
	leftPaddle.speed = 600;
	// Right Paddle Variables
	rightPaddle.scale.x = 15;
	rightPaddle.scale.y = 100;
	rightPaddle.position.x = screenWidth - (10 + rightPaddle.scale.x);
	rightPaddle.position.y = screenHeight / 2 - rightPaddle.scale.y / 2;
	rightPaddle.speed = 600;
	// CPU Paddle Variables
	cpu.scale.x = 15;
	cpu.scale.y = 100;
	cpu.position.x = 10;
	cpu.position.y = screenHeight / 2 - cpu.scale.y / 2;
	cpu.speed = 600;
	// Ball Variables
	ball.radius = 15;
	ball.position.x = screenWidth / 2.0f;
	ball.position.y = screenHeight / 2.0f;
	ball.speed_x = 480.0f;
	ball.speed_y = 480.0f;

	InitWindow(screenWidth, screenHeight, "Pong"); 		// Initialize Window
	SetTargetFPS(60);	// Set game to run at 60 FPS
	
	InitAudioDevice();// Initialize Audio Device
	// Load sounds and music
	Sound ball_hit = LoadSound("audio/resources/ball_hit.wav");
	Sound score_up = LoadSound("audio/resources/score_up.wav");
	Music run_amok = LoadMusicStream("audio/resources/run_amok.mp3");
	PlayMusicStream(run_amok); // Play music on Startup


	while (!WindowShouldClose())
	{
		/* ---------------------------------------------------------------------------------*/
		/* ----------------------------------- Begin Update  -------------------------------*/
		/* ---------------------------------------------------------------------------------*/
		float delta = GetFrameTime();		// Get delta/Frame Time

		// Audio
		SetSoundVolume(ball_hit, 0.80f);
		SetSoundVolume(score_up, 0.80f);
		UpdateMusicStream(run_amok);

		// Update Entities
		rightPaddle.Update(delta, 2);		// Update Right Paddle
		cpu.Update(ball.position.y, delta);		// Update CPU Paddle
		ball.Update(delta, false, true, screenWidth, screenHeight);// Update Ball

		// Update Score
		if (ballOffScreen == 1)
		{
			if (ball.position.x + ball.radius >= GetScreenWidth()) {
				cpuScore++;
				PlaySound(score_up);
				ballOffScreen = 2;
			}
			if (ball.position.x - ball.radius <= 0) {
				playerScore++;
				PlaySound(score_up);
				ballOffScreen = 2;
			}
		}
		// Increases frames counter
		if (ballOffScreen == 2) framesCounter++;

		// Check for Collisions
		if (CheckCollisionCircleRec(ball.position, ball.radius,
			Rectangle{ rightPaddle.position.x, rightPaddle.position.y,
			rightPaddle.scale.x,rightPaddle.scale.y })) {
			ball.speed_x *= -1;
			PlaySound(ball_hit);
		}
		if (CheckCollisionCircleRec(ball.position, ball.radius,
			Rectangle{ cpu.position.x, cpu.position.y, cpu.scale.x, cpu.scale.y })) {
			ball.speed_x *= -1;
			PlaySound(ball_hit);
		}
		// Reset the scores
		if (IsKeyPressed(KEY_R)) {
			cpuScore = 0;
			playerScore = 0;
		}

		/* ----------------------------------------------------------------------------------*/
		/* ----------------------------------- Begin Drawing  -------------------------------*/
		/* ----------------------------------------------------------------------------------*/
		BeginDrawing();
		// Backgrounds
		DrawRectangle(0, 0, screenWidth / 2, screenHeight, SKYBLUE);
		DrawRectangle(screenWidth / 2, 0, screenWidth / 2, screenHeight, PURPLE);
		DrawEllipse(screenWidth / 2, screenHeight / 2, 80.0f, 40.0f, VIOLET);
		DrawEllipse(screenWidth / 2, screenHeight / 2, 40.0f, 80.0f, BLUE);
		DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RAYWHITE);
		DrawCircle(screenWidth / 2, screenHeight / 2, 50, BLACK);
		DrawCircle(screenWidth / 2, screenHeight / 2, 45, lightBlue);
		
		// Game Objects
		ball.Draw();		// Draw ball
		cpu.Draw();		// Draw cpu/left paddle
		rightPaddle.Draw();		// Draw player/right paddle

		// UI
		DrawText(TextFormat("%i", cpuScore), screenWidth * 0.25f - 2, screenHeight * 0.05 - 3, 90, BLACK);
		DrawText(TextFormat("%i", cpuScore), screenWidth * 0.25f, screenHeight * 0.05, 80, WHITE);
		DrawText(TextFormat("%i", playerScore), screenWidth * 0.75f - 2, screenHeight * 0.05 - 3, 90, BLACK);
		DrawText(TextFormat("%i", playerScore), screenWidth * 0.75f, screenHeight * 0.05, 80, WHITE);

		// Debug UI
		debug.DrawFps(debug.debugMode);
		EndDrawing();
	}
	// Unload files, close devices and the window
	UnloadMusicStream(run_amok);
	UnloadSound(score_up);
	UnloadSound(ball_hit);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}


