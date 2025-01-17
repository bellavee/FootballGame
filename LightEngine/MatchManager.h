#pragma once
#include "Ball.h"
#include "Player.h"

class MatchManager
{
public:
	MatchManager(std::vector<Player*>* greenTeam, std::vector<Player*>* redTeam, Scene* scene, Ball* ball);
	void Update();
	void CheckScoring();
	void ResetAfterScoring(int teamToReceiveBall);
	void ResetPositions();

	int GetGreenScore() const { return mGreenScore; }
	int GetRedScore() const { return mRedScore; }

private:
	Scene* mScene;
	std::vector<Player*>* mGreenTeam;
	std::vector<Player*>* mRedTeam;
	Ball* mBall;

	int mGreenScore;
	int mRedScore;
	bool mIsPlaying;

	float mScoringCooldown = 0.0f;

};

