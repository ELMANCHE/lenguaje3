#include "GameEngine.h"
#include <chrono>

GameEngine::GameEngine(int width, int height)
    : window(sf::VideoMode(sf::Vector2u(width, height)), "Automata Arcade: The Chomsky Challenge"),
    isRunning(false)
{
    window.setFramerateLimit(60);
}

GameEngine::~GameEngine() {
    shutdown();
}

void GameEngine::init() {
    // SFML 3.x: Font usa constructor en lugar de loadFromFile
    try {
        // Intenta cargar desde la carpeta sources
        gameFont = sf::Font("sources/arial.ttf");
    } catch (...) {
        try {
            // Intenta desde /System/Library/Fonts con SFNSMono
            gameFont = sf::Font("/System/Library/Fonts/SFNSMono.ttf");
        } catch (...) {
            try {
                // Intenta desde Geneva
                gameFont = sf::Font("/System/Library/Fonts/Geneva.ttf");
            } catch (...) {
                // Si todo falla, proceder sin fuente
            }
        }
    }
    
    context.currentState = GameState::MAIN_MENU;
    context.player1Name = "Jugador 1";
    context.player2Name = "Jugador 2";
    context.player1Score = 0;
    context.player2Score = 0;
    context.currentLevelIndex = 1;
    
    // Initialize menu input state
    inputName1 = "";
    inputName2 = "";
    editingPlayer1 = true;
    editingPlayer2 = false;
    cursorBlink = 0.f;
    restartBlink = 0.f;

    // Load menu background image
    if (!menuBackgroundTexture.loadFromFile("sources/fondomenu.jpeg")) {
        // Si falla, continuar sin imagen de fondo
    } else {
        menuBackgroundSprite = std::make_unique<sf::Sprite>(menuBackgroundTexture);
        // Scale image to fit window
        sf::Vector2u textureSize = menuBackgroundTexture.getSize();
        sf::Vector2u windowSize = window.getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        menuBackgroundSprite->setScale(sf::Vector2f(scaleX, scaleY));
    }

    isRunning = true;
    playMenuAudio();
}

void GameEngine::playMenuAudio() {
    if (!audioMenu.openFromFile("sources/audiomenu.mp3")) {
        // Si falla, continuar sin audio
    }
    audioMenu.play();
}

void GameEngine::stopMenuAudio() {
    audioMenu.stop();
}

void GameEngine::playGameAudio() {
    if (!audioGame.openFromFile("sources/audiojuego.mp3")) {
        // Si falla, continuar sin audio
    }
    audioGame.play();
}

void GameEngine::stopGameAudio() {
    audioGame.stop();
}

void GameEngine::transitionToState(GameState newState) {
    context.currentState = newState;
    
    if (newState == GameState::EXPLANATION_SCREEN) {
        // SFML 3.x: getStatus() retorna sf::SoundSource::Status (enum)
        if (audioMenu.getStatus() == sf::SoundSource::Status::Playing) stopMenuAudio();
        if (audioGame.getStatus() != sf::SoundSource::Status::Playing) playGameAudio();
    }
    else if (newState == GameState::MAIN_MENU) {
        if (audioGame.getStatus() == sf::SoundSource::Status::Playing) stopGameAudio();
        if (audioMenu.getStatus() != sf::SoundSource::Status::Playing) playMenuAudio();
    }
    else if (newState == GameState::FINAL_SCORE) {
        restartBlink = 0.f;
    }
}

