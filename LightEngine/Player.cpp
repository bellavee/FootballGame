#include "Player.h"

#include "Ball.h"

void Player::OnInitialize() {
	mHasBall = false;
	SetDirection(0, 1);
	SetSpeed(150.0f);
}

void Player::OnUpdate() {
	sf::Vector2f pos = GetPosition();
	if (pos.y <= mZoneMinY) {
		SetDirection(0, 1); 
		SetSpeed(150.0f);
	}
	else if (pos.y >= mZoneMaxY) {
		SetDirection(0, -1);
		SetSpeed(150.0f);
	}

	pos = GetPosition();
	if (pos.y < mZoneMinY) SetPosition(pos.x, mZoneMinY);
	if (pos.y > mZoneMaxY) SetPosition(pos.x, mZoneMaxY);
}

void Player::OnCollision(Entity* collidedWith) {
	if (Ball* ball = dynamic_cast<Ball*>(collidedWith)) {
		HoldBall(ball);
	}
}

void Player::HoldBall(Ball* ball) {
	mHasBall = true;
	ball->SetCurrentHolder(this);
}

void Player::LoseBall(Ball* ball) {
	mHasBall = false;
	if (ball->GetCurrentHolder() != this) {
		ball->SetCurrentHolder(nullptr);
	}
}
