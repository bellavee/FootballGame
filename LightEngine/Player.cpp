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

	// Update timers
	if (mInvincibilityTimer > 0) {
		mInvincibilityTimer -= dt;
	}

	if (mPassCooldownTimer > 0) {
		mPassCooldownTimer -= dt;
	}

	if (mSpeedBoostTimer > 0) {
		mSpeedBoostTimer -= dt;
		SetSpeed(mBoostSpeed);
	}
	else {
		SetSpeed(mBaseSpeed);
	}

	if (mHasBall) {
		MoveTowardGoal();

		if (CanMakePass()) {
			Player* target = FindBestPassTarget();
			if (target) {
				Ball* ball = mCurrentHolder; 
				GiveBall(target, ball);
			}
		}
	}
	else {
		sf::Vector2f pos = GetPosition();
		if (pos.y <= mZoneMinY) {
			SetDirection(0, 1);
			SetSpeed(150.0f);
		}
		else if (pos.y >= mZoneMaxY) {
			SetDirection(0, -1);
			SetSpeed(150.0f);
		}
	}

	sf::Vector2f pos = GetPosition();
	if (pos.y < mZoneMinY) SetPosition(pos.x, mZoneMinY);
	if (pos.y > mZoneMaxY) SetPosition(pos.x, mZoneMaxY);
}

void Player::OnCollision(Entity* collidedWith) {
	if (dynamic_cast<Player*>(collidedWith)) {
		return;
	}

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

		if ((mTeamSide == 0 && teammatePos.x > myPos.x) ||
			(mTeamSide == 1 && teammatePos.x < myPos.x)) {
			continue;
		}

		float score = 0.0f;

		float distance = std::hypot(teammatePos.x - myPos.x, teammatePos.y - myPos.y);
		if (distance < 50.0f || distance > 300.0f) continue;
		score += (1.0f - (distance / 300.0f)) * 0.4f;

		bool isBlocked = false;
		std::vector<Player*> opponents = mTeamSide == 0 ? matchScene->GetGreenTeam() : matchScene->GetRedTeam();

		for (Player* opponent : opponents) {
			sf::Vector2f oppPos = opponent->GetPosition();
			if (IsOpponentBlockingPass(myPos, teammatePos, oppPos)) {
				isBlocked = true;
				break;
			}
		}

		float progressScore = mTeamSide == 0 ?
			(teammatePos.x / matchScene->GetWindowWidth()) :
			(1.0f - teammatePos.x / matchScene->GetWindowWidth());
		score += progressScore * 0.6f;

		if (score > bestScore) {
			bestScore = score;
			bestTarget = teammate;
		}
	}

	return bestScore > 0.3f ? bestTarget : nullptr;
}

void Player::MoveTowardGoal() {
	sf::Vector2f currentPos = GetPosition();
	float goalX = mTeamSide == 0 ?
		GetScene()->GetWindowWidth() - Constant::GOAL_LINE_MARGIN :
		Constant::GOAL_LINE_MARGIN;

	float targetY = currentPos.y;
	float zoneMinY = mZoneMinY;
	float zoneMaxY = mZoneMaxY;

	MatchScene* matchScene = dynamic_cast<MatchScene*>(GetScene());
	std::vector<Player*> opponents = mTeamSide == 0 ? matchScene->GetGreenTeam() : matchScene->GetRedTeam();

	Player* nearestThreat = nullptr;
	float minThreatDist = FLT_MAX;

	for (Player* opponent : opponents) {
		sf::Vector2f oppPos = opponent->GetPosition();

		if ((mTeamSide == 0 && oppPos.x < currentPos.x) ||
			(mTeamSide == 1 && oppPos.x > currentPos.x)) {
			continue;
		}

		float dist = std::hypot(oppPos.x - currentPos.x, oppPos.y - currentPos.y);
		if (dist < minThreatDist) {
			minThreatDist = dist;
			nearestThreat = opponent;
		}
	}

	if (nearestThreat && minThreatDist < 150.0f) {
		sf::Vector2f threatPos = nearestThreat->GetPosition();
		if (threatPos.y > currentPos.y) {
			targetY = std::max(zoneMinY, currentPos.y - 50.f);
		}
		else {
			targetY = std::min(zoneMaxY, currentPos.y + 50.f);
		}
	}

	float moveX = mTeamSide == 0 ? goalX - 50.f : goalX + 50.f;
	GoToPosition(moveX, targetY);
}


bool Player::IsOpponentBlockingPass(const sf::Vector2f& from,
	const sf::Vector2f& to,
	const sf::Vector2f& oppPos) {
	const float SAFETY_MARGIN = 30.0f;

	float lineLength = std::hypot(to.x - from.x, to.y - from.y);
	float crossProduct = std::abs((to.x - from.x) * (from.y - oppPos.y) -
		(from.x - oppPos.x) * (to.y - from.y));
	float perpDistance = crossProduct / lineLength;

	if (perpDistance > SAFETY_MARGIN) return false;

	float dotProduct = ((oppPos.x - from.x) * (to.x - from.x) +
		(oppPos.y - from.y) * (to.y - from.y)) / (lineLength * lineLength);

	return dotProduct >= 0 && dotProduct <= 1;
}

void Player::DrawDebugInfo(bool isSelected) {
	sf::Vector2f pos = GetPosition();
	float radius = GetRadius();

	if (isSelected) {
		Debug::DrawCircle(
			pos.x, pos.y,
			radius + 5.0f, 
			sf::Color(255, 255, 0, 128)  
		);

		std::string info = "Team: " + std::to_string(mTeamSide) +
			"\nHas Ball: " + (mHasBall ? "Yes" : "No");
		Debug::DrawText(pos.x, pos.y - radius - 20, info, 0.5f, 1.0f, sf::Color::White);
	}

	if (mHasBall) {
		Debug::DrawCircle(
			pos.x, pos.y,
			radius + 3.0f,
			sf::Color(0, 0, 255, 180) 
		);
	}
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
				sf::Color(255, 0, 0, 128) 
			);

			Debug::DrawCircle(
				myPos.x, myPos.y,
				Constant::INTERCEPTION_RANGE,
				sf::Color(255, 0, 0, 64) 
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