void GameEngine::renderMainMenu() {
    window.clear(sf::Color::Black);

    // Draw background image with dark overlay
    if (menuBackgroundSprite) {
        window.draw(*menuBackgroundSprite);

        // Dark overlay (semi-transparent black rectangle)
        sf::RectangleShape darkOverlay(sf::Vector2f(static_cast<float>(window.getSize().x),
                                                    static_cast<float>(window.getSize().y)));
        darkOverlay.setFillColor(sf::Color(0, 0, 0, 180)); // Alpha 180 para oscurecer
        window.draw(darkOverlay);
    }

    // Title: SQUID GAMES
    sf::Text title(gameFont);
    title.setString("SQUID GAMES");
    title.setCharacterSize(72);
    title.setFillColor(sf::Color(255, 0, 0)); // Rojo
    title.setStyle(sf::Text::Bold);
    // Center the title
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(sf::Vector2f(titleBounds.size.x * 0.5f, 0.f));
    title.setPosition(sf::Vector2f(640.f, 80.f));
    window.draw(title);
    
    // Player 1 input label
    sf::Text p1Label(gameFont);
    p1Label.setString("Jugador 1:");
    p1Label.setCharacterSize(18);
    p1Label.setFillColor(sf::Color(0, 255, 255));
    p1Label.setPosition(sf::Vector2f(200.f, 220.f));
    window.draw(p1Label);
    
    // Player 1 input box (background)
    sf::RectangleShape p1Box(sf::Vector2f(300.f, 50.f));
    p1Box.setPosition(sf::Vector2f(450.f, 210.f));
    p1Box.setFillColor(sf::Color(20, 40, 60));
    p1Box.setOutlineThickness(2.f);
    p1Box.setOutlineColor(editingPlayer1 ? sf::Color(0, 255, 255) : sf::Color(100, 100, 100));
    window.draw(p1Box);
    
    // Player 1 input text with cursor
    sf::Text p1Input(gameFont);
    std::string p1Display = inputName1;
    if (editingPlayer1 && ((int)(cursorBlink * 2) % 2 == 0)) {
        p1Display += "_";
    }
    p1Input.setString(p1Display);
    p1Input.setCharacterSize(20);
    p1Input.setFillColor(sf::Color(0, 255, 0));
    p1Input.setPosition(sf::Vector2f(470.f, 220.f));
    window.draw(p1Input);
    
    // Player 1 char count
    sf::Text p1Count(gameFont);
    p1Count.setString(std::to_string(inputName1.length()) + "/5");
    p1Count.setCharacterSize(14);
    p1Count.setFillColor(sf::Color(200, 200, 200));
    p1Count.setPosition(sf::Vector2f(760.f, 225.f));
    window.draw(p1Count);
    
    // Player 2 input label
    sf::Text p2Label(gameFont);
    p2Label.setString("Jugador 2:");
    p2Label.setCharacterSize(18);
    p2Label.setFillColor(sf::Color(0, 255, 255));
    p2Label.setPosition(sf::Vector2f(200.f, 340.f));
    window.draw(p2Label);
    
    // Player 2 input box (background)
    sf::RectangleShape p2Box(sf::Vector2f(300.f, 50.f));
    p2Box.setPosition(sf::Vector2f(450.f, 330.f));
    p2Box.setFillColor(sf::Color(20, 40, 60));
    p2Box.setOutlineThickness(2.f);
    p2Box.setOutlineColor(editingPlayer2 ? sf::Color(0, 255, 255) : sf::Color(100, 100, 100));
    window.draw(p2Box);
    
    // Player 2 input text with cursor
    sf::Text p2Input(gameFont);
    std::string p2Display = inputName2;
    if (editingPlayer2 && ((int)(cursorBlink * 2) % 2 == 0)) {
        p2Display += "_";
    }
    p2Input.setString(p2Display);
    p2Input.setCharacterSize(20);
    p2Input.setFillColor(sf::Color(0, 255, 0));
    p2Input.setPosition(sf::Vector2f(470.f, 340.f));
    window.draw(p2Input);
    
    // Player 2 char count
    sf::Text p2Count(gameFont);
    p2Count.setString(std::to_string(inputName2.length()) + "/5");
    p2Count.setCharacterSize(14);
    p2Count.setFillColor(sf::Color(200, 200, 200));
    p2Count.setPosition(sf::Vector2f(760.f, 345.f));
    window.draw(p2Count);
}

