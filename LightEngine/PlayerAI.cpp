#include "PlayerAI.h"
#include "PlayerAIActions.h"
#include "PlayerAICondition.h"

PlayerAI::PlayerAI() : m_stateMachine(std::make_unique<StateMachine<PlayerAI>>(this, PlayerState::Count))
{
}

void PlayerAI::OnUpdate()
{
}

void PlayerAI::OnCollision(Entity* collidedWith)
{
}

void PlayerAI::OnInitialize()
{
	//tag
	/* IDLE */
	Action<PlayerAI>* idle = m_stateMachine->CreateAction<PlayerAIAction_Idle>(PlayerState::Idle);

	auto transitionIdleJustHadBall = idle->CreateTransition(PlayerState::JustGotTheBall);
	auto transitionIdleTeamMateHadBall = idle->CreateTransition(PlayerState::TeamMateHavingTheBall);
	auto transitionIdleOpponentHadBall = idle->CreateTransition(PlayerState::OpponentHavingTheBall);
	// add condition

	/* JustGotTheBall */
	Action<PlayerAI>* justGotTheBall = m_stateMachine->CreateAction<PlayerAIAction_JustGotTheBall>(PlayerState::JustGotTheBall);
	auto transitionJustGotTheBallHavingBall = justGotTheBall->CreateTransition(PlayerState::HavingTheBall);
	//condition

	/* HavingBall */
	Action<PlayerAI>* havingTheBall = m_stateMachine->CreateAction<PlayerAIAction_HavingTheBall>(PlayerState::HavingTheBall);
	auto transitionHavingBallTeamMateHavingBall = havingTheBall->CreateTransition(PlayerState::TeamMateHavingTheBall);
	auto transitionHavingBallOpponentHavingBall = havingTheBall->CreateTransition(PlayerState::OpponentHavingTheBall);
	// condition

	/* TeamMateHavingTheBall */
	Action<PlayerAI>* mateHavingTheBall = m_stateMachine->CreateAction<PlayerAIAction_TeamMateHavingTheBall>(PlayerState::TeamMateHavingTheBall);
	auto transitionMateHavingBallJustGotBall = mateHavingTheBall->CreateTransition(PlayerState::JustGotTheBall);
	auto transitionMateHavingBallOppGotBall = mateHavingTheBall->CreateTransition(PlayerState::OpponentHavingTheBall);
	auto transitionMateHavingBallIdle = mateHavingTheBall->CreateTransition(PlayerState::Idle);
	// condition

	/* OppHavingTheBall */
	Action<PlayerAI>* oppHavingTheBall = m_stateMachine->CreateAction<PlayerAIAction_OpponentHavingTheBall>(PlayerState::OpponentHavingTheBall);
	auto transitionOppHavingBallJustGotBall = oppHavingTheBall->CreateTransition(PlayerState::JustGotTheBall);
	auto transitionOppHavingBallmATEGotBall = oppHavingTheBall->CreateTransition(PlayerState::TeamMateHavingTheBall);
	auto transitionOppHavingBallIdle = oppHavingTheBall->CreateTransition(PlayerState::Idle);
	// condition

	m_stateMachine->SetState(PlayerState::Idle);
}
