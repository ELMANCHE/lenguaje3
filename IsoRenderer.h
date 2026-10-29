#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>

namespace IsoRenderer {

    inline sf::Color tileColorForIndex(int index) {
        switch (index) {
            case 0: return sf::Color(255, 0, 255);
            case 1: return sf::Color(0, 255, 255);
            default: return sf::Color(255, 255, 0);
        }
    }

    inline const char* tileLabelForIndex(int index) {
        switch (index) {
            case 0: return "MAGENTA";
            case 1: return "CYAN";
            default: return "AMARILLO";
        }
    }

    inline sf::FloatRect tileHitbox(sf::Vector2f center, float tileW) {
        float hw = tileW * 0.55f;
        float hh = tileW * 0.32f;
        return sf::FloatRect(
            sf::Vector2f(center.x - hw, center.y - hh),
            sf::Vector2f(hw * 2.f, hh * 2.f)
        );
    }

    inline void drawTexturedQuad(sf::RenderWindow& window, const sf::Texture& tex,
                                 sf::Vector2f p0, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3) {
        sf::VertexArray quad(sf::PrimitiveType::TriangleFan, 4);
        auto ts = tex.getSize();
        quad[0].position = p0;
        quad[0].color = sf::Color::White;
        quad[0].texCoords = sf::Vector2f(0.f, 0.f);
        quad[1].position = p1;
        quad[1].color = sf::Color::White;
        quad[1].texCoords = sf::Vector2f(static_cast<float>(ts.x), 0.f);
        quad[2].position = p2;
        quad[2].color = sf::Color::White;
        quad[2].texCoords = sf::Vector2f(static_cast<float>(ts.x), static_cast<float>(ts.y));
        quad[3].position = p3;
        quad[3].color = sf::Color::White;
        quad[3].texCoords = sf::Vector2f(0.f, static_cast<float>(ts.y));
        window.draw(quad, &tex);
    }

    inline void drawDiorama(sf::RenderWindow& window, const sf::Texture& pisoTex, const sf::Texture& paredTex) {
        const sf::Vector2f apex(640.f, 320.f);
        const sf::Vector2f right(960.f, 500.f);
        const sf::Vector2f bottom(640.f, 680.f);
        const sf::Vector2f left(320.f, 500.f);
        const float wallHeight = 175.f;

        if (paredTex.getSize().x > 0) {
            // Pared izquierda: borde apex -> left, extendida hacia arriba
            drawTexturedQuad(window, paredTex,
                left + sf::Vector2f(-35.f, -wallHeight),
                apex + sf::Vector2f(0.f, -wallHeight),
                apex,
                left);

            // Pared derecha: borde apex -> right, extendida hacia arriba
            drawTexturedQuad(window, paredTex,
                apex + sf::Vector2f(0.f, -wallHeight),
                right + sf::Vector2f(35.f, -wallHeight),
                right,
                apex);
        }

        if (pisoTex.getSize().x > 0) {
            drawTexturedQuad(window, pisoTex, apex, right, bottom, left);
        } else {
            sf::ConvexShape floor;
            floor.setPointCount(4);
            floor.setPoint(0, apex);
            floor.setPoint(1, right);
            floor.setPoint(2, bottom);
            floor.setPoint(3, left);
            floor.setFillColor(sf::Color(120, 100, 70));
            window.draw(floor);
        }

        sf::ConvexShape outline;
        outline.setPointCount(4);
        outline.setPoint(0, apex);
        outline.setPoint(1, right);
        outline.setPoint(2, bottom);
        outline.setPoint(3, left);
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineThickness(2.f);
        outline.setOutlineColor(sf::Color(0, 255, 255, 120));
        window.draw(outline);
    }

    inline void drawNeonTile(sf::RenderWindow& window, sf::Vector2f center, float tileW, sf::Color neon) {
        const float hw = tileW * 0.5f;
        const float hh = tileW * 0.28f;
        const float depth = tileW * 0.14f;

        sf::Vector2f top(center.x, center.y - hh);
        sf::Vector2f right(center.x + hw, center.y);
        sf::Vector2f bottom(center.x, center.y + hh);
        sf::Vector2f left(center.x - hw, center.y);
        sf::Vector2f depthOff(depth * 0.7f, depth);

        sf::Color sideDark(neon.r / 3, neon.g / 3, neon.b / 3, 200);
        sf::Color sideMid(neon.r / 2, neon.g / 2, neon.b / 2, 220);

        sf::ConvexShape leftFace;
        leftFace.setPointCount(4);
        leftFace.setPoint(0, left);
        leftFace.setPoint(1, bottom);
        leftFace.setPoint(2, bottom + depthOff);
        leftFace.setPoint(3, left + depthOff);
        leftFace.setFillColor(sideDark);
        window.draw(leftFace);

        sf::ConvexShape rightFace;
        rightFace.setPointCount(4);
        rightFace.setPoint(0, right);
        rightFace.setPoint(1, bottom);
        rightFace.setPoint(2, bottom + depthOff);
        rightFace.setPoint(3, right + depthOff);
        rightFace.setFillColor(sideMid);
        window.draw(rightFace);

        sf::ConvexShape topFace;
        topFace.setPointCount(4);
        topFace.setPoint(0, top);
        topFace.setPoint(1, right);
        topFace.setPoint(2, bottom);
        topFace.setPoint(3, left);
        topFace.setFillColor(sf::Color(15, 15, 20, 210));
        topFace.setOutlineThickness(3.f);
        topFace.setOutlineColor(neon);
        window.draw(topFace);

        sf::ConvexShape glow;
        glow.setPointCount(4);
        glow.setPoint(0, top);
        glow.setPoint(1, right);
        glow.setPoint(2, bottom);
        glow.setPoint(3, left);
        glow.setFillColor(sf::Color(neon.r, neon.g, neon.b, 25));
        glow.setOutlineThickness(6.f);
        glow.setOutlineColor(sf::Color(neon.r, neon.g, neon.b, 60));
        window.draw(glow);
    }

