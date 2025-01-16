#pragma once
#include "Player.h"
#include "Condition.h"

class PlayerCondition_JustHadBall : public Condition<Player>
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_HavingBall : public Condition<Player>
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_TeamMateHavingBall : public Condition<Player>
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_OpponentHavingBall : public Condition<Player>
{
public:
	bool OnTest(Player* owner) override;
};

class PlayerCondition_CanMakePass : public Condition<Player>
{
public:
	bool OnTest(Player* owner) override;
};