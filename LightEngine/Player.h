#pragma once
#include "Entity.h"
#include "StateMachine.h"

class Ball;

class Player : public Entity
{
public:
	Player();
	void OnInitialize() override;
	void OnUpdate() override;
	void OnCollision(Entity* collidedWith) override;

	int GetTeam() const { return mTeamSide; }
	void SetTeam(int team) { mTeamSide = team; }
	bool HasBall() const { return mHasBall; }
	void GiveBall() { mHasBall = true; }
	void RemoveBall() { mHasBall = false; }
	int GetPlayerWithBallTeam();
	void SetZoneBounds(float minY, float maxY) {
		mZoneMinY = minY;
		mZoneMaxY = maxY;
	}
	float GetInvincibilityTimer() { return mInvincibilityTimer; }
	void SetInvincibilityTimer(float timer) { mInvincibilityTimer = timer; }
	float GetPassCooldownTimer() { return mPassCooldownTimer;  }
	void SetPassCooldownTimer(float timer) { mPassCooldownTimer = timer;  }
	float GetSpeedBoostTimer() { return mSpeedBoostTimer;  }
	void SetSpeedBoostTimer(float timer) { mSpeedBoostTimer = timer; }

	StateMachine<Player>* GetStateMachine() { return m_stateMachine.get(); }

	bool CanMakePass() const { return mPassCooldownTimer <= 0.0f; }
	bool IsInvincible() const { return mInvincibilityTimer > 0.0f; }

	void HoldBall();
	void LoseBall(Ball* ball);
	void GiveBall(Player* player, Ball* ball);

	void DrawInterceptionLines();
	void DrawPassingTrajectory(const sf::Vector2f& target);

	void ResetStates();

	std::string GetPlayerState();


	Player* FindBestPassTarget();

	enum PlayerState
	{
		Idle,
		JustGotTheBall,
		HavingTheBall,
		TeamMateHavingTheBall,
		OpponentHavingTheBall,
		Count
	};
	void HandleHavingBall(Player* passTarget);
	void HandleSupportingBehavior();
	void HandleDefensiveBehavior();
private:
	bool IsOpponentBlockingPass(const sf::Vector2f& from, const sf::Vector2f& to, const sf::Vector2f& oppPos);

	void HandleBallCarrierBehavior(const std::vector<Player*>& opposingTeam, Player* passTarget);
	
	
	void HandleFreeBallBehavior();

	void InitializeStateMachine();

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

	Ball* mBall = nullptr;
	std::unique_ptr<StateMachine<Player>> m_stateMachine;
	
};