    inline sf::Vector2f bridgeCell(float col, float row, sf::Vector2f origin, float tileSize) {
        const float halfW = tileSize * 0.52f;
        const float halfH = tileSize * 0.30f;
        return origin + sf::Vector2f((col - row) * halfW, (col + row) * halfH);
    }

    inline sf::Vector2f bridgeOriginForCenter(sf::Vector2f center, float tileSize, int rows = 6) {
        const float halfW = tileSize * 0.52f;
        const float halfH = tileSize * 0.30f;
        float sumX = 0.f;
        float sumY = 0.f;
        int count = 0;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < 2; ++col) {
                sumX += (static_cast<float>(col) - static_cast<float>(row)) * halfW;
                sumY += (static_cast<float>(col) + static_cast<float>(row)) * halfH;
                ++count;
            }
        }
        return center - sf::Vector2f(sumX / count, sumY / count);
    }

    inline sf::FloatRect bridgeBounds(sf::Vector2f origin, float tileSize, int rows = 6) {
        sf::Vector2f minP(100000.f, 100000.f);
        sf::Vector2f maxP(-100000.f, -100000.f);
        const float pad = tileSize * 0.55f;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < 2; ++col) {
                sf::Vector2f c = bridgeCell(static_cast<float>(col), static_cast<float>(row), origin, tileSize);
                minP.x = std::min(minP.x, c.x - pad);
                minP.y = std::min(minP.y, c.y - pad);
                maxP.x = std::max(maxP.x, c.x + pad);
                maxP.y = std::max(maxP.y, c.y + pad);
            }
        }
        return sf::FloatRect(minP, maxP - minP);
    }

    inline void drawGlassStairTile(sf::RenderWindow& window, sf::Vector2f center, float tileSize,
                                   bool highlight, sf::Color neonColor) {
        const float hw = tileSize * 0.46f;
        const float hh = tileSize * 0.26f;
        const float depth = tileSize * 0.18f;
        const sf::Vector2f depthOff(depth * 0.75f, depth);

        sf::Vector2f top(center.x, center.y - hh);
        sf::Vector2f right(center.x + hw, center.y);
        sf::Vector2f bottom(center.x, center.y + hh);
        sf::Vector2f left(center.x - hw, center.y);

        sf::ConvexShape leftFace;
        leftFace.setPointCount(4);
        leftFace.setPoint(0, left);
        leftFace.setPoint(1, bottom);
        leftFace.setPoint(2, bottom + depthOff);
        leftFace.setPoint(3, left + depthOff);
        leftFace.setFillColor(sf::Color(210, 210, 215));
        window.draw(leftFace);

        sf::ConvexShape rightFace;
        rightFace.setPointCount(4);
        rightFace.setPoint(0, right);
        rightFace.setPoint(1, bottom);
        rightFace.setPoint(2, bottom + depthOff);
        rightFace.setPoint(3, right + depthOff);
        rightFace.setFillColor(sf::Color(235, 235, 240));
        window.draw(rightFace);

        sf::ConvexShape topFace;
        topFace.setPointCount(4);
        topFace.setPoint(0, top);
        topFace.setPoint(1, right);
        topFace.setPoint(2, bottom);
        topFace.setPoint(3, left);
        topFace.setFillColor(sf::Color(252, 252, 255));
        topFace.setOutlineThickness(highlight ? 3.f : 2.5f);
        topFace.setOutlineColor(highlight ? neonColor : sf::Color(10, 10, 10));
        window.draw(topFace);
    }

    inline void drawBinaryBridge(sf::RenderWindow& window, sf::Vector2f origin, float tileSize,
                                   int rows, const bool visible[4][2], int activeRow) {
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < 2; ++col) {
                if (!visible[row][col]) continue;
                sf::Vector2f center = bridgeCell(static_cast<float>(col), static_cast<float>(row), origin, tileSize);
                bool active = (row == activeRow);
                sf::Color border = active ? sf::Color(0, 255, 255) : sf::Color(10, 10, 10);
                drawGlassStairTile(window, center, tileSize, active, border);
            }
        }
    }
}
