#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Ball.h"
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
	void DrawField();

private:
	std::vector<Player*> mTeamGreen;
	std::vector<Player*> mTeamRed;
	Ball* mBall;

	sf::RectangleShape mLeftGoalLine;
	sf::RectangleShape mRightGoalLine;
};

