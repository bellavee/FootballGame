#include "Player.h"

#include "Ball.h"
#include "Constant.h"
#include "Debug.h"
#include "MatchScene.h"

void Player::OnInitialize() {
	mHasBall = false;
	SetDirection(0, 1);
	SetSpeed(150.0f);
}

void Player::OnUpdate() {
	float dt = GetDeltaTime();

	if (mInvincibilityTimer > 0) {
		mInvincibilityTimer -= dt;
	}

	if (mPassCooldownTimer > 0) {
		mPassCooldownTimer -= dt;
	}

	if (mSpeedBoostTimer > 0) {
		mSpeedBoostTimer -= dt;
		SetSpeed(Constant::PLAYER_SPEED * 1.5f);
	}
	else {
		SetSpeed(Constant::PLAYER_SPEED);
	}

	MatchScene* matchScene = dynamic_cast<MatchScene*>(GetScene());
	std::vector<Player*> opposingTeam = mTeamSide == 0 ?
		matchScene->GetRedTeam() : matchScene->GetGreenTeam();

	if (mHasBall) {
		HandleBallCarrierBehavior(opposingTeam);
	}
	else {
		if (Ball* ball = matchScene->GetBall()) {
			Player* ballCarrier = ball->GetCurrentHolder();
			if (ballCarrier) {
				if (ballCarrier->GetTeam() == mTeamSide) {
					HandleSupportingBehavior(ballCarrier);
				}
				else {
					HandleDefensiveBehavior(ballCarrier);
				}
			}
			else {
				HandleFreeBallBehavior(ball);
			}
		}
	}

	sf::Vector2f pos = GetPosition();
	if (pos.y < mZoneMinY) SetPosition(pos.x, mZoneMinY);
	if (pos.y > mZoneMaxY) SetPosition(pos.x, mZoneMaxY);
}

void Player::OnCollision(Entity* collidedWith) {
	if (Ball* ball = dynamic_cast<Ball*>(collidedWith)) {
		if (ball->GetCurrentHolder() == nullptr) {
			HoldBall(ball);
		}
	}
}

void Player::HoldBall(Ball* ball) {
	if (Player* currentHolder = ball->GetCurrentHolder()) {
		currentHolder->LoseBall(ball);
	}

	mHasBall = true;
	mCurrentHolder = ball;
	ball->SetCurrentHolder(this);

	mInvincibilityTimer = Constant::INVINCIBILITY_DURATION;
	mPassCooldownTimer = Constant::PASS_COOLDOWN;
	mSpeedBoostTimer = Constant::SPEED_BOOST_DURATION;
}

void Player::LoseBall(Ball* ball) {
	mHasBall = false;
	mCurrentHolder = nullptr;
	if (ball->GetCurrentHolder() == this) {
		ball->SetCurrentHolder(nullptr);
	}
}

void Player::GiveBall(Player* player, Ball* ball) {
	if (!player->IsInvincible() && player->CanMakePass()) {
		LoseBall(ball);
		ball->MoveTo(player);
		ball->SetCurrentHolder(player);
		player->HoldBall(ball);
	}
}

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

void Player::HandleBallCarrierBehavior(const std::vector<Player*>& opposingTeam) {
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
		Player* passTarget = FindBestPassTarget();
		if (passTarget) {
			GiveBall(passTarget, mCurrentHolder);
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

void Player::HandleSupportingBehavior(Player* ballCarrier) {
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

void Player::HandleDefensiveBehavior(Player* ballCarrier) {
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

void Player::HandleFreeBallBehavior(Ball* ball) {
	sf::Vector2f ballPos = ball->GetPosition();

	if (ballPos.y >= mZoneMinY && ballPos.y <= mZoneMaxY) {
		GoToPosition(ballPos.x, ballPos.y);
	}
}

void Player::ResetStates() {
	mHasBall = false;
	mCurrentHolder = nullptr;
	mInvincibilityTimer = 0.0f;
	mPassCooldownTimer = 0.0f;
	mSpeedBoostTimer = 0.0f;
	SetSpeed(Constant::PLAYER_SPEED);
}