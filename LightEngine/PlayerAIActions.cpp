#include "PlayerAIActions.h"
#include "Ball.h"
#include <iostream>

void PlayerAction_Idle::OnStart(Player* player)
{
	player->SetIsResetLaunched(false);
}

void PlayerAction_Idle::OnCollision(Player* player, Entity* collidedWith)
{
	/*if (Ball* ball = static_cast<Ball*>(collidedWith)) {
		if (ball->GetCurrentHolder() == nullptr) {
			player->GiveBall();
			//->GetStateMachine()->SetState(Player::PlayerState::JustGotTheBall);
		}
	}*/
}

void PlayerAction_JustGotTheBall::OnStart(Player* player)
{
	player->HoldBall();
}

void PlayerAction_JustGotTheBall::OnUpdate(Player* player)
{

	float dt = player->GetDeltaTime(), mInvincibilityTimer = player->GetInvincibilityTimer(), mPassCooldownTimer = player->GetPassCooldownTimer(), mSpeedBoostTimer = player->GetSpeedBoostTimer();

	if (mInvincibilityTimer > 0) {
		mInvincibilityTimer -= dt;
		player->SetInvincibilityTimer(mInvincibilityTimer);
	}

	if (mPassCooldownTimer > 0) {
		mPassCooldownTimer -= dt;
		player->SetPassCooldownTimer(mPassCooldownTimer);
	}

	if (mSpeedBoostTimer > 0) {
		mSpeedBoostTimer -= dt;
		player->SetSpeedBoostTimer(mSpeedBoostTimer);
		player->SetSpeed(Constant::PLAYER_SPEED * 1.5f);
	}
	else {
		player->SetSpeed(Constant::PLAYER_SPEED);
	}
	Player* passTarget = nullptr;
	player->HandleHavingBall(&passTarget);
}

void PlayerAction_JustGotTheBall::OnEnd(Player* player)
{
	player->SetInvincibilityTimer(0);
	player->SetPassCooldownTimer(0);
}

void PlayerAction_HavingTheBall::OnUpdate(Player* player)
{
	Player* passTarget = nullptr;
	player->HandleHavingBall(&passTarget);
	if (!passTarget)
		return;
	passTarget->GiveBall();
}

void PlayerAction_TeamMateHavingTheBall::OnUpdate(Player* player)
{

	if (player->GetTeam() == player->GetPlayerWithBallTeam())
		player->HandleSupportingBehavior();
}

void PlayerAction_OpponentHavingTheBall::OnUpdate(Player* player)
{
	if (player->GetTeam() != player->GetPlayerWithBallTeam())
		player->HandleDefensiveBehavior();
}
