#pragma once
#include <SFML/Graphics/RectangleShape.hpp>

#include "Ball.h"
#include "MatchManager.h"
#include "Player.h"
#include "Scene.h"

class MatchScene : public Scene
{
public:
	enum Tag {
		PLAYER,
		BALL,
	};
	void OnInitialize() override;
	void OnEvent(const sf::Event& event) override;
	void OnUpdate() override;

	const std::vector<Player*>& GetGreenTeam() const { return mGreenTeam; }
	const std::vector<Player*>& GetRedTeam() const { return mRedTeam; }
	Ball* GetBall() const { return mBall; }

private:
	void CreatePlayers();
	void DrawGoalLines();

	void DrawZones();
	void HighlightZone(float top, float bottom);
	void DrawZoneNumber(int number, float y);

private:
	std::vector<Player*> mGreenTeam;
	std::vector<Player*> mRedTeam;
	Ball* mBall;

	sf::RectangleShape mLeftGoalLine;
	sf::RectangleShape mRightGoalLine;
	MatchManager* mMatchManager = nullptr;

	bool mDebugMode = false;
	bool mShowPassTrajectory = false;
};

