#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdint>

class RobloxAvatar {
private:
    std::string name;
    sf::Vector2f position;
    float speed;
    int controlScheme;
    sf::Color headColor;
    sf::Color torsoColor;
    sf::Color legsColor;
    sf::Text nameTag;
    bool isAlive = true;

    static sf::Color shadeColor(sf::Color base, float factor) {
        return sf::Color(
            static_cast<std::uint8_t>(base.r * factor),
            static_cast<std::uint8_t>(base.g * factor),
            static_cast<std::uint8_t>(base.b * factor),
            base.a);
    }

    static void drawBlock(sf::RenderWindow& window, sf::Vector2f center, sf::Vector2f size,
                          sf::Color fill, sf::Color sideDark, sf::Color sideLight) {
        const float depth = size.x * 0.18f;
        const float hw = size.x * 0.5f;
        const float hh = size.y * 0.5f;

        sf::Vector2f topLeft(center.x - hw, center.y - hh);
        sf::Vector2f topRight(center.x + hw, center.y - hh);
        sf::Vector2f botLeft(center.x - hw, center.y + hh);
        sf::Vector2f botRight(center.x + hw, center.y + hh);
        sf::Vector2f depthOff(depth, depth * 0.55f);

        sf::ConvexShape rightFace;
        rightFace.setPointCount(4);
        rightFace.setPoint(0, topRight);
        rightFace.setPoint(1, botRight);
        rightFace.setPoint(2, botRight + depthOff);
        rightFace.setPoint(3, topRight + depthOff);
        rightFace.setFillColor(sideDark);
        window.draw(rightFace);

        sf::ConvexShape topFace;
        topFace.setPointCount(4);
        topFace.setPoint(0, topLeft);
        topFace.setPoint(1, topRight);
        topFace.setPoint(2, topRight + depthOff);
        topFace.setPoint(3, topLeft + depthOff);
        topFace.setFillColor(sideLight);
        window.draw(topFace);

        sf::RectangleShape front(sf::Vector2f(size.x, size.y));
        front.setOrigin(sf::Vector2f(hw, hh));
        front.setPosition(center);
        front.setFillColor(fill);
        front.setOutlineThickness(1.5f);
        front.setOutlineColor(sf::Color(20, 20, 20));
        window.draw(front);
    }

public:
    RobloxAvatar(std::string pName, int scheme, sf::Vector2f spawn,
                 const sf::Font& font,
                 sf::Color head, sf::Color torso, sf::Color legs)
        : name(std::move(pName)), position(spawn), speed(220.f), controlScheme(scheme),
          headColor(head), torsoColor(torso), legsColor(legs), nameTag(font)
    {
        nameTag.setString(name);
        nameTag.setCharacterSize(12);
        nameTag.setFillColor(sf::Color::White);
    }

    void handleInput(float dt) {
        sf::Vector2f movement(0.f, 0.f);
        if (controlScheme == 1) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) movement.y -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement.y += 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) movement.x -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement.x += 1.f;
        } else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    movement.y -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  movement.y += 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  movement.x -= 1.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) movement.x += 1.f;
        }

        if (movement.x != 0.f || movement.y != 0.f) {
            float len = std::sqrt(movement.x * movement.x + movement.y * movement.y);
            movement /= len;
        }
        position += movement * speed * dt;
    }

    void clampToArea(sf::Vector2f minCorner, sf::Vector2f maxCorner) {
        position.x = std::max(minCorner.x, std::min(maxCorner.x, position.x));
        position.y = std::max(minCorner.y, std::min(maxCorner.y, position.y));
    }

    void draw(sf::RenderWindow& window) {
        if (!isAlive) return;
        const sf::Vector2f feet = position;
        drawBlock(window, feet + sf::Vector2f(0.f, -12.f), sf::Vector2f(30.f, 22.f),
                  legsColor, shadeColor(legsColor, 0.7f), shadeColor(legsColor, 0.85f));
        
        // Brazos (dibujados antes del torso para que se vean a los lados)
        drawBlock(window, feet + sf::Vector2f(-26.f, -34.f), sf::Vector2f(14.f, 26.f),
                  torsoColor, shadeColor(torsoColor, 0.65f), shadeColor(torsoColor, 0.82f));
        drawBlock(window, feet + sf::Vector2f(26.f, -34.f), sf::Vector2f(14.f, 26.f),
                  torsoColor, shadeColor(torsoColor, 0.65f), shadeColor(torsoColor, 0.82f));
        
        drawBlock(window, feet + sf::Vector2f(0.f, -34.f), sf::Vector2f(34.f, 28.f),
                  torsoColor, shadeColor(torsoColor, 0.65f), shadeColor(torsoColor, 0.82f));
        
        drawBlock(window, feet + sf::Vector2f(0.f, -58.f), sf::Vector2f(26.f, 24.f),
                  headColor, shadeColor(headColor, 0.62f), shadeColor(headColor, 0.78f));

        // Cara simple (ojos y boca)
        sf::Vector2f headCenter = feet + sf::Vector2f(0.f, -58.f);
        sf::CircleShape leftEye(4.f);
        leftEye.setFillColor(sf::Color::Black);
        leftEye.setPosition(sf::Vector2f(headCenter.x - 7.f, headCenter.y - 5.f));
        window.draw(leftEye);

        sf::CircleShape rightEye(4.f);
        rightEye.setFillColor(sf::Color::Black);
        rightEye.setPosition(sf::Vector2f(headCenter.x + 7.f, headCenter.y - 5.f));
        window.draw(rightEye);

        sf::RectangleShape mouth(sf::Vector2f(12.f, 3.f));
        mouth.setFillColor(sf::Color::Black);
        mouth.setPosition(sf::Vector2f(headCenter.x - 6.f, headCenter.y + 5.f));
        window.draw(mouth);

        nameTag.setPosition(sf::Vector2f(feet.x - 18.f, feet.y - 82.f));
        window.draw(nameTag);
    }

    sf::Vector2f getPosition() const { return position; }
    void setPosition(sf::Vector2f pos) { position = pos; }
    float getDrawDepth() const { return position.y; }
    void setAlive(bool alive) { isAlive = alive; }
    bool getAlive() const { return isAlive; }
};
