
#include <SFML/Graphics.hpp>
#include <iostream>

#include "GameManager.h"
#include "SampleScene.h"

#include <cstdlib>
#include <crtdbg.h>

#include "MatchScene.h"

int main() 
{ 
    GameManager* pInstance = GameManager::Get();

    pInstance->CreateWindow(1280, 720, "Rugby Game");

    pInstance->LaunchScene<MatchScene>();
    return 0;
}
