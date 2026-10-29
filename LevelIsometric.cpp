#include "LevelIsometric.h"
#include <algorithm>
#include <cmath>

LevelIsometric::LevelIsometric(GameContext& ctx, sf::Font& font, int levelId)
    : Level(ctx, font),
    currentRound(0),
    roundTimer(10.f),
    levelId(levelId),
    roundEvaluated(false),
    level4EarlyFinish(false),
    tileSize(90.f),
    bridgeTileSize(50.f)
{
    createPlaceholderTexture();
    loadQuestions();
    loadTextures();

    if (levelId == 2 || levelId == 3) {
        tileCenters[0] = sf::Vector2f(480.f, 540.f);
        tileCenters[1] = sf::Vector2f(640.f, 455.f);
        tileCenters[2] = sf::Vector2f(800.f, 540.f);

        avatarJ1 = std::make_unique<RobloxAvatar>(
            ctx.player1Name, 1, sf::Vector2f(560.f, 590.f), font,
            sf::Color(255, 220, 0), sf::Color(0, 120, 255), sf::Color(0, 200, 80));
        avatarJ2 = std::make_unique<RobloxAvatar>(
            ctx.player2Name, 2, sf::Vector2f(720.f, 590.f), font,
            sf::Color(255, 140, 0), sf::Color(160, 160, 170), sf::Color(160, 60, 220));
    } else if (levelId == 4) {
        setupBinaryBridge(bridgeJ1, sf::Vector2f(460.f, 500.f));
        setupBinaryBridge(bridgeJ2, sf::Vector2f(820.f, 500.f));

        avatarJ1 = std::make_unique<RobloxAvatar>(
            ctx.player1Name, 1, bridgeJ1.cellCenter[0][0], font,
            sf::Color(255, 255, 255), sf::Color(30, 30, 30), sf::Color(30, 30, 30));
        avatarJ2 = std::make_unique<RobloxAvatar>(
            ctx.player2Name, 2, bridgeJ2.cellCenter[0][0], font,
            sf::Color(255, 255, 255), sf::Color(30, 30, 30), sf::Color(30, 30, 30));
    }
}

void LevelIsometric::setupBinaryBridge(BinaryBridgeLayout& bridge, sf::Vector2f centerOnFloor) {
    bridge.origin = IsoRenderer::bridgeOriginForCenter(centerOnFloor, bridgeTileSize, kBridgeRows);
    for (int row = 0; row < kBridgeRows; ++row) {
        for (int col = 0; col < 2; ++col) {
            bridge.cellCenter[row][col] = IsoRenderer::bridgeCell(
                static_cast<float>(col), static_cast<float>(row), bridge.origin, bridgeTileSize);
            bridge.blockVisible[row][col] = true;
        }
    }
    bridge.currentCol = 0;
    bridge.currentRow = 0;
}

void LevelIsometric::createPlaceholderTexture() {
    sf::Image placeholder(sf::Vector2u(640, 360), sf::Color(50, 50, 50));
    placeholderTexture.loadFromImage(placeholder);
}

void LevelIsometric::createWallFallback() {
    sf::Image img(sf::Vector2u(800, 450));
    for (unsigned y = 0; y < 450; ++y) {
        for (unsigned x = 0; x < 800; ++x) {
            img.setPixel(sf::Vector2u(x, y), sf::Color(15, 5, 35 + static_cast<std::uint8_t>((x + y) % 40)));
        }
    }
    texturePared.loadFromImage(img);
}