void GameEngine::renderExplanationScreen() {
    window.clear(sf::Color::Black);

    if (context.currentLevelIndex == 2) {
        auto drawLine = [&](const std::string& text, float y, unsigned size, sf::Color color) {
            sf::Text line(gameFont);
            line.setString(text);
            line.setCharacterSize(size);
            line.setFillColor(color);
            line.setPosition(sf::Vector2f(40.f, y));
            window.draw(line);
        };

        drawLine("SECTOR 2 - RECONOCIMIENTO DE LENGUAJES", 25.f, 26, sf::Color(0, 255, 255));
        drawLine("", 55.f, 16, sf::Color::White);
        drawLine("Expresiones Regulares (ER):", 75.f, 20, sf::Color(0, 255, 0));
        drawLine("Patrones que describen lenguajes con union, concatenacion y la estrella de Kleene.", 105.f, 18, sf::Color(0, 255, 0));
        drawLine("", 135.f, 16, sf::Color::White);
        drawLine("Gramaticas Libres de Contexto (GLC):", 155.f, 20, sf::Color(0, 255, 0));
        drawLine("Reglas que generan lenguajes reemplazando simbolos sin importar lo que los rodea.", 185.f, 18, sf::Color(0, 255, 0));
        drawLine("", 215.f, 16, sf::Color::White);
        drawLine("Como jugar:", 235.f, 22, sf::Color(255, 255, 0));
        drawLine("1. Lee la pregunta de arriba y las tres respuestas con su color:", 265.f, 18, sf::Color(255, 255, 0));
        drawLine("   [MAGENTA], [CYAN] y [AMARILLO].", 295.f, 18, sf::Color(255, 255, 0));
        drawLine("2. En el piso hay tres cuadros vacios con esos mismos colores.", 325.f, 18, sf::Color(255, 255, 0));
        drawLine("3. Tienes 10 segundos para mover tu personaje al cuadro correcto.", 355.f, 18, sf::Color(255, 255, 0));
        drawLine("", 385.f, 16, sf::Color::White);
        drawLine("Jugador 1: WASD   |   Jugador 2: Flechas   |   +15 pts por acierto", 415.f, 18, sf::Color(0, 255, 255));
        drawLine("[ Presiona ENTER para comenzar el nivel ]", 660.f, 20, sf::Color(0, 255, 255));
        return;
    }

    if (context.currentLevelIndex == 3) {
        auto drawLine = [&](const std::string& text, float y, unsigned size, sf::Color color) {
            sf::Text line(gameFont);
            line.setString(text);
            line.setCharacterSize(size);
            line.setFillColor(color);
            line.setPosition(sf::Vector2f(40.f, y));
            window.draw(line);
        };

        drawLine("SISTEMA REBOOT: INGRESO AL SECTOR 3 - MEMORIA SIMPLIFICADA (LIFO)", 20.f, 24, sf::Color(0, 255, 255));
        drawLine("", 50.f, 16, sf::Color::White);
        drawLine("AUTOMATAS DE PILA (AP):", 70.f, 20, sf::Color(0, 255, 0));
        drawLine("Extienden los autómatas finitos con una pila auxiliar LIFO:", 100.f, 18, sf::Color(0, 255, 0));
        drawLine("el ultimo en entrar (Push) es el primero en salir (Pop).", 130.f, 18, sf::Color(0, 255, 0));
        drawLine("", 160.f, 16, sf::Color::White);
        drawLine("MECANICA DEL DESAFIO (lee antes de que inicie el tiempo):", 180.f, 20, sf::Color(255, 255, 0));
        drawLine("", 210.f, 16, sf::Color::White);
        drawLine("- Las pilas siguen LIFO? SI -> cuadrado [MAGENTA]", 230.f, 18, sf::Color(255, 255, 0));
        drawLine("- Apilas A, B, C. Cual sale primero? La 'C' -> [CYAN]", 260.f, 18, sf::Color(255, 255, 0));
        drawLine("- Operacion para sacar el tope? POP -> [AMARILLO]", 290.f, 18, sf::Color(255, 255, 0));
        drawLine("", 320.f, 16, sf::Color::White);
        drawLine("10 segundos por ronda. Parate en el cuadrado correcto del piso.", 340.f, 18, sf::Color(0, 255, 255));
        drawLine("Jugador 1: WASD   |   Jugador 2: Flechas   |   +15 pts por acierto", 370.f, 18, sf::Color(0, 255, 255));
        drawLine("[ Presiona ENTER para comenzar el nivel ]", 660.f, 20, sf::Color(0, 255, 255));
        return;
    }

    if (context.currentLevelIndex == 4) {
        auto drawLine = [&](const std::string& text, float y, unsigned size, sf::Color color) {
            sf::Text line(gameFont);
            line.setString(text);
            line.setCharacterSize(size);
            line.setFillColor(color);
            line.setPosition(sf::Vector2f(40.f, y));
            window.draw(line);
        };

        drawLine("SISTEMA REBOOT: INGRESO AL SECTOR 4 - LA CINTA DE TURING BINARIA", 15.f, 22, sf::Color(0, 255, 255));
        drawLine("", 45.f, 14, sf::Color::White);
        drawLine("REGLAS DEL CODIGO BINARIO:", 65.f, 20, sf::Color(0, 255, 0));
        drawLine("Estas sobre la cinta de una Maquina de Turing de alta tension. El puente consta", 90.f, 16, sf::Color(0, 255, 0));
        drawLine("de 4 filas. Cada paso es una decision binaria: bloque IZQUIERDO o DERECHO.", 115.f, 16, sf::Color(0, 255, 0));
        drawLine("Una opcion procesa el algoritmo; la otra corrompe el sistema y destruye el bloque.", 140.f, 16, sf::Color(0, 255, 0));
        drawLine("", 165.f, 14, sf::Color::White);
        drawLine("GUIA SINTACTICA (LEER ANTES DE AVANZAR):", 185.f, 18, sf::Color(255, 255, 0));
        drawLine("", 210.f, 14, sf::Color::White);
        drawLine("FILA 1: Invertir \"[1][0][1]\" -> IZQUIERDA \"[0][1][0]\" (CORRECTA) | DERECHA \"[1][1][1]\"", 230.f, 16, sf::Color(255, 255, 0));
        drawLine("FILA 2: Leer 0, escribir 1, mover R -> IZQUIERDA \"Borra cinta\" | DERECHA \"Escribe 1\" (CORRECTA)", 260.f, 16, sf::Color(255, 255, 0));
        drawLine("FILA 3: Limite insuperable MT -> IZQUIERDA \"Problema de Parada\" (CORRECTA) | DERECHA \"Palindromo\"", 290.f, 16, sf::Color(255, 255, 0));
        drawLine("FILA 4: Alfabeto MT Binaria -> IZQUIERDA \"{0,1}\" (CORRECTA) | DERECHA \"{A,B,C}\"", 320.f, 16, sf::Color(255, 255, 0));
        drawLine("", 350.f, 14, sf::Color::White);
        drawLine("10 segundos por fila. Si pisas el bloque glitcheado, desaparece y mueres.", 370.f, 16, sf::Color(0, 255, 255));
        drawLine("Jugador 1: A/D = Izq/Der, W = Avanzar  |  Jugador 2: Flechas  |  +15 pts por acierto", 400.f, 16, sf::Color(0, 255, 255));
        drawLine("[ Presiona ENTER para comenzar el nivel ]", 660.f, 20, sf::Color(0, 255, 255));
        return;
    }

    std::string titulo, parrafo, instrucciones;
    
    if (context.currentLevelIndex == 1) {
        titulo = "Automatas Finitos Deterministas (AFD) y No Deterministas (AFND)";
        parrafo = "Un AFD tiene exactamente una transicion de salida para cada simbolo\ndel alfabeto desde cualquier estado. Un AFND puede tener multiples\ntransiciones con el mismo simbolo o saltos vacios (epsilon).";
        instrucciones = "Veran 6 diagramas de autómatas, uno a la vez.\n"
                         "Arriba aparece la pregunta: ¿Es AFD o es AFND?\n"
                         "Tienen 10 segundos por pregunta.\n\n"
                         "Jugador 1: Q = AFND  |  R = AFD\n"
                         "Jugador 2: P = AFND  |  ENTER = AFD\n\n"
                         "+10 puntos por acierto, -5 por error.";
    }
    
    window.clear(sf::Color::Black);

    sf::Text titleText(gameFont);
    titleText.setString(titulo);
    titleText.setCharacterSize(28);
    titleText.setFillColor(sf::Color(0, 255, 255));
    titleText.setPosition(sf::Vector2f(50.f, 30.f));
    window.draw(titleText);

    sf::Text parrafoText(gameFont);
    parrafoText.setString(parrafo);
    parrafoText.setCharacterSize(20);
    parrafoText.setFillColor(sf::Color(0, 255, 0));
    parrafoText.setPosition(sf::Vector2f(60.f, 100.f));
    window.draw(parrafoText);

    sf::Text instrText(gameFont);
    instrText.setString(instrucciones);
    instrText.setCharacterSize(20);
    instrText.setFillColor(sf::Color(255, 255, 0));
    instrText.setPosition(sf::Vector2f(60.f, 250.f));
    window.draw(instrText);

    sf::Text continueText(gameFont);
    continueText.setString("[ Presione ENTER para comenzar el nivel ]");
    continueText.setCharacterSize(24);
    continueText.setFillColor(sf::Color(0, 255, 255));
    continueText.setPosition(sf::Vector2f(300.f, 650.f));
    window.draw(continueText);
}

