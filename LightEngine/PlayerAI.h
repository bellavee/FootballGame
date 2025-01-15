#pragma once
#include "Entity.h"
#include "StateMachine.h"

class PlayerAI : public Entity
{
public:
	PlayerAI();
protected:
	void OnUpdate() override;
	void OnCollision(Entity* collidedWith) override;
	void OnInitialize() override;

private:
	std::unique_ptr<StateMachine<PlayerAI>> m_stateMachine;
	enum PlayerState
	{
		Idle,
		JustGotTheBall,
		HavingTheBall,
		TeamMateHavingTheBall,
		OpponentHavingTheBall,
		Count
	};
};

