#include "PlayerAICondition.h"

bool PlayerCondition_HavingBall::OnTest(Player* owner)
{
	return owner->HasBall();
}

bool PlayerCondition_TeamMateHavingBall::OnTest(Player* owner)
{
	return (!owner->HasBall() && (owner->GetTeam() == owner->GetPlayerWithBallTeam()));
}

bool PlayerCondition_OpponentHavingBall::OnTest(Player* owner)
{
	return (!owner->HasBall() && (owner->GetTeam() != owner->GetPlayerWithBallTeam()));
}

bool PlayerCondition_JustHadBall::OnTest(Player* owner)
{
	return !owner->IsInvincible();
}

bool PlayerCondition_CanMakePass::OnTest(Player* owner)
{
	return owner->CanMakePass();
}
