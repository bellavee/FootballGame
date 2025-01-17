#include "Player.h"

#include "Ball.h"
#include "Constant.h"
#include "Debug.h"
#include "MatchScene.h"
#include "PlayerAIActions.h"
#include "PlayerAICondition.h"

Player::Player() : m_stateMachine(std::make_unique<StateMachine<Player>>(this, PlayerState::Count))
{
}

void Player::OnInitialize() {
	SetTag(MatchScene::Tag::PLAYER);
	InitializeStateMachine();
	mHasBall = false;
	SetDirection(0, 0);
	SetSpeed(150.0f);
	mBall = static_cast<MatchScene*>(GetScene())->GetBall();
}

void Player::OnUpdate() {
	float dt = GetDeltaTime();
	if (!mBall)
		mBall = static_cast<MatchScene*>(GetScene())->GetBall();
	m_stateMachine->Update();
	sf::Vector2f pos = GetPosition(); 
	if (pos.y < mZoneMinY) SetPosition(pos.x, mZoneMinY);
	if (pos.y > mZoneMaxY) SetPosition(pos.x, mZoneMaxY);
	Debug::DrawCircle(pos.x, pos.y, HasBall() ? Constant::PLAYER_RADIUS : 0, sf::Color(0, 0, 255, 128));
	Debug::DrawText(GetPosition(0, 0).x, GetPosition(0, 0).y - 50, GetPlayerState(), sf::Color::White);
}

void Player::OnCollision(Entity* collidedWith) {
	if (collidedWith->IsTag(MatchScene::Tag::BALL)) {
		Ball* ball = dynamic_cast<Ball*>(collidedWith);
		if (ball->GetCurrentHolder() == nullptr && !IsInvincible())
			HoldBall();
	}
	if (collidedWith->IsTag(MatchScene::Tag::PLAYER)) {
		mTarget.isSet = false;
		//sf::Vector2f pos = GetPosition();
		//GoToPosition(pos.x - Constant::DISTANCE_BETWEEN_PLAYERS, pos.y - Constant::DISTANCE_BETWEEN_PLAYERS);
	}
}

int Player::GetPlayerWithBallTeam()
{
	int res = -1;
	MatchScene* matchScene = static_cast<MatchScene*>(GetScene());
	if (matchScene->GetBall())
		res = matchScene->GetBall()->GetCurrentHolderTeam();
	return res;
}

void Player::HoldBall() {
	Ball* ball = static_cast<MatchScene*>(GetScene())->GetBall();
	if (Player* currentHolder = ball->GetCurrentHolder()) {
		currentHolder->LoseBall(ball);
	}

	//mHasBall = true;
	//mCurrentHolder = ball;
	ball->SetCurrentHolder(this);
	//ball->MoveTo(this); // new

	mInvincibilityTimer = Constant::INVINCIBILITY_DURATION;
	mPassCooldownTimer = Constant::PASS_COOLDOWN;
	mSpeedBoostTimer = Constant::SPEED_BOOST_DURATION;
}

void Player::LoseBall(Ball* ball) {
	//mHasBall = false;
	//mCurrentHolder = nullptr;
	RemoveBall();
	if (ball->GetCurrentHolder() == this) {
		ball->SetCurrentHolder(nullptr);
	}
}

/*
void Player::GiveBall(Player* player, Ball* ball) {
	if (!player->IsInvincible() && player->CanMakePass()) {
		LoseBall(ball);
		ball->MoveTo(player);
		ball->SetCurrentHolder(player);
		player->HoldBall();
	}
}*/

