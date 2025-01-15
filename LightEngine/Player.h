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

	bool CanMakePass() const { return mPassCooldownTimer <= 0.0f; }
	bool IsInvincible() const { return mInvincibilityTimer > 0.0f; }

	void HoldBall(Ball* ball);
	void LoseBall(Ball* ball);
	void GiveBall(Player* player, Ball* ball);

	void DrawInterceptionLines();
	void DrawPassingTrajectory(const sf::Vector2f& target);

	void ResetStates();

	Player* FindBestPassTarget();

private:
	bool IsOpponentBlockingPass(const sf::Vector2f& from, const sf::Vector2f& to, const sf::Vector2f& oppPos);

	void HandleBallCarrierBehavior(const std::vector<Player*>& opposingTeam);
	void HandleSupportingBehavior(Player* ballCarrier);
	void HandleDefensiveBehavior(Player* ballCarrier);
	void HandleFreeBallBehavior(Ball* ball);

private:
	int mTeamSide;
	bool mHasBall;
	float mZoneMinY;
	float mZoneMaxY;
	bool mMovingUp;

	float mPossessionTimer = 0.0f;
	float mInvincibilityTimer = 0.0f;
	float mPassCooldownTimer = 0.0f;
	float mSpeedBoostTimer = 0.0f;
	float mBaseSpeed = 150.0f;
	float mBoostSpeed = 250.0f;

	Ball* mCurrentHolder = nullptr;
};


