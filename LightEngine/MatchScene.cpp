#include "MatchScene.h"

#include "Constant.h"
#include "Debug.h"

void MatchScene::OnInitialize() {
    CreatePlayers();

    mBall = CreateEntity<Ball>(Constant::BALL_RADIUS, sf::Color::Yellow);
    mBall->SetPosition(250.f, GetWindowHeight() * 0.5f);
}

void MatchScene::OnEvent(const sf::Event& event) {
}

void MatchScene::OnUpdate() {
    DrawField();
}

void MatchScene::CreatePlayers() {
    std::vector<sf::Vector2f> greenPositions = {
        {50.f, 50.f},
        {175.f, 100.f},
        {250.f, GetWindowHeight() * 0.5f},
        {175.f, GetWindowHeight() - 100.f},
        {50.f, GetWindowHeight() - 50.f}
    };

    std::vector<sf::Vector2f> redPositions = {
        {GetWindowWidth() - 50.f, 50.f},
        {GetWindowWidth() - 175.f, 100.f},
        {GetWindowWidth() - 250.f, GetWindowHeight() * 0.5f},
        {GetWindowWidth() - 175.f, GetWindowHeight() - 100.f},
        {GetWindowWidth() - 50.f, GetWindowHeight() - 50.f}
    };

    for (int i = 0; i < Constant::PLAYERS_PER_TEAM; i++) {
        Player* player = CreateEntity<Player>(Constant::PLAYER_RADIUS, sf::Color::Green);
        player->SetPosition(greenPositions[i].x, greenPositions[i].y);
        player->SetTeam(0);
        mTeamGreen.push_back(player);
    }

    for (int i = 0; i < Constant::PLAYERS_PER_TEAM; i++) {
        Player* player = CreateEntity<Player>(Constant::PLAYER_RADIUS, sf::Color::Red);
        player->SetPosition(redPositions[i].x, redPositions[i].y);
        player->SetTeam(1);
        mTeamRed.push_back(player);
    }
}

void MatchScene::DrawField() {
    Debug::DrawLine(
        Constant::GOAL_LINE_MARGIN, 0,
        Constant::GOAL_LINE_MARGIN, GetWindowHeight(),
        sf::Color::White
    );

    Debug::DrawLine(
        GetWindowWidth() - Constant::GOAL_LINE_MARGIN, 0,
        GetWindowWidth() - Constant::GOAL_LINE_MARGIN, GetWindowHeight(),
        sf::Color::White
    );

    Debug::DrawText(GetWindowWidth() / 2.0f, 10.0f, "Score: 0 - 0", 0.5f, 0.0f, sf::Color::White);
}