void LevelIsometric::loadQuestions() {
    if (levelId == 2) {
        database.push_back({
            "Identifica una cadena valida para la ER: r = a(b*)a",
            {"aba", "abb", "aab"}, 0});
        database.push_back({
            "Cual cadena NO pertenece a la ER: r = (ab)+",
            {"abab", "ab", "Epsilon"}, 2});
        database.push_back({
            "Dada la GLC: S -> aSb | ab. Que cadena genera?",
            {"aabb", "aaab", "abab"}, 0});
        database.push_back({
            "Dada la GLC: S -> aSa | bSb | a | b. Cual es valida?",
            {"aba", "aab", "abb"}, 0});
        database.push_back({
            "Las Gramaticas Libres de Contexto (GLC) generan...",
            {"Lenguajes Regulares", "Lenguajes Libres de Contexto", "Lenguajes de Turing"}, 1});
    } else if (levelId == 3) {
        database.push_back({
            "Las pilas siguen LIFO: ultimo en entrar, primero en salir?",
            {"SI", "NO", "Depende"}, 0});
        database.push_back({
            "Apilas A, luego B, luego C. Cual sale primero?",
            {"La A", "La C", "La B"}, 1});
        database.push_back({
            "Que operacion retira el elemento superior de una pila?",
            {"PUSH", "POP", "PEEK"}, 2});
    } else if (levelId == 4) {
        bridgeDatabase.push_back({
            "¿Qué es una clase en programación?",
            "Un molde para crear objetos (CORRECTA)",
            "Un tipo de base de datos",
            0});
        bridgeDatabase.push_back({
            "¿Para qué sirve un hilo en un programa?",
            "Para ejecutar tareas (CORRECTA)",
            "Para guardar imágenes",
            0});
        bridgeDatabase.push_back({
            "¿Para qué sirve el patrón MVC?",
            "Para organizar el código (CORRECTA)",
            "Para aumentar la velocidad de Internet",
            0});
        bridgeDatabase.push_back({
            "¿Qué significa la letra M en MVC?",
            "Modelo (CORRECTA)",
            "Memoria",
            0});
    }
}

void LevelIsometric::loadTextures() {
    std::string basePath = "sources/";
    std::string wallFile, floorFile;

    if (levelId == 2) {
        wallFile = basePath + "parednivel2.jpg";
        floorFile = basePath + "pisonivel2.JPG";
    } else if (levelId == 3) {
        wallFile = basePath + "parednivel3.jpg";
        floorFile = basePath + "pisonivel3.jpeg";
    } else if (levelId == 4) {
        wallFile = basePath + "parednivel4.jpg";
        floorFile = basePath + "pisonivel4.jpeg";
        if (!texturePared.loadFromFile(wallFile)) {
            wallFile = basePath + "parednivel4.webp";
            if (!texturePared.loadFromFile(wallFile)) {
                createWallFallback();
            }
        }
        texturePiso.loadFromFile(floorFile);
        return;
    }

    if (!texturePared.loadFromFile(wallFile)) {
        createWallFallback();
    }
    texturePiso.loadFromFile(floorFile);
}

void LevelIsometric::snapPlayerToCell(RobloxAvatar& avatar, BinaryBridgeLayout& bridge, int row, int col) {
    bridge.currentRow = row;
    bridge.currentCol = col;
    avatar.setPosition(bridge.cellCenter[row][col]);
}

void LevelIsometric::initLevel4Round() {
    roundTimer = 10.f;
    roundEvaluated = false;

    if (avatarJ1->getAlive()) {
        int startRow = currentRound;
        if (currentRound > 0 && bridgeJ1.currentRow < currentRound) {
            startRow = currentRound - 1;
        }
        snapPlayerToCell(*avatarJ1, bridgeJ1, startRow, bridgeJ1.currentCol);
    }
    if (avatarJ2->getAlive()) {
        int startRow = currentRound;
        if (currentRound > 0 && bridgeJ2.currentRow < currentRound) {
            startRow = currentRound - 1;
        }
        snapPlayerToCell(*avatarJ2, bridgeJ2, startRow, bridgeJ2.currentCol);
    }
}

void LevelIsometric::init() {
    roundTimer = 10.f;
    roundEvaluated = false;

    if (levelId == 2 || levelId == 3) {
        avatarJ1->setPosition(sf::Vector2f(560.f, 520.f));
        avatarJ2->setPosition(sf::Vector2f(720.f, 520.f));
        for (int i = 0; i < 3; ++i) {
            tileHitboxes[i] = IsoRenderer::tileHitbox(tileCenters[i], tileSize);
        }
    } else if (levelId == 4) {
        currentRound = 0;
        level4EarlyFinish = false;
        context.level4BothEliminated = false;

        avatarJ1->setAlive(true);
        avatarJ2->setAlive(true);

        for (int row = 0; row < kBridgeRows; ++row) {
            for (int col = 0; col < 2; ++col) {
                bridgeJ1.blockVisible[row][col] = true;
                bridgeJ2.blockVisible[row][col] = true;
            }
        }
        bridgeJ1.currentCol = 0;
        bridgeJ2.currentCol = 0;
        bridgeJ1.currentRow = 0;
        bridgeJ2.currentRow = 0;

        initLevel4Round();
    }
}

