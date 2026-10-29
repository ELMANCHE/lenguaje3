#pragma once
#include "Level.h"
#include "RobloxAvatar.h"
#include "IsoRenderer.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

struct IsometricQuestion {
    std::string textInUI;
    std::string textOnTiles[3];
    int correctIndex;
};

struct BinaryBridgeRow {
    std::string textInUI;
    std::string leftOption;
    std::string rightOption;
    int correctCol;
};

struct BinaryBridgeLayout {
    sf::Vector2f origin;
    sf::Vector2f cellCenter[4][2];
    bool blockVisible[4][2];
    int currentCol;
    int currentRow;
};

class LevelIsometric : public Level {
private:
    static constexpr int kBridgeRows = 4;

    std::unique_ptr<RobloxAvatar> avatarJ1;
    std::unique_ptr<RobloxAvatar> avatarJ2;

    std::vector<IsometricQuestion> database;
    std::vector<BinaryBridgeRow> bridgeDatabase;
    int currentRound;
    float roundTimer;
    int levelId;
    bool roundEvaluated;
    bool level4EarlyFinish;

    sf::FloatRect tileHitboxes[3];
    sf::Vector2f tileCenters[3];
    float tileSize;

    BinaryBridgeLayout bridgeJ1;
    BinaryBridgeLayout bridgeJ2;
    float bridgeTileSize;

    sf::Texture texturePared;
    sf::Texture texturePiso;
    sf::Texture placeholderTexture;

public:
    LevelIsometric(GameContext& ctx, sf::Font& font, int levelId);

    void init() override;
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    bool isFinished() override;

private:
    void loadQuestions();
    void loadTextures();
    void createPlaceholderTexture();
    void evaluateRound();
    void setupBinaryBridge(BinaryBridgeLayout& bridge, sf::Vector2f centerOnFloor);
    void initLevel4Round();
    void snapPlayerToCell(RobloxAvatar& avatar, BinaryBridgeLayout& bridge, int row, int col);
    void handleBridgeInput(RobloxAvatar& avatar, BinaryBridgeLayout& bridge, int scheme);
    void evaluateLevel4Round();

    void updateIsoGameplay(float dt, sf::Vector2f minBound, sf::Vector2f maxBound);
    void updateLevel4(float dt);

    void drawIsoScene(sf::RenderWindow& window);
    void drawLevel2(sf::RenderWindow& window);
    void drawLevel3(sf::RenderWindow& window);
    void drawLevel4(sf::RenderWindow& window);
    void drawLevel2UI(sf::RenderWindow& window);
    void drawLevel3UI(sf::RenderWindow& window);
    void drawLevel4UI(sf::RenderWindow& window);
    void drawSectorHUD(sf::RenderWindow& window, const std::string& sectorTitle);
    void createWallFallback();
};
