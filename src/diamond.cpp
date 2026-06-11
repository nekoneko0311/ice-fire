#include "App.hpp"
#include <cmath>

void App::InitDiamonds(
    const std::vector<glm::vec2>& redPositions,
    const std::vector<glm::vec2>& bluePositions
) {
    // 先清掉舊紅寶石
    for (auto& diamond : m_RedDiamonds) {
        if (diamond) {
            m_Root->RemoveChild(diamond);
        }
    }

    // 先清掉舊藍寶石
    for (auto& diamond : m_BlueDiamonds) {
        if (diamond) {
            m_Root->RemoveChild(diamond);
        }
    }

    m_RedDiamonds.clear();
    m_BlueDiamonds.clear();

    m_RedDiamondBasePos.clear();
    m_BlueDiamondBasePos.clear();

    m_RedDiamondCollected.clear();
    m_BlueDiamondCollected.clear();

    // ===== 建立紅寶石 =====
    for (const auto& pos : redPositions) {
        auto diamond = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(PIC_PATH + "r_diamond.png"),
            -1.0f
        );

        diamond->m_Transform.translation = pos;
        diamond->m_Transform.scale = {0.6f, 0.6f};

        m_RedDiamonds.push_back(diamond);
        m_RedDiamondBasePos.push_back(pos);
        m_RedDiamondCollected.push_back(false);

        m_Root->AddChild(diamond);
    }

    // ===== 建立藍寶石 =====
    for (const auto& pos : bluePositions) {
        auto diamond = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(PIC_PATH + "b_diamond.png"),
            -1.0f
        );

        diamond->m_Transform.translation = pos;
        diamond->m_Transform.scale = {0.6f, 0.6f};

        m_BlueDiamonds.push_back(diamond);
        m_BlueDiamondBasePos.push_back(pos);
        m_BlueDiamondCollected.push_back(false);

        m_Root->AddChild(diamond);
    }
}

void App::UpdateDiamonds() {
    m_DiamondFloatTime += m_DiamondFloatSpeed;
    float offsetY = std::sin(m_DiamondFloatTime) * m_DiamondFloatRange;

    for (int i = 0; i < m_RedDiamonds.size(); i++) {
        if (m_RedDiamonds[i] && !m_RedDiamondCollected[i]) {
            m_RedDiamonds[i]->m_Transform.translation.y =
                m_RedDiamondBasePos[i].y + offsetY;
        }
    }

    for (int i = 0; i < m_BlueDiamonds.size(); i++) {
        if (m_BlueDiamonds[i] && !m_BlueDiamondCollected[i]) {
            m_BlueDiamonds[i]->m_Transform.translation.y =
                m_BlueDiamondBasePos[i].y + offsetY;
        }
    }
}

void App::CheckDiamondCollection() {
    // Fire 吃紅寶石
    for (int i = 0; i < m_RedDiamonds.size(); i++) {
        if (m_RedDiamonds[i] && !m_RedDiamondCollected[i]) {
            if (IsColliding(m_Fire, m_RedDiamonds[i])) {
                m_RedDiamondCollected[i] = true;
                m_Root->RemoveChild(m_RedDiamonds[i]);
                m_RedDiamonds[i].reset();
                m_Score += 1;
            }
        }
    }

    // Ice 吃藍寶石
    for (int i = 0; i < m_BlueDiamonds.size(); i++) {
        if (m_BlueDiamonds[i] && !m_BlueDiamondCollected[i]) {
            if (IsColliding(m_Ice, m_BlueDiamonds[i])) {
                m_BlueDiamondCollected[i] = true;
                m_Root->RemoveChild(m_BlueDiamonds[i]);
                m_BlueDiamonds[i].reset();
                m_Score += 1;
            }
        }
    }
}