void LevelIsometric::evaluateRound() {
    int correctIdx = database[currentRound].correctIndex;

    if (tileHitboxes[correctIdx].contains(avatarJ1->getPosition())) {
        context.player1Score += 15;
    }
    if (tileHitboxes[correctIdx].contains(avatarJ2->getPosition())) {
        context.player2Score += 15;
    }
}

void LevelIsometric::evaluateLevel4Round() {
    const int correctCol = bridgeDatabase[currentRound].correctCol;
    const int row = currentRound;

    auto evaluatePlayer = [&](RobloxAvatar& avatar, BinaryBridgeLayout& bridge) {
        if (!avatar.getAlive()) return;

        const int col = bridge.currentCol;
        if (col == correctCol) {
            if (&avatar == avatarJ1.get()) context.player1Score += 15;
            else context.player2Score += 15;
        } else {
            bridge.blockVisible[row][col] = false;
            avatar.setAlive(false);
        }
    };

    evaluatePlayer(*avatarJ1, bridgeJ1);
    evaluatePlayer(*avatarJ2, bridgeJ2);

    if (!avatarJ1->getAlive() && !avatarJ2->getAlive()) {
        level4EarlyFinish = true;
        context.level4BothEliminated = true;
    }
}

void LevelIsometric::handleBridgeInput(RobloxAvatar& avatar, BinaryBridgeLayout& bridge, int scheme) {
    if (!avatar.getAlive()) return;

    const int row = currentRound;
    const bool onActiveRow = (bridge.currentRow >= row);

    if (scheme == 1) {
        const bool w = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
        const bool a = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        const bool d = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

        if (!onActiveRow && w) {
            if (a) snapPlayerToCell(avatar, bridge, row, 0);
            else if (d) snapPlayerToCell(avatar, bridge, row, 1);
            else snapPlayerToCell(avatar, bridge, row, bridge.currentCol);
        } else if (a) {
            snapPlayerToCell(avatar, bridge, row, 0);
        } else if (d) {
            snapPlayerToCell(avatar, bridge, row, 1);
        }
    } else {
        const bool up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
        const bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
        const bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

        if (!onActiveRow && up) {
            if (left) snapPlayerToCell(avatar, bridge, row, 0);
            else if (right) snapPlayerToCell(avatar, bridge, row, 1);
            else snapPlayerToCell(avatar, bridge, row, bridge.currentCol);
        } else if (left) {
            snapPlayerToCell(avatar, bridge, row, 0);
        } else if (right) {
            snapPlayerToCell(avatar, bridge, row, 1);
        }
    }
}

void LevelIsometric::updateIsoGameplay(float dt, sf::Vector2f minBound, sf::Vector2f maxBound) {
    roundTimer -= dt;

    if (roundTimer > 0.f) {
        avatarJ1->handleInput(dt);
        avatarJ2->handleInput(dt);
        avatarJ1->clampToArea(minBound, maxBound);
        avatarJ2->clampToArea(minBound, maxBound);
    } else {
        if (!roundEvaluated) {
            evaluateRound();
            roundEvaluated = true;
        }
        if (roundTimer < -2.f) {
            currentRound++;
            if (currentRound < static_cast<int>(database.size())) {
                roundTimer = 10.f;
                roundEvaluated = false;
                avatarJ1->setPosition(sf::Vector2f(560.f, 520.f));
                avatarJ2->setPosition(sf::Vector2f(720.f, 520.f));
            }
        }
    }
}

void LevelIsometric::updateLevel4(float dt) {
    if (level4EarlyFinish) return;

    roundTimer -= dt;

    if (roundTimer > 0.f) {
        handleBridgeInput(*avatarJ1, bridgeJ1, 1);
        handleBridgeInput(*avatarJ2, bridgeJ2, 2);
    } else {
        if (!roundEvaluated) {
            evaluateLevel4Round();
            roundEvaluated = true;
            if (level4EarlyFinish) return;
        }
        if (roundTimer < -2.f) {
            currentRound++;
            if (currentRound < kBridgeRows) {
                initLevel4Round();
            }
        }
    }
}

