#pragma once

#include "Action.h"
#include "Constant.h"
#include "Player.h"

class PlayerAction_Idle : public Action<Player>
{
public:
	void OnStart(Player* player) override;
	void OnUpdate(Player* player) override {}
	void OnEnd(Player* player) override {}
	void OnCollision(Player* player, Entity* collidedWith) override;
};

class PlayerAction_JustGotTheBall : public Action<Player>
{
public:
	void OnStart(Player* player) override;
	void OnUpdate(Player* player) override;
	void OnEnd(Player* player) override;
};

class PlayerAction_HavingTheBall : public Action<Player>
{
public:
	void OnStart(Player* player) override {}
	void OnUpdate(Player* player) override;
	void OnEnd(Player* player) override {}
};

class PlayerAction_TeamMateHavingTheBall : public Action<Player>
{
public:
	void OnStart(Player* player) override {}
	void OnUpdate(Player* player) override;
	void OnEnd(Player* player) override {}
};

class PlayerAction_OpponentHavingTheBall : public Action<Player>
{
public:
	void OnStart(Player* player) override {}
	void OnUpdate(Player* player) override;
	void OnEnd(Player* player) override {}
};