Player* Player::FindBestPassTarget() {
	if (!mHasBall) return nullptr;

	MatchScene* matchScene = dynamic_cast<MatchScene*>(GetScene());
	std::vector<Player*> myTeammates = mTeamSide == 0 ? matchScene->GetGreenTeam() : matchScene->GetRedTeam();
	std::vector<Player*> opponents = mTeamSide == 0 ? matchScene->GetRedTeam() : matchScene->GetGreenTeam();

	Player* bestTarget = nullptr;
	float bestScore = -1.0f;
	float MIN_DIST = 50.0f;
	float MAX_DIST = 300.0f;
	sf::Vector2f myPos = GetPosition();

	for (Player* teammate : myTeammates) {
		if (teammate == this) continue;

		bool isCollidingWithOpponent = false;
		for (Player* opponent : opponents) {
			if (teammate->IsColliding(opponent)) {
				isCollidingWithOpponent = true;
				break;
			}
		}
		if (isCollidingWithOpponent) continue;

		sf::Vector2f teammatePos = teammate->GetPosition();

		float score = 0.0f;

		float distance = std::hypot(teammatePos.x - myPos.x, teammatePos.y - myPos.y);
		if (distance < MIN_DIST || distance > MAX_DIST) continue;
		score += (1.0f - (distance / MAX_DIST)) * 0.4f;

		bool isBlocked = false;
		for (Player* opponent : opponents) {
			sf::Vector2f oppPos = opponent->GetPosition();
			if (IsOpponentBlockingPass(myPos, teammatePos, oppPos)) {
				isBlocked = true;
				break;
			}
		}
		if (isBlocked) continue;

		float progressScore = mTeamSide == 0 ?
			(teammatePos.x / matchScene->GetWindowWidth()) : 
			(1.0f - teammatePos.x / matchScene->GetWindowWidth());
		score += progressScore * 0.6f;

		if ((mTeamSide == 0 && teammatePos.x > myPos.x) ||
			(mTeamSide == 1 && teammatePos.x < myPos.x)) {
			score *= 1.5f;  
		}

		if (score > bestScore) {
			bestScore = score;
			bestTarget = teammate;
		}
	}

	return bestScore > 0.3f ? bestTarget : nullptr;
}

void Player::HandleHavingBall(Player** passTarget)
{
	MatchScene* matchScene = dynamic_cast<MatchScene*>(GetScene());
	std::vector<Player*> opposingTeam = mTeamSide == 0 ?
		matchScene->GetRedTeam() : matchScene->GetGreenTeam();
	HandleBallCarrierBehavior(opposingTeam, passTarget);
}

bool Player::IsOpponentBlockingPass(const sf::Vector2f& from,
	const sf::Vector2f& to,
	const sf::Vector2f& oppPos) {
	const float SAFETY_MARGIN = 30.0f;

	float lineLength = std::hypot(to.x - from.x, to.y - from.y);
	float crossProduct = std::abs((to.x - from.x) * (from.y - oppPos.y) - (from.x - oppPos.x) * (to.y - from.y));
	float perpDistance = crossProduct / lineLength;

	if (perpDistance > SAFETY_MARGIN) return false;

	float dotProduct = ((oppPos.x - from.x) * (to.x - from.x) + (oppPos.y - from.y) * (to.y - from.y)) / (lineLength * lineLength);

	return dotProduct >= 0 && dotProduct <= 1;
}

void Player::DrawInterceptionLines() {
	sf::Vector2f myPos = GetPosition();
	MatchScene* matchScene = dynamic_cast<MatchScene*>(GetScene());

	std::vector<Player*> opponents = mTeamSide == 0 ?
		matchScene->GetRedTeam() : matchScene->GetGreenTeam();

	for (Player* opponent : opponents) {
		sf::Vector2f oppPos = opponent->GetPosition();
		float distance = std::hypot(oppPos.x - myPos.x, oppPos.y - myPos.y);

		if (distance <= Constant::INTERCEPTION_RANGE) {
			Debug::DrawLine(
				myPos.x, myPos.y,
				oppPos.x, oppPos.y,
				sf::Color(255, 255, 0, 128) 
			);
		}
	}
}

void Player::DrawPassingTrajectory(const sf::Vector2f& target) {
	if (!mHasBall) return;
	
	sf::Vector2f start = GetPosition();
	sf::Vector2f direction = target - start;
	float length = std::hypot(direction.x, direction.y);

	if (length > 0) {
		direction.x /= length;
		direction.y /= length;

		sf::Vector2f end = start + direction * Constant::PASS_TRAJECTORY_LENGTH;
		Debug::DrawLine(
			start.x, start.y,
			end.x, end.y,
			sf::Color(0, 255, 255, 180)
		);

		float arrowSize = 10.0f;
		float angle = std::atan2(direction.y, direction.x);
		sf::Vector2f arrow1(
			end.x - arrowSize * std::cos(angle + 0.5f),
			end.y - arrowSize * std::sin(angle + 0.5f)
		);
		sf::Vector2f arrow2(
			end.x - arrowSize * std::cos(angle - 0.5f),
			end.y - arrowSize * std::sin(angle - 0.5f)
		);

		Debug::DrawLine(end.x, end.y, arrow1.x, arrow1.y, sf::Color(0, 255, 255, 180));
		Debug::DrawLine(end.x, end.y, arrow2.x, arrow2.y, sf::Color(0, 255, 255, 180));
	}
}

