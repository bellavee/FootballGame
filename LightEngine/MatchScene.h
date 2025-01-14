#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Ball.h"
#include "MatchManager.h"
#include "Player.h"
#include "Scene.h"

class MatchScene : public Scene
{
public:
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;

private:
	void CreateFieldLines();
	void CreatePlayers();
	void DrawGoalLines();

	void DrawZones();
	void HighlightZone(float top, float bottom);
	void DrawZoneNumber(int number, float y);
	void DebugBall();

	float GetZoneHeight() const { return GetWindowHeight() / 3.0f; }

private:
	std::vector<Player*> mTeamGreen;
	std::vector<Player*> mTeamRed;
	Ball* mBall;
	float mBallDirection;

	sf::RectangleShape mLeftGoalLine;
	sf::RectangleShape mRightGoalLine;
	MatchManager* mMatchManager;
};

