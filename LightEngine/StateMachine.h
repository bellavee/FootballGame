#pragma once

#include <vector>

#include "Action.h"

class Entity;

template<typename T>
class StateMachine
{
protected:
	std::vector<Action<T>*> mActions;
	int mCurrentState;

	T* mOwner;

public:
	StateMachine(T* owner, int stateCount);
	~StateMachine();

	void Update();
	void SetState(int state);
	int GetCurrentState() const { return mCurrentState; }
	void OnCollision(Entity* collidedWith);

	template<typename U>
	U* CreateAction(int state);
};

#include "StateMachine.inl"