void Player::HandleBallCarrierBehavior(const std::vector<Player*>& opposingTeam, Player **passTarget) {
	sf::Vector2f currentPos = GetPosition();
	float goalX = mTeamSide == 0 ?
		GetScene()->GetWindowWidth() - Constant::GOAL_LINE_MARGIN :
		Constant::GOAL_LINE_MARGIN;

	Player* nearestOpponent = nullptr;
	float minDistance = FLT_MAX;

	for (Player* opponent : opposingTeam) {
		if (opponent->IsInvincible()) continue;

		float dist = std::hypot(
			opponent->GetPosition().x - currentPos.x,
			opponent->GetPosition().y - currentPos.y
		);

		if (dist < minDistance) {
			minDistance = dist;
			nearestOpponent = opponent;
		}
	}

	bool nearGoal = mTeamSide == 0 ?
		(currentPos.x > GetScene()->GetWindowWidth() - Constant::GOAL_LINE_MARGIN * 3) :
		(currentPos.x < Constant::GOAL_LINE_MARGIN * 3);

	if (!nearGoal && nearestOpponent && minDistance < Constant::INTERCEPTION_RANGE && CanMakePass()) {
		*passTarget = FindBestPassTarget();
		if (*passTarget && !IsInvincible() && CanMakePass()) {
			//DrawPassingTrajectory((*passTarget)->GetPosition());
			
			DrawPassingTrajectory((*passTarget)->GetPosition());
			//RemoveBall();
			//GiveBall(passTarget, mCurrentHolder);
			//LoseBall(mBall);
			return;
		}
	}

	float targetX;
	float targetY = currentPos.y;

	if (nearGoal) {
		targetX = goalX;
	}
	else {
		targetX = mTeamSide == 0 ?
			std::min(goalX, currentPos.x + Constant::PLAYER_RADIUS * 2) :
			std::max(goalX, currentPos.x - Constant::PLAYER_RADIUS * 2);

		if (nearestOpponent) {
			sf::Vector2f opponentPos = nearestOpponent->GetPosition();
			bool opponentInFront = (mTeamSide == 0) ?
				(opponentPos.x > currentPos.x) :
				(opponentPos.x < currentPos.x);

			if (minDistance < Constant::INTERCEPTION_RANGE * 1.5f && opponentInFront) {
				float yDiff = opponentPos.y - currentPos.y;
				if (abs(yDiff) < Constant::PLAYER_RADIUS * 2) {
					targetY = opponentPos.y > currentPos.y ?
						std::max(mZoneMinY, currentPos.y - Constant::PLAYER_RADIUS * 2) :
						std::min(mZoneMaxY, currentPos.y + Constant::PLAYER_RADIUS * 2);
				}
			}
		}
	}

	targetY = std::clamp(targetY, mZoneMinY, mZoneMaxY);

	GoToPosition(targetX, targetY);
}

void Player::HandleSupportingBehavior() 
{
	Player* ballCarrier = mBall->GetCurrentHolder();

	if (!ballCarrier) {
		HandleFreeBallBehavior();
		return;
	}

	sf::Vector2f currentPos = GetPosition();
	sf::Vector2f carrierPos = ballCarrier->GetPosition();
	MatchScene* matchScene = dynamic_cast<MatchScene*>(GetScene());


	std::vector<Player*> teammates = mTeamSide == 0 ?
		matchScene->GetGreenTeam() : matchScene->GetRedTeam();

	float baseX;
	if (mTeamSide == 0) {  
		baseX = std::min(
			carrierPos.x - Constant::PLAYER_RADIUS * 3,
			GetScene()->GetWindowWidth() - Constant::GOAL_LINE_MARGIN);
	}
	else {  
		baseX = std::max(
			carrierPos.x + Constant::PLAYER_RADIUS * 3,  
			Constant::GOAL_LINE_MARGIN  
		);
	}

	float minSpacing = Constant::PLAYER_RADIUS * 3;
	float targetY = carrierPos.y;
	bool isAbove = false;

	int teammatesAbove = 0;
	int teammatesBelow = 0;
	for (Player* teammate : teammates) {
		if (teammate != this && teammate != ballCarrier) {
			if (teammate->GetPosition().y < carrierPos.y) teammatesAbove++;
			else teammatesBelow++;
		}
	}

	if (teammatesAbove <= teammatesBelow) {
		targetY = carrierPos.y - minSpacing * (teammatesAbove + 1);
		isAbove = true;
	}
	else {
		targetY = carrierPos.y + minSpacing * teammatesBelow;
	}

	targetY = std::clamp(targetY, mZoneMinY, mZoneMaxY);

	GoToPosition(baseX, targetY);
}

