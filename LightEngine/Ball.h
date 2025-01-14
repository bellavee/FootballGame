#pragma once
#include "Entity.h"

class Player;

class Ball : public Entity
{
public:
	void OnUpdate() override;
	void MoveTo(Player* player);

	Player* GetCurrentHolder() const { return mCurrentHolder; }
	void SetCurrentHolder(Player* player) { mCurrentHolder = player; }

private:
	Player* mCurrentHolder = nullptr;

};