void LevelIsometric::update(float dt) {
    if (levelId == 2 || levelId == 3) {
        updateIsoGameplay(dt, sf::Vector2f(360.f, 410.f), sf::Vector2f(920.f, 630.f));
    } else if (levelId == 4) {
        updateLevel4(dt);
    }
}

void LevelIsometric::drawSectorHUD(sf::RenderWindow& window, const std::string& sectorTitle) {
    sf::Text title(gameFont);
    title.setString(sectorTitle);
    title.setCharacterSize(18);
    title.setFillColor(sf::Color(0, 255, 255));
    title.setPosition(sf::Vector2f(20.f, 12.f));
    window.draw(title);

    sf::Text p1Ctrl(gameFont);
    p1Ctrl.setString("P1 (" + context.player1Name + "): WASD = Mover");
    p1Ctrl.setCharacterSize(13);
    p1Ctrl.setFillColor(sf::Color(255, 0, 255));
    p1Ctrl.setPosition(sf::Vector2f(20.f, 38.f));
    window.draw(p1Ctrl);

    sf::Text p2Ctrl(gameFont);
    p2Ctrl.setString("P2 (" + context.player2Name + "): Flechas = Mover");
    p2Ctrl.setCharacterSize(13);
    p2Ctrl.setFillColor(sf::Color(0, 255, 255));
    p2Ctrl.setPosition(sf::Vector2f(20.f, 58.f));
    window.draw(p2Ctrl);

    int secondsLeft = static_cast<int>(std::ceil(std::max(0.f, roundTimer)));
    sf::RectangleShape timerBox(sf::Vector2f(220.f, 56.f));
    timerBox.setPosition(sf::Vector2f(1020.f, 12.f));
    timerBox.setFillColor(sf::Color(20, 0, 0));
    timerBox.setOutlineThickness(3.f);
    timerBox.setOutlineColor(sf::Color(255, 0, 0));
    window.draw(timerBox);

    sf::Text timerText(gameFont);
    timerText.setString("TIEMPO: " + std::to_string(secondsLeft) + "s");
    timerText.setCharacterSize(26);
    timerText.setFillColor(secondsLeft <= 3 ? sf::Color(255, 80, 80) : sf::Color(255, 0, 0));
    timerText.setPosition(sf::Vector2f(1035.f, 22.f));
    window.draw(timerText);

    sf::Text roundText(gameFont);
    roundText.setString("Ronda " + std::to_string(currentRound + 1) + " / " +
                        std::to_string(static_cast<int>(database.size())));
    roundText.setCharacterSize(15);
    roundText.setFillColor(sf::Color(255, 255, 0));
    roundText.setPosition(sf::Vector2f(580.f, 12.f));
    window.draw(roundText);

    sf::Text questionText(gameFont);
    questionText.setString(database[currentRound].textInUI);
    questionText.setCharacterSize(17);
    questionText.setFillColor(sf::Color(0, 255, 0));
    {
        sf::FloatRect b = questionText.getLocalBounds();
        questionText.setOrigin(sf::Vector2f(b.size.x * 0.5f, 0.f));
        questionText.setPosition(sf::Vector2f(640.f, 82.f));
    }
    window.draw(questionText);

    const float optionY = 110.f;
    const float optionCenters[3] = {220.f, 640.f, 1060.f};
    const sf::Color optionColors[3] = {
        sf::Color(255, 0, 255), sf::Color(0, 255, 255), sf::Color(255, 255, 0)
    };

    for (int i = 0; i < 3; ++i) {
        sf::Text label(gameFont);
        std::string optText = "[" + std::string(IsoRenderer::tileLabelForIndex(i)) + "] " +
                              database[currentRound].textOnTiles[i];
        label.setString(optText);
        label.setCharacterSize(20);
        label.setStyle(sf::Text::Bold);
        label.setFillColor(optionColors[i]);
        label.setOutlineThickness(1.5f);
        label.setOutlineColor(sf::Color(0, 0, 0, 200));
        {
            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(sf::Vector2f(bounds.size.x * 0.5f, 0.f));
            label.setPosition(sf::Vector2f(optionCenters[i], optionY));
        }
        window.draw(label);
    }

    sf::Text escText(gameFont);
    escText.setString("ESC para Salir");
    escText.setCharacterSize(12);
    escText.setFillColor(sf::Color(180, 180, 180));
    escText.setPosition(sf::Vector2f(20.f, 695.f));
    window.draw(escText);

    sf::Text score1(gameFont);
    score1.setString(context.player1Name + ": " + std::to_string(context.player1Score) + " Pts");
    score1.setCharacterSize(15);
    score1.setFillColor(sf::Color(255, 255, 0));
    score1.setPosition(sf::Vector2f(200.f, 695.f));
    window.draw(score1);

    sf::Text score2(gameFont);
    score2.setString(context.player2Name + ": " + std::to_string(context.player2Score) + " Pts");
    score2.setCharacterSize(15);
    score2.setFillColor(sf::Color(255, 255, 0));
    score2.setPosition(sf::Vector2f(900.f, 695.f));
    window.draw(score2);
}