void Player::HandleDefensiveBehavior() {
	Player* ballCarrier = mBall->GetCurrentHolder();
	if (!ballCarrier) {
		HandleFreeBallBehavior();
		return;
	}
	sf::Vector2f currentPos = GetPosition();
	sf::Vector2f ballPos = ballCarrier->GetPosition();

	float interceptX = ballPos.x;
	float interceptY = ballPos.y;

	if (mTeamSide == 0) {
		interceptX = std::max(ballPos.x - Constant::PLAYER_RADIUS * 3,
			Constant::GOAL_LINE_MARGIN);
		if (currentPos.x < ballPos.x) {
			interceptX = currentPos.x + Constant::PLAYER_RADIUS * 2;
		}
	}
	else { 
		interceptX = std::min(ballPos.x + Constant::PLAYER_RADIUS * 3,
			GetScene()->GetWindowWidth() - Constant::GOAL_LINE_MARGIN);
		if (currentPos.x > ballPos.x) {
			interceptX = currentPos.x - Constant::PLAYER_RADIUS * 2;
		}
	}

	float yDiff = ballPos.y - currentPos.y;
	if (abs(yDiff) > Constant::PLAYER_RADIUS * 2) {
		interceptY = currentPos.y + (yDiff > 0 ? 1 : -1) * Constant::PLAYER_RADIUS * 2;
	}

	interceptY = std::clamp(interceptY, mZoneMinY, mZoneMaxY);

	GoToPosition(interceptX, interceptY);
}

void Player::HandleFreeBallBehavior() {
	sf::Vector2f ballPos = mBall->GetPosition();

	if (ballPos.y >= mZoneMinY && ballPos.y <= mZoneMaxY) {
		GoToPosition(ballPos.x, ballPos.y);
	}
}

void Player::ResetStates() {
	if (mHasBall) {
		return;
	}
	mHasBall = false;
	mIsResetLaunched = true;
	mInvincibilityTimer = 0.0f;
	mPassCooldownTimer = 0.0f;
	mSpeedBoostTimer = 0.0f;
	SetSpeed(Constant::PLAYER_SPEED);
}

std::string Player::GetPlayerState()
{
	switch (m_stateMachine->GetCurrentState())
	{
	case PlayerState::Idle:
		return "Idle" + std::to_string(mTeamSide);
	case PlayerState::JustGotTheBall:
		return "JustGot\nBall" + std::to_string(mTeamSide);
	case PlayerState::HavingTheBall:
		return "Having\nBall" + std::to_string(mTeamSide);
	case PlayerState::TeamMateHavingTheBall: 
		return "TeamMate\nHadBall" + std::to_string(mTeamSide);
	case PlayerState::OpponentHavingTheBall:
		return "Opponent\nHadBall" + std::to_string(mTeamSide);
	default:
		return "Unknown";
	};
}