void GameEngine::renderFinalScore() {
    window.clear(sf::Color(10, 10, 10));

    sf::Text titleText(gameFont);
    titleText.setString("RESULTADOS FINALES");
    titleText.setCharacterSize(42);
    titleText.setFillColor(sf::Color(0, 255, 255));
    {
        sf::FloatRect b = titleText.getLocalBounds();
        titleText.setOrigin(sf::Vector2f(b.size.x * 0.5f, 0.f));
        titleText.setPosition(sf::Vector2f(640.f, 60.f));
    }
    window.draw(titleText);

    sf::RectangleShape tableBg(sf::Vector2f(700.f, 180.f));
    tableBg.setPosition(sf::Vector2f(290.f, 180.f));
    tableBg.setFillColor(sf::Color(20, 20, 30));
    tableBg.setOutlineThickness(2.f);
    tableBg.setOutlineColor(sf::Color(0, 255, 255, 150));
    window.draw(tableBg);

    sf::Text headerText(gameFont);
    headerText.setString("JUGADOR          PUNTUACION");
    headerText.setCharacterSize(18);
    headerText.setFillColor(sf::Color(0, 255, 255));
    headerText.setPosition(sf::Vector2f(340.f, 200.f));
    window.draw(headerText);

    sf::Text p1Text(gameFont);
    p1Text.setString(context.player1Name + "              " + std::to_string(context.player1Score) + " Pts");
    p1Text.setCharacterSize(26);
    p1Text.setFillColor(sf::Color(255, 0, 255));
    p1Text.setPosition(sf::Vector2f(340.f, 250.f));
    window.draw(p1Text);

    sf::Text p2Text(gameFont);
    p2Text.setString(context.player2Name + "              " + std::to_string(context.player2Score) + " Pts");
    p2Text.setCharacterSize(26);
    p2Text.setFillColor(sf::Color(0, 255, 255));
    p2Text.setPosition(sf::Vector2f(340.f, 300.f));
    window.draw(p2Text);

    std::string winnerMsg;
    sf::Color winnerColor;

    if (context.level4BothEliminated) {
        winnerMsg = "¡FIN DEL JUEGO: AMBOS PROCESADORES FUERON ELIMINADOS!";
        winnerColor = sf::Color(255, 0, 80);
    } else if (context.player1Score > context.player2Score) {
        winnerMsg = "¡EL GANADOR DEFINITIVO ES: " + context.player1Name + "!";
        winnerColor = sf::Color(0, 255, 0);
    } else if (context.player2Score > context.player1Score) {
        winnerMsg = "¡EL GANADOR DEFINITIVO ES: " + context.player2Name + "!";
        winnerColor = sf::Color(0, 255, 0);
    } else {
        winnerMsg = "¡EMPATE DE COMPUTABILIDAD!";
        winnerColor = sf::Color(255, 255, 0);
    }

    sf::Text winnerText(gameFont);
    winnerText.setString(winnerMsg);
    winnerText.setCharacterSize(context.level4BothEliminated ? 22 : 30);
    winnerText.setFillColor(winnerColor);
    {
        sf::FloatRect b = winnerText.getLocalBounds();
        winnerText.setOrigin(sf::Vector2f(b.size.x * 0.5f, 0.f));
        winnerText.setPosition(sf::Vector2f(640.f, 420.f));
    }
    window.draw(winnerText);

    if (static_cast<int>(restartBlink * 2.f) % 2 == 0) {
        sf::Text restartText(gameFont);
        restartText.setString("[ PRESIONE 'R' PARA REINICIAR EL SISTEMA / VOLVER AL MENU ]");
        restartText.setCharacterSize(16);
        restartText.setFillColor(sf::Color(0, 255, 255));
        {
            sf::FloatRect b = restartText.getLocalBounds();
            restartText.setOrigin(sf::Vector2f(b.size.x * 0.5f, 0.f));
            restartText.setPosition(sf::Vector2f(640.f, 620.f));
        }
        window.draw(restartText);
    }
}

