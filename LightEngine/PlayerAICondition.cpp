#include "PlayerAICondition.h"

bool PlayerCondition_HavingBall::OnTest(Player* owner)
{
	return owner->HasBall();
}

bool PlayerCondition_IsNotInvincible::OnTest(Player* owner)
{
	return !owner->IsInvincible();
}

bool PlayerCondition_TeamMateHavingBall::OnTest(Player* owner)
{
	return (!owner->HasBall() && (owner->GetTeam() == owner->GetPlayerWithBallTeam()) && owner->GetPlayerWithBallTeam() >= 0);
}

bool PlayerCondition_OpponentHavingBall::OnTest(Player* owner)
{
	return (!owner->HasBall() && (owner->GetTeam() != owner->GetPlayerWithBallTeam()) && owner->GetPlayerWithBallTeam() >= 0);
}



bool PlayerCondition_CanPassBall::OnTest(Player* owner)
{
	return owner->CanMakePass();
}
