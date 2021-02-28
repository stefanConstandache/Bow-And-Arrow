#pragma once

#include <Component/SimpleScene.h>
#include <TextRenderer/TextRenderer.h>
#include <string>
#include <Core/Engine.h>

class Tema1 : public SimpleScene
{
public:
	Tema1();
	~Tema1();
	int lifeCount = 3;
	int waveCount = 0;
	int stageCount = 1;
	int score = 0;
	int miliseconds = 0;
	int releaseTimeGlobalBalloons = 0;
	int releaseTimeGlobalShurikens = 0;
	int initialSpeed = 100;
	int balloonVectorSize, balloonVectorMaxSize = 5;
	int shurikenVectorSize, shurikenVectorMaxSize = 5, shurikenVectorAddSize = 5;
	int shurikenXSpeedAdd = -10;
	int balloonUpSpeedAdd = 0;
	float textHightNewGame, textWidthNewGame, textWidthExitGame, textHightExitGame, textWidthContinue, textHightContinue;
	float temp = 0;
	float shurikenScaleAdd = 0;
	float barScaleX = 0, barScaleY = 2;
	float powerArrow;
	float rotateBow;
	float rotateArrow;
	float initialAngle;
	float arrowXfinal, arrowYfinal;
	float Vx, Vy;
	float backgoundColorR = 0.5, backgoundColorB = 0.75, backgoundColorG = 0;
	float youDiedColor = 0;
	glm::vec3 newGamesNewGameColor = glm::vec3(0, 0, 0);
	glm::vec3 newGamesExitGameColor = glm::vec3(0.75, 0, 0);
	glm::vec3 exitGamesNewGameColor = glm::vec3(0.75, 0, 0);
	glm::vec3 exitGamesExitGameColor = glm::vec3(0.75, 0, 0);
	glm::vec3 newGamesContinueColor = glm::vec3(0, 0, 0);
	bool releaseArrow = false;
	bool colorRed = true;
	bool brokenArrow = false;
	bool powerBarAppearance = false;
	bool gameStart = false;
	bool newGameScreen = true;
	bool gameOverScreen = false;
	bool pauseGameScreen = false;
	bool mouseClickNewGame = false;
	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;
	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	void drawHud();
	TextRenderer* Text;

protected:
	glm::mat3 modelMatrix;
	glm::vec3 balloonOrigin = glm::vec3(0, 0, 0);
	float g = 9.8;
	float arrowLength = 100, arrowHigh = 2, arrowTipWidth = 10;
	float bowX = 100, bowY = 360;
	float bowRadius = 70;
	float balloonRadiusX = 35, balloonRadiusY = 50;
	float shurikenWidth = 10;
	float arrowX, arrowY;
	float mouseXglobal, mouseYglobal;
	float scaleX, scaleY;
	float angularStep;
};
#pragma once