void GameEngine::handleInput() {
    // SFML 3.x: pollEvent retorna std::optional<sf::Event>
    while (auto event = window.pollEvent()) {
        // Cerrar ventana
        if (event->is<sf::Event::Closed>()) {
            isRunning = false;
            return;
        }
        
        // Manejo de texto (para menú - entrada de nombres)
        if (auto textEvent = event->getIf<sf::Event::TextEntered>()) {
            if (context.currentState == GameState::MAIN_MENU) {
                unsigned int charCode = textEvent->unicode;
                
                // Permitir entrada solo si estamos en modo edición
                if (editingPlayer1) {
                    if (charCode == 8) { // Backspace
                        if (!inputName1.empty()) {
                            inputName1.pop_back();
                        }
                    } else if (charCode >= 32 && charCode < 127) { // Caracteres imprimibles
                        if (inputName1.length() < 5) { // Máximo 5 caracteres
                            inputName1 += static_cast<char>(charCode);
                        }
                    }
                } else if (editingPlayer2) {
                    if (charCode == 8) { // Backspace
                        if (!inputName2.empty()) {
                            inputName2.pop_back();
                        }
                    } else if (charCode >= 32 && charCode < 127) { // Caracteres imprimibles
                        if (inputName2.length() < 5) { // Máximo 5 caracteres
                            inputName2 += static_cast<char>(charCode);
                        }
                    }
                }
            }
        }
        
        // Tecla presionada
        if (auto keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            if (keyEvent->code == sf::Keyboard::Key::Escape) {
                isRunning = false;
                return;
            }
            
            if (context.currentState == GameState::MAIN_MENU) {
                if (keyEvent->code == sf::Keyboard::Key::Tab) {
                    // Cambiar entre campos
                    editingPlayer1 = !editingPlayer1;
                    editingPlayer2 = !editingPlayer2;
                }
                // ENTER no hace nada por ahora (sin botón de inicio)
            }
            // Temporalmente deshabilitado - sin niveles
            /*
            else if (context.currentState == GameState::EXPLANATION_SCREEN) {
                if (keyEvent->code == sf::Keyboard::Key::Enter) {
                    switch (context.currentLevelIndex) {
                        case 1:
                            transitionToState(GameState::LEVEL_1);
                            currentLevel = std::make_unique<Level1>(context, gameFont);
                            currentLevel->init();
                            break;
                        case 2:
                            transitionToState(GameState::LEVEL_2);
                            currentLevel = std::make_unique<LevelIsometric>(context, gameFont, 2);
                            currentLevel->init();
                            break;
                        case 3:
                            transitionToState(GameState::LEVEL_3);
                            currentLevel = std::make_unique<LevelIsometric>(context, gameFont, 3);
                            currentLevel->init();
                            break;
                        case 4:
                            transitionToState(GameState::LEVEL_4);
                            currentLevel = std::make_unique<LevelIsometric>(context, gameFont, 4);
                            currentLevel->init();
                            break;
                    }
                }
            }
            else if (context.currentState == GameState::FINAL_SCORE) {
                if (keyEvent->code == sf::Keyboard::Key::R) {
                    context.player1Score = 0;
                    context.player2Score = 0;
                    context.currentLevelIndex = 1;
                    context.level4BothEliminated = false;
                    inputName1 = "";
                    inputName2 = "";
                    editingPlayer1 = true;
                    editingPlayer2 = false;
                    transitionToState(GameState::MAIN_MENU);
                    currentLevel = nullptr;
                }
            }
            */
        }
    }
}

