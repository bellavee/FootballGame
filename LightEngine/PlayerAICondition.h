#pragma once
#include "PlayerAI.h"
#include "Condition.h"

class PlayerAICondition : public Condition<PlayerAI>
{
public:
	bool OnTest(PlayerAI* owner) override;
};

