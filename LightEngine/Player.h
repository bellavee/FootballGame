#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
	void SetTeam(int team) { mTeamSide = team; }
	int GetTeam() const { return mTeamSide; }

private:
	int mTeamSide;

};