void GameEngine::update(float dt) {
    cursorBlink += dt;
    if (cursorBlink > 1.0f) {
        cursorBlink = 0.0f;
    }

    if (context.currentState == GameState::FINAL_SCORE) {
        restartBlink += dt;
        if (restartBlink > 1.5f) {
            restartBlink = 0.f;
        }
    }

    // Temporalmente deshabilitado - sin niveles
    /*
    if (currentLevel) {
        currentLevel->update(dt);

        if (currentLevel->isFinished()) {
            context.currentLevelIndex++;

            if (context.currentLevelIndex > 4) {
                transitionToState(GameState::FINAL_SCORE);
            } else {
                transitionToState(GameState::EXPLANATION_SCREEN);
            }
            currentLevel = nullptr;
        }
    }
    */
}

void GameEngine::render() {
    window.clear(sf::Color::Black);

    switch (context.currentState) {
        case GameState::MAIN_MENU:
            renderMainMenu();
            break;
        // Temporalmente deshabilitado - sin niveles
        /*
        case GameState::EXPLANATION_SCREEN:
            renderExplanationScreen();
            break;
        case GameState::LEVEL_1:
        case GameState::LEVEL_2:
        case GameState::LEVEL_3:
        case GameState::LEVEL_4:
            if (currentLevel) {
                currentLevel->draw(window);
            }
            break;
        case GameState::FINAL_SCORE:
            renderFinalScore();
            break;
        */
        default:
            break;
    }

    window.display();
}

void GameEngine::run() {
    sf::Clock clock;
    
    while (isRunning && window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        handleInput();
        update(dt);
        render();
    }
}

void GameEngine::shutdown() {
    stopMenuAudio();
    stopGameAudio();
    window.close();
    isRunning = false;
}
