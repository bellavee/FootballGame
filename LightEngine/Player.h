#pragma once
#include "Entity.h"

class Ball;

class Player : public Entity
{
public:
	void OnInitialize() override;
	void OnUpdate() override;
	void OnCollision(Entity* collidedWith) override;

	int GetTeam() const { return mTeamSide; }
	void SetTeam(int team) { mTeamSide = team; }
	bool HasBall() const { return mHasBall; }
	void SetZoneBounds(float minY, float maxY) {
		mZoneMinY = minY;
		mZoneMaxY = maxY;
	}

	void HoldBall(Ball* ball);
	void LoseBall(Ball* ball);

private:
	int mTeamSide;
	bool mHasBall;
	float mZoneMinY;
	float mZoneMaxY;
	bool mMovingUp;

};

