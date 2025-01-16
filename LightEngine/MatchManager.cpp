#include "MatchManager.h"

#include <iostream>
#include <SFML/System/Sleep.hpp>

#include "Constant.h"

MatchManager::MatchManager(std::vector<Player*>* greenTeam, std::vector<Player*>* redTeam, Scene* scene, Ball* ball)
    : mScene(scene), mGreenTeam(greenTeam), mRedTeam(redTeam), mBall(ball), mGreenScore(0), mRedScore(0), mIsPlaying(true)
{
}

void MatchManager::Update() {
    if (!mIsPlaying) return;
    CheckBallPossession();
    CheckScoring();
}

void MatchManager::CheckBallPossession() {
   /* Player* ballCarrier = mBall->GetCurrentHolder();
    if (!ballCarrier) return;

    std::vector<Player*>& opposingTeam =
        (ballCarrier->GetTeam() == 0) ? *mRedTeam : *mGreenTeam;

    for (Player* opponent : opposingTeam) {
        if (opponent->IsColliding(ballCarrier) 
            && opponent->CanMakePass() 
            && !opponent->IsInvincible()) {
            opponent->HoldBall();
            
            sf::Vector2f pos1 = ballCarrier->GetPosition();
            sf::Vector2f pos2 = opponent->GetPosition();
            sf::Vector2f direction = pos1 - pos2;
            float length = std::hypot(direction.x, direction.y);
            if (length > 0) {
                direction.x /= length;
                direction.y /= length;
                ballCarrier->SetPosition(
                    pos1.x + direction.x * 5.0f,
                    pos1.y + direction.y * 5.0f
                );
            }
            break;
        }
    }*/
}

void MatchManager::CheckScoring() {
    if (!mBall) return;

    if (mScoringCooldown > 0) {
        mScoringCooldown -= mBall->GetDeltaTime();
        return;
    }

    sf::Vector2f ballPos = mBall->GetPosition();

    if (ballPos.x >= mScene->GetWindowWidth() - Constant::GOAL_LINE_MARGIN) {
        mGreenScore++;
        mScoringCooldown = 1.0f;
        ResetAfterScoring(1);
    }
    else if (ballPos.x <= Constant::GOAL_LINE_MARGIN) {
        mRedScore++;
        mScoringCooldown = 1.0f;
        ResetAfterScoring(0);
    }
}

void MatchManager::ResetAfterScoring(int teamToReceiveBall) {
    ResetPositions();

    std::vector<Player*>& receivingTeam = (teamToReceiveBall == 0) ? *mGreenTeam : *mRedTeam;

    int randomIndex = rand() % receivingTeam.size();
    Player* selectedPlayer = receivingTeam[randomIndex];

    float centerX = mScene->GetWindowWidth() / 2.0f;
    float centerY = mScene->GetWindowHeight() / 2.0f;
    mBall->SetPosition(centerX, centerY);

    mBall->SetCurrentHolder(nullptr);

    sf::sleep(sf::milliseconds(100));
    //selectedPlayer->GetStateMachine()->SetState(Player::PlayerState::JustGotTheBall);
	selectedPlayer->GiveBall();

    sf::Vector2f playerPos = selectedPlayer->GetPosition();
    mBall->SetPosition(playerPos.x, playerPos.y);
}

void MatchManager::ResetPositions() {
    float windowHeight = static_cast<float>(mScene->GetWindowHeight());
    float windowWidth = static_cast<float>(mScene->GetWindowWidth());

    std::vector<sf::Vector2f> greenPositions = {
        {50.f, 50.f},
        {175.f, windowHeight * 0.25f},
        {250.f, windowHeight * 0.5f},
        {175.f, windowHeight * 0.75f},
        {50.f, windowHeight - 50.f}
    };

    std::vector<sf::Vector2f> redPositions = {
        {windowWidth - 50.f, 50.f},
        {windowWidth - 175.f, windowHeight * 0.25f},
        {windowWidth - 250.f, windowHeight * 0.5f},
        {windowWidth - 175.f, windowHeight * 0.75f},
        {windowWidth - 50.f, windowHeight - 50.f}
    };

    for (size_t i = 0; i < mGreenTeam->size(); i++) {
        (*mGreenTeam)[i]->SetPosition(
            greenPositions[i].x,
            greenPositions[i].y
        );
        (*mGreenTeam)[i]->GetStateMachine()->SetState(Player::PlayerState::Idle);
    }

    for (size_t i = 0; i < mRedTeam->size(); i++) {
        (*mRedTeam)[i]->SetPosition(
            redPositions[i].x,
            redPositions[i].y
        );
        (*mRedTeam)[i]->GetStateMachine()->SetState(Player::PlayerState::Idle);
    }
}
