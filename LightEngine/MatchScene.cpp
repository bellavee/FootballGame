#include "MatchScene.h"

#include "Constant.h"
#include "Debug.h"
#include "MatchManager.h"

void MatchScene::OnInitialize() {
    CreatePlayers();

    mBall = CreateEntity<Ball>(Constant::BALL_RADIUS, sf::Color::Yellow);
    mBall->SetPosition(250.f, GetWindowHeight() * 0.5f);
    mBallDirection = 1.0f;
    mMatchManager = new MatchManager(&mTeamGreen, &mTeamRed, this, mBall);
}

void MatchScene::OnEvent(const sf::Event& event) {

}

void MatchScene::OnUpdate() {
    mMatchManager->Update();
    DrawGoalLines();
	DrawZones();
    DebugBall();
}

void MatchScene::CreatePlayers() {
    float halfHeight = GetWindowHeight() / 2.0f;
    float quarterHeight = GetWindowHeight() / 4.0f;

    std::vector<std::pair<float, float>> zoneBounds = {
        {0, halfHeight},
        {0, halfHeight},
        {quarterHeight, 3 * quarterHeight},
        {halfHeight, GetWindowHeight()}, 
        {halfHeight, GetWindowHeight()} 
    };

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
        player->SetZoneBounds(zoneBounds[i].first, zoneBounds[i].second);
        mTeamGreen.push_back(player);
    }

    for (int i = 0; i < Constant::PLAYERS_PER_TEAM; i++) {
        Player* player = CreateEntity<Player>(Constant::PLAYER_RADIUS, sf::Color::Red);
        player->SetPosition(redPositions[i].x, redPositions[i].y);
        player->SetTeam(1);
        player->SetZoneBounds(zoneBounds[i].first, zoneBounds[i].second);
        mTeamRed.push_back(player);
    }
}

void MatchScene::DrawGoalLines() {
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

    std::string scoreText = "Score: " +
        std::to_string(mMatchManager->GetGreenScore()) + " - " +
        std::to_string(mMatchManager->GetRedScore());
    Debug::DrawText(GetWindowWidth() / 2.0f, 10.0f, scoreText, 0.5f, 0.0f, sf::Color::White);
}

void MatchScene::DrawZones() {
    float halfHeight = GetWindowHeight() / 2.0f;
    float quarterHeight = GetWindowHeight() / 4.0f;

    Debug::DrawLine(0, halfHeight, GetWindowWidth(), halfHeight, sf::Color::Blue);
    Debug::DrawLine(0, quarterHeight, GetWindowWidth(), quarterHeight, sf::Color::Magenta);
    Debug::DrawLine(0, 3 * quarterHeight, GetWindowWidth(), 3 * quarterHeight, sf::Color::Yellow);

    DrawZoneNumber(1, quarterHeight / 2);
    DrawZoneNumber(2, GetWindowHeight() - quarterHeight / 2);
    DrawZoneNumber(3, halfHeight);
}

void MatchScene::HighlightZone(float top, float bottom) {
    Debug::DrawLine(0, top, GetWindowWidth(), top, sf::Color::Red);
    Debug::DrawLine(0, bottom, GetWindowWidth(), bottom, sf::Color::Red);
    Debug::DrawLine(0, top, 0, bottom, sf::Color::Red);
    Debug::DrawLine(GetWindowWidth(), top, GetWindowWidth(), bottom, sf::Color::Red);
}

void MatchScene::DrawZoneNumber(int number, float y) {
    Debug::DrawText(
        GetWindowWidth() / 2.0f,
        y,
        std::to_string(number),
        0.5f, 0.5f,
        sf::Color::Red
    );
}

void MatchScene::DebugBall() {
    float ballSpeed = 200.0f;
    float currentY = mBall->GetPosition().y;
    float nextY = currentY + (mBallDirection * ballSpeed * GetDeltaTime());

    if (nextY <= 0 || nextY >= GetWindowHeight()) {
        mBallDirection *= -1.0f;
        nextY = currentY + (mBallDirection * ballSpeed * GetDeltaTime());
    }

    mBall->SetPosition(mBall->GetPosition().x, nextY);
}