void LevelIsometric::drawLevel4UI(sf::RenderWindow& window) {
    sf::Text title(gameFont);
    title.setString("SECTOR 4 - PUENTE DE TURING");
    title.setCharacterSize(18);
    title.setFillColor(sf::Color(0, 255, 255));
    title.setPosition(sf::Vector2f(20.f, 12.f));
    window.draw(title);

    sf::Text p1Ctrl(gameFont);
    p1Ctrl.setString("P1 (" + context.player1Name + "): A/D = Izq/Der, W = Avanzar fila");
    p1Ctrl.setCharacterSize(12);
    p1Ctrl.setFillColor(sf::Color(255, 0, 255));
    p1Ctrl.setPosition(sf::Vector2f(20.f, 38.f));
    window.draw(p1Ctrl);

    sf::Text p2Ctrl(gameFont);
    p2Ctrl.setString("P2 (" + context.player2Name + "): Flechas = Izq/Der/Avanzar");
    p2Ctrl.setCharacterSize(12);
    p2Ctrl.setFillColor(sf::Color(0, 255, 255));
    p2Ctrl.setPosition(sf::Vector2f(20.f, 56.f));
    window.draw(p2Ctrl);

    int secondsLeft = static_cast<int>(std::ceil(std::max(0.f, roundTimer)));
    sf::RectangleShape timerBox(sf::Vector2f(220.f, 56.f));
    timerBox.setPosition(sf::Vector2f(1020.f, 12.f));
    timerBox.setFillColor(sf::Color(20, 0, 0));
    timerBox.setOutlineThickness(3.f);
    timerBox.setOutlineColor(sf::Color(255, 0, 0));
    window.draw(timerBox);

    sf::Text timerText(gameFont);
    timerText.setString("TIEMPO: " + std::to_string(secondsLeft) + "s");
    timerText.setCharacterSize(26);
    timerText.setFillColor(secondsLeft <= 3 ? sf::Color(255, 80, 80) : sf::Color(255, 0, 0));
    timerText.setPosition(sf::Vector2f(1035.f, 22.f));
    window.draw(timerText);

    sf::Text roundText(gameFont);
    roundText.setString("Fila " + std::to_string(currentRound + 1) + " / " + std::to_string(kBridgeRows));
    roundText.setCharacterSize(15);
    roundText.setFillColor(sf::Color(255, 255, 0));
    roundText.setPosition(sf::Vector2f(580.f, 12.f));
    window.draw(roundText);

    if (currentRound < static_cast<int>(bridgeDatabase.size())) {
        sf::Text questionText(gameFont);
        questionText.setString(bridgeDatabase[currentRound].textInUI);
        questionText.setCharacterSize(16);
        questionText.setFillColor(sf::Color(0, 255, 0));
        {
            sf::FloatRect b = questionText.getLocalBounds();
            questionText.setOrigin(sf::Vector2f(b.size.x * 0.5f, 0.f));
            questionText.setPosition(sf::Vector2f(640.f, 82.f));
        }
        window.draw(questionText);

        sf::Text leftOpt(gameFont);
        leftOpt.setString("IZQUIERDA: " + bridgeDatabase[currentRound].leftOption);
        leftOpt.setCharacterSize(15);
        leftOpt.setFillColor(sf::Color(255, 0, 255));
        leftOpt.setPosition(sf::Vector2f(120.f, 108.f));
        window.draw(leftOpt);

        sf::Text rightOpt(gameFont);
        rightOpt.setString("DERECHA: " + bridgeDatabase[currentRound].rightOption);
        rightOpt.setCharacterSize(15);
        rightOpt.setFillColor(sf::Color(0, 255, 255));
        rightOpt.setPosition(sf::Vector2f(720.f, 108.f));
        window.draw(rightOpt);
    }

    sf::Text escText(gameFont);
    escText.setString("ESC para Salir");
    escText.setCharacterSize(12);
    escText.setFillColor(sf::Color(180, 180, 180));
    escText.setPosition(sf::Vector2f(20.f, 695.f));
    window.draw(escText);

    sf::Text score1(gameFont);
    score1.setString(context.player1Name + ": " + std::to_string(context.player1Score) + " Pts" +
                     (avatarJ1->getAlive() ? "" : " [ELIMINADO]"));
    score1.setCharacterSize(15);
    score1.setFillColor(avatarJ1->getAlive() ? sf::Color(255, 255, 0) : sf::Color(255, 80, 80));
    score1.setPosition(sf::Vector2f(200.f, 695.f));
    window.draw(score1);

    sf::Text score2(gameFont);
    score2.setString(context.player2Name + ": " + std::to_string(context.player2Score) + " Pts" +
                     (avatarJ2->getAlive() ? "" : " [ELIMINADO]"));
    score2.setCharacterSize(15);
    score2.setFillColor(avatarJ2->getAlive() ? sf::Color(255, 255, 0) : sf::Color(255, 80, 80));
    score2.setPosition(sf::Vector2f(900.f, 695.f));
    window.draw(score2);
}

