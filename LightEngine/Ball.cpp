#include "Ball.h"

#include "Player.h"


void Ball::OnUpdate() {
    if (mCurrentHolder) {
        SetPosition(mCurrentHolder->GetPosition().x, mCurrentHolder->GetPosition().y);
    }
}
