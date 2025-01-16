#pragma once
#include "Entity.h"
#include "Player.h"


class Ball : public Entity
{
public:
	void OnUpdate() override;
	void MoveTo(Player* player);

	Player* GetCurrentHolder() const { return mCurrentHolder; }
	int GetCurrentHolderTeam() { return mCurrentHolder->GetTeam(); }
	void SetCurrentHolder(Player* player) { mCurrentHolder = player; }

private:
	Player* mCurrentHolder = nullptr;

};