void LevelIsometric::drawIsoScene(sf::RenderWindow& window) {
    IsoRenderer::drawDiorama(window, texturePiso, texturePared);

    for (int i = 0; i < 3; ++i) {
        IsoRenderer::drawNeonTile(window, tileCenters[i], tileSize, IsoRenderer::tileColorForIndex(i));
    }

    if (avatarJ1->getDrawDepth() < avatarJ2->getDrawDepth()) {
        avatarJ1->draw(window);
        avatarJ2->draw(window);
    } else {
        avatarJ2->draw(window);
        avatarJ1->draw(window);
    }
}

void LevelIsometric::drawLevel2UI(sf::RenderWindow& window) {
    drawSectorHUD(window, "SECTOR 2 - RED DE RECONOCIMIENTO");
}

void LevelIsometric::drawLevel3UI(sf::RenderWindow& window) {
    drawSectorHUD(window, "SECTOR 3 - MEMORIA LIFO (AP)");
}

void LevelIsometric::drawLevel2(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    drawIsoScene(window);
    drawLevel2UI(window);
}

void LevelIsometric::drawLevel3(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    drawIsoScene(window);
    drawLevel3UI(window);
}

void LevelIsometric::drawLevel4(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);

    IsoRenderer::drawDiorama(window, texturePiso, texturePared);

    IsoRenderer::drawBinaryBridge(window, bridgeJ1.origin, bridgeTileSize, kBridgeRows,
                                  bridgeJ1.blockVisible, currentRound);
    IsoRenderer::drawBinaryBridge(window, bridgeJ2.origin, bridgeTileSize, kBridgeRows,
                                  bridgeJ2.blockVisible, currentRound);

    if (avatarJ1->getDrawDepth() < avatarJ2->getDrawDepth()) {
        avatarJ1->draw(window);
        avatarJ2->draw(window);
    } else {
        avatarJ2->draw(window);
        avatarJ1->draw(window);
    }

    drawLevel4UI(window);
}

void LevelIsometric::draw(sf::RenderWindow& window) {
    if (levelId == 2) drawLevel2(window);
    else if (levelId == 3) drawLevel3(window);
    else if (levelId == 4) drawLevel4(window);
}

bool LevelIsometric::isFinished() {
    if (levelId == 4) {
        return level4EarlyFinish || currentRound >= kBridgeRows;
    }
    return currentRound >= static_cast<int>(database.size());
}
