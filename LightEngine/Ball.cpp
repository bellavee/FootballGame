#include "Ball.h"


void Ball::OnUpdate() {
    if (mCurrentHolder) {
        SetPosition(mCurrentHolder->GetPosition().x, mCurrentHolder->GetPosition().y);
    }
}

void Ball::MoveTo(Player* player) {
    SetPosition(player->GetPosition().x, player->GetPosition().y);
}
