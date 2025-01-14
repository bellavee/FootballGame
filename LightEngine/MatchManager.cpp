#include "MatchManager.h"

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

}

void MatchManager::CheckScoring() {
    sf::Vector2f ballPos = mBall->GetPosition();
    if (ballPos.x <= Constant::GOAL_LINE_MARGIN) {
        mRedScore++;
        ResetPositions();
    }
    else if (ballPos.x >= mScene->GetWindowWidth() - Constant::GOAL_LINE_MARGIN) {
        mGreenScore++;
        ResetPositions();
    }
}

void MatchManager::ResetPositions() {
    std::vector<sf::Vector2f> greenPositions = {
        {50.f, 50.f},
        {175.f, 100.f},
        {250.f, mScene->GetWindowHeight() * 0.5f},
        {175.f, mScene->GetWindowHeight() - 100.f},
        {50.f, mScene->GetWindowHeight() - 50.f}
    };

    std::vector<sf::Vector2f> redPositions = {
        {mScene->GetWindowWidth() - 50.f, 50.f},
        {mScene->GetWindowWidth() - 175.f, 100.f},
        {mScene->GetWindowWidth() - 250.f, mScene->GetWindowHeight() * 0.5f},
        {mScene->GetWindowWidth() - 175.f, mScene->GetWindowHeight() - 100.f},
        {mScene->GetWindowWidth() - 50.f, mScene->GetWindowHeight() - 50.f}
    };

    for (size_t i = 0; i < mGreenTeam->size(); i++) {
        (*mGreenTeam)[i]->SetPosition(greenPositions[i].x, greenPositions[i].y);
    }

    for (size_t i = 0; i < mRedTeam->size(); i++) {
        (*mRedTeam)[i]->SetPosition(redPositions[i].x, redPositions[i].y);
    }

    if (mGreenScore > mRedScore) {
        sf::Vector2f pos = (*mRedTeam)[2]->GetPosition();
        mBall->SetPosition(pos.x, pos.y);
    }
    else {
        sf::Vector2f pos = (*mGreenTeam)[2]->GetPosition();
        mBall->SetPosition(pos.x, pos.y);
    }
}
