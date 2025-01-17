#include "Ball.h"
#include "MatchScene.h"

void Ball::OnInitialize()
{
    SetTag(MatchScene::Tag::BALL);
}

void Ball::OnUpdate() {
    if (mCurrentHolder) {
        SetPosition(mCurrentHolder->GetPosition().x, mCurrentHolder->GetPosition().y);
    }
}

void Ball::MoveTo(Player* player) {
    SetPosition(player->GetPosition().x, player->GetPosition().y);
}