void Player::InitializeStateMachine()
{
	//tag
	/* IDLE */
	Action<Player>* idle = m_stateMachine->CreateAction<PlayerAction_Idle>(PlayerState::Idle);
	auto transitionIdleJustHadBall = idle->CreateTransition(PlayerState::JustGotTheBall);
	transitionIdleJustHadBall->AddCondition<PlayerCondition_HavingBall>();
	transitionIdleJustHadBall->AddCondition<PlayerCondition_IsNotInvincible>();
	auto transitionIdleTeamMateHadBall = idle->CreateTransition(PlayerState::TeamMateHavingTheBall);
	transitionIdleTeamMateHadBall->AddCondition<PlayerCondition_TeamMateHavingBall>();
	auto transitionIdleOpponentHadBall = idle->CreateTransition(PlayerState::OpponentHavingTheBall);
	transitionIdleOpponentHadBall->AddCondition<PlayerCondition_OpponentHavingBall>();

	/* JustGotTheBall */
	Action<Player>* justGotTheBall = m_stateMachine->CreateAction<PlayerAction_JustGotTheBall>(PlayerState::JustGotTheBall);
	auto transitionJustGotTheBallHavingBall = justGotTheBall->CreateTransition(PlayerState::HavingTheBall);
	transitionJustGotTheBallHavingBall->AddCondition<PlayerCondition_HavingBall>();
	transitionJustGotTheBallHavingBall->AddCondition<PlayerCondition_IsNotInvincible>();
	transitionJustGotTheBallHavingBall->AddCondition<PlayerCondition_CanPassBall>();
	transitionJustGotTheBallHavingBall->AddCondition<PlayerCondition_IsBooted>(false);


	/* HavingBall */
	Action<Player>* havingTheBall = m_stateMachine->CreateAction<PlayerAction_HavingTheBall>(PlayerState::HavingTheBall);
	auto transitionHavingBallTeamMateHavingBall = havingTheBall->CreateTransition(PlayerState::TeamMateHavingTheBall);
	transitionHavingBallTeamMateHavingBall->AddCondition<PlayerCondition_HavingBall>(false);
	transitionHavingBallTeamMateHavingBall->AddCondition<PlayerCondition_TeamMateHavingBall>();
	transitionHavingBallTeamMateHavingBall->AddCondition<PlayerCondition_CanPassBall>();
	auto transitionHavingBallOpponentHavingBall = havingTheBall->CreateTransition(PlayerState::OpponentHavingTheBall);
	transitionHavingBallOpponentHavingBall->AddCondition<PlayerCondition_HavingBall>(false);
	transitionHavingBallOpponentHavingBall->AddCondition<PlayerCondition_OpponentHavingBall>();
	transitionHavingBallOpponentHavingBall->AddCondition<PlayerCondition_CanPassBall>();
	//auto transitionHavingBallIdle = havingTheBall->CreateTransition(PlayerState::Idle);
	//transitionHavingBallIdle->AddCondition<PlayerCondition_IsResetLaunched>();

	/* TeamMateHavingTheBall */
	Action<Player>* mateHavingTheBall = m_stateMachine->CreateAction<PlayerAction_TeamMateHavingTheBall>(PlayerState::TeamMateHavingTheBall);
	auto transitionMateHavingBallJustGotBall = mateHavingTheBall->CreateTransition(PlayerState::JustGotTheBall);
	transitionMateHavingBallJustGotBall->AddCondition<PlayerCondition_HavingBall>();
	transitionMateHavingBallJustGotBall->AddCondition<PlayerCondition_IsNotInvincible>();
	transitionMateHavingBallJustGotBall->AddCondition<PlayerCondition_IsBooted>(false);
	auto transitionMateHavingBallOppGotBall = mateHavingTheBall->CreateTransition(PlayerState::OpponentHavingTheBall);
	transitionMateHavingBallOppGotBall->AddCondition<PlayerCondition_OpponentHavingBall>();
	auto transitionMateHavingBallIdle = mateHavingTheBall->CreateTransition(PlayerState::Idle);
	transitionMateHavingBallIdle->AddCondition<PlayerCondition_IsResetLaunched>();

	/* OppHavingTheBall */
	Action<Player>* oppHavingTheBall = m_stateMachine->CreateAction<PlayerAction_OpponentHavingTheBall>(PlayerState::OpponentHavingTheBall);
	auto transitionOppHavingBallJustGotBall = oppHavingTheBall->CreateTransition(PlayerState::JustGotTheBall);
	transitionOppHavingBallJustGotBall->AddCondition<PlayerCondition_HavingBall>();
	transitionOppHavingBallJustGotBall->AddCondition<PlayerCondition_IsNotInvincible>();
	transitionOppHavingBallJustGotBall->AddCondition<PlayerCondition_IsBooted>(false);
	auto transitionOppHavingBallMateGotBall = oppHavingTheBall->CreateTransition(PlayerState::TeamMateHavingTheBall);
	transitionOppHavingBallMateGotBall->AddCondition<PlayerCondition_TeamMateHavingBall>();
	auto transitionOppHavingBallIdle = oppHavingTheBall->CreateTransition(PlayerState::Idle);
	transitionOppHavingBallIdle->AddCondition<PlayerCondition_IsResetLaunched>();

	m_stateMachine->SetState(PlayerState::Idle);
}