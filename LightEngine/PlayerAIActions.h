#pragma once

#include "Action.h"
#include "PlayerAI.h"

class PlayerAIAction_Idle : public Action<PlayerAI>
{
public:
	void OnStart(PlayerAI* player) override {}
	void OnUpdate(PlayerAI* player) override {}
	void OnEnd(PlayerAI* player) override {}
};

class PlayerAIAction_JustGotTheBall : public Action<PlayerAI>
{
public:
	void OnStart(PlayerAI* player) override {}
	void OnUpdate(PlayerAI* player) override {}
	void OnEnd(PlayerAI* player) override {}
};

class PlayerAIAction_HavingTheBall : public Action<PlayerAI>
{
public:
	void OnStart(PlayerAI* player) override {}
	void OnUpdate(PlayerAI* player) override {}
	void OnEnd(PlayerAI* player) override {}
};

class PlayerAIAction_TeamMateHavingTheBall : public Action<PlayerAI>
{
public:
	void OnStart(PlayerAI* player) override {}
	void OnUpdate(PlayerAI* player) override {}
	void OnEnd(PlayerAI* player) override {}
};

class PlayerAIAction_OpponentHavingTheBall : public Action<PlayerAI>
{
public:
	void OnStart(PlayerAI* player) override {}
	void OnUpdate(PlayerAI* player) override {}
	void OnEnd(PlayerAI* player) override {}
};