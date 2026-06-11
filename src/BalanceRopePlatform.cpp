#include "BalanceRopePlatform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

BalanceRopePlatform::BalanceRopePlatform(
    const std::string& boardPath,
    const std::string& vChainPath,
    const std::string& ropeLinkPath,
    const std::string& wheelPath,
    const glm::vec2& centerPos,
    float sideOffsetX,
    float baseBoardY,
    float topWheelY,
    float boardWidth,
    float boardHeight,
    float leftFloorY,
    float rightFloorY,
    float leftInitialY,
    float rightInitialY,
    const glm::vec2& boardScale,
    const glm::vec2& vChainScale,
    const glm::vec2& ropeLinkScale,
    const glm::vec2& wheelScale
)
    : m_CenterPos(centerPos),
      m_SideOffsetX(sideOffsetX),
      m_BaseBoardY(baseBoardY),
      m_TopWheelY(topWheelY),
      m_BoardWidth(boardWidth),
      m_BoardHeight(boardHeight),
      m_LeftFloorY(leftFloorY),
      m_RightFloorY(rightFloorY),
      m_BoardScale(boardScale),
      m_VChainScale(vChainScale),
      m_RopeLinkScale(ropeLinkScale),
      m_WheelScale(wheelScale)
{
    m_LeftBoard = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(boardPath),
        10.0f
    );

    m_RightBoard = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(boardPath),
        10.0f
    );

    m_LeftVChain = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(vChainPath),
        9.0f
    );

    m_RightVChain = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(vChainPath),
        9.0f
    );

    m_LeftWheel = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(wheelPath),
        11.0f
    );

    m_RightWheel = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(wheelPath),
        11.0f
    );

    m_LeftBoard->m_Transform.scale = m_BoardScale;
    m_RightBoard->m_Transform.scale = m_BoardScale;

    m_LeftVChain->m_Transform.scale = m_VChainScale;
    m_RightVChain->m_Transform.scale = m_VChainScale;

    m_LeftWheel->m_Transform.scale = m_WheelScale;
    m_RightWheel->m_Transform.scale = m_WheelScale;

    /*
        leftFloorY / rightFloorY：
            只代表左右最低高度

        leftInitialY / rightInitialY：
            代表左右一開始的位置

        公式：
            leftY  = baseY - offset
            rightY = baseY + offset

        反推：
            baseY  = (leftInitialY + rightInitialY) / 2
            offset = (rightInitialY - leftInitialY) / 2
    */

    float visualHalfBoardH = m_LeftBoard->GetScaledSize().y / 2.0f;
    float floorPadding = 4.0f;

    // 地板表面 Y 轉成木板中心最低 Y
    float leftDownCenterY = m_LeftFloorY + visualHalfBoardH + floorPadding;
    float rightDownCenterY = m_RightFloorY + visualHalfBoardH + floorPadding;

    // 初始高度先各自限制，不讓木板一開始低於地板
    leftInitialY = std::max(leftInitialY, leftDownCenterY);
    rightInitialY = std::max(rightInitialY, rightDownCenterY);

    // 用修正後的左右初始高度反推 base 和 offset
    m_BaseBoardY = (leftInitialY + rightInitialY) / 2.0f;
    m_RopeOffset = (rightInitialY - leftInitialY) / 2.0f;

    // 左右最多下降量
    m_LeftMaxRopeOffset = std::max(0.0f, m_BaseBoardY - leftDownCenterY);
    m_RightMaxRopeOffset = std::max(0.0f, m_BaseBoardY - rightDownCenterY);

    m_TargetRopeOffset = m_RopeOffset;
    m_LastRopeOffset = m_RopeOffset;
    m_RopeScroll = 0.0f;

    m_LeftBoardY = m_BaseBoardY - m_RopeOffset;
    m_RightBoardY = m_BaseBoardY + m_RopeOffset;

    CreateRopeLinks(ropeLinkPath);
    UpdateObjects();
}

void BalanceRopePlatform::BeginFrame() {
    m_LeftWeight = 0.0f;
    m_RightWeight = 0.0f;
}

void BalanceRopePlatform::AddWeight(bool isLeftBoard, float weight) {
    if (isLeftBoard) {
        m_LeftWeight += weight;
    } else {
        m_RightWeight += weight;
    }
}

bool BalanceRopePlatform::ResolveOnBoard(
    float boardCenterX,
    float boardCenterY,
    glm::vec2& objectPos,
    const glm::vec2& objectSize,
    float& velocityY,
    float weight,
    bool isLeftBoard
) {
    float halfObjW = objectSize.x / 2.0f;
    float halfObjH = objectSize.y / 2.0f;

    float halfBoardW = (m_BoardWidth * m_BoardScale.x) / 2.0f;
    float halfBoardH = (m_BoardHeight * m_BoardScale.y) / 2.0f;

    float objLeft = objectPos.x - halfObjW;
    float objRight = objectPos.x + halfObjW;
    float objBottom = objectPos.y - halfObjH;

    float boardLeft = boardCenterX - halfBoardW;
    float boardRight = boardCenterX + halfBoardW;
    float boardTop = boardCenterY + halfBoardH;

    bool overlapX = objRight > boardLeft && objLeft < boardRight;
    bool fallingOrStanding = velocityY <= 0.0f;

    bool nearTop =
        objBottom <= boardTop + 8.0f &&
        objBottom >= boardTop - 24.0f;

    if (overlapX && fallingOrStanding && nearTop) {
        objectPos.y = boardTop + halfObjH;
        velocityY = 0.0f;

        AddWeight(isLeftBoard, weight);

        return true;
    }

    return false;
}

bool BalanceRopePlatform::CheckCollisionWithObject(
    glm::vec2& objectPos,
    const glm::vec2& objectSize,
    float& velocityY,
    float weight
) {
    float leftBoardX = m_CenterPos.x - m_SideOffsetX;
    float rightBoardX = m_CenterPos.x + m_SideOffsetX;

    if (ResolveOnBoard(
        leftBoardX,
        m_LeftBoardY,
        objectPos,
        objectSize,
        velocityY,
        weight,
        true
    )) {
        return true;
    }

    if (ResolveOnBoard(
        rightBoardX,
        m_RightBoardY,
        objectPos,
        objectSize,
        velocityY,
        weight,
        false
    )) {
        return true;
    }

    return false;
}

void BalanceRopePlatform::Update(float dt) {
    const float EPS = 0.1f;

    bool hasLeftWeight = m_LeftWeight > EPS;
    bool hasRightWeight = m_RightWeight > EPS;

    if (m_LeftWeight > m_RightWeight + EPS) {
        // 左邊比較重：左板下降、右板上升
        m_TargetRopeOffset = m_LeftMaxRopeOffset;
    }
    else if (m_RightWeight > m_LeftWeight + EPS) {
        // 右邊比較重：右板下降、左板上升
        m_TargetRopeOffset = -m_RightMaxRopeOffset;
    }
    else {
        // 不回彈：重量相同時停住
        if (hasLeftWeight || hasRightWeight) {
            m_TargetRopeOffset = m_RopeOffset;
        }
    }

    m_TargetRopeOffset = std::clamp(
        m_TargetRopeOffset,
        -m_RightMaxRopeOffset,
        m_LeftMaxRopeOffset
    );

    float diff = m_TargetRopeOffset - m_RopeOffset;
    float step = m_MoveSpeed * dt * 60.0f;

    if (std::abs(diff) <= step) {
        m_RopeOffset = m_TargetRopeOffset;
    } else {
        if (diff > 0.0f) {
            m_RopeOffset += step;
        } else {
            m_RopeOffset -= step;
        }
    }

    m_RopeOffset = std::clamp(
        m_RopeOffset,
        -m_RightMaxRopeOffset,
        m_LeftMaxRopeOffset
    );

    if (std::abs(m_TargetRopeOffset - m_RopeOffset) < 0.05f) {
        m_RopeOffset = m_TargetRopeOffset;
    }

    float deltaOffset = m_RopeOffset - m_LastRopeOffset;
    m_RopeScroll += deltaOffset;
    m_LastRopeOffset = m_RopeOffset;

    m_LeftBoardY = m_BaseBoardY - m_RopeOffset;
    m_RightBoardY = m_BaseBoardY + m_RopeOffset;

    // 保險：到底就停住，避免視覺穿地板
    float visualHalfBoardH = m_LeftBoard->GetScaledSize().y / 2.0f;
    float floorPadding = 4.0f;

    float leftMinCenterY = m_LeftFloorY + visualHalfBoardH + floorPadding;
    float rightMinCenterY = m_RightFloorY + visualHalfBoardH + floorPadding;

    if (m_LeftBoardY < leftMinCenterY) {
        m_LeftBoardY = leftMinCenterY;
        m_RopeOffset = m_BaseBoardY - m_LeftBoardY;
        m_TargetRopeOffset = m_RopeOffset;
    }

    if (m_RightBoardY < rightMinCenterY) {
        m_RightBoardY = rightMinCenterY;
        m_RopeOffset = m_RightBoardY - m_BaseBoardY;
        m_TargetRopeOffset = m_RopeOffset;
    }

    UpdateObjects();
}

void BalanceRopePlatform::UpdateObjects() {
    float leftBoardX = m_CenterPos.x - m_SideOffsetX;
    float rightBoardX = m_CenterPos.x + m_SideOffsetX;

    float halfBoardH = (m_BoardHeight * m_BoardScale.y) / 2.0f;

    m_LeftBoard->m_Transform.translation = {
        leftBoardX,
        m_LeftBoardY
    };

    m_RightBoard->m_Transform.translation = {
        rightBoardX,
        m_RightBoardY
    };

    m_LeftBoard->m_Transform.rotation = 0.0f;
    m_RightBoard->m_Transform.rotation = 0.0f;

    m_LeftWheel->m_Transform.translation = {
        leftBoardX,
        m_TopWheelY
    };

    m_RightWheel->m_Transform.translation = {
        rightBoardX,
        m_TopWheelY
    };

    m_LeftWheel->m_Transform.rotation = glm::radians(m_RopeScroll * 6.0f);
    m_RightWheel->m_Transform.rotation = glm::radians(m_RopeScroll * 6.0f);

    float leftBoardTopY = m_LeftBoardY + halfBoardH;
    float rightBoardTopY = m_RightBoardY + halfBoardH;

    float leftVHalfH = m_LeftVChain->GetScaledSize().y / 2.0f;
    float rightVHalfH = m_RightVChain->GetScaledSize().y / 2.0f;

    float leftVCenterY = leftBoardTopY + leftVHalfH + m_VBoardGap;
    float rightVCenterY = rightBoardTopY + rightVHalfH + m_VBoardGap;

    m_LeftVChain->m_Transform.translation = {
        leftBoardX,
        leftVCenterY
    };

    m_RightVChain->m_Transform.translation = {
        rightBoardX,
        rightVCenterY
    };

    m_LeftVChain->m_Transform.rotation = 0.0f;
    m_RightVChain->m_Transform.rotation = 0.0f;

    m_LeftHookY = leftVCenterY + leftVHalfH - m_VApexInset;
    m_RightHookY = rightVCenterY + rightVHalfH - m_VApexInset;

    UpdateRopeLinks();
}

void BalanceRopePlatform::CreateRopeLinks(const std::string& ropeLinkPath) {
    m_RopeLinks.clear();

    float totalLen = GetRopeLength();
    int count = static_cast<int>(totalLen / 4.0f) + 120;

    for (int i = 0; i < count; i++) {
        auto link = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(ropeLinkPath),
            8.5f
        );

        link->m_Transform.scale = m_RopeLinkScale;
        link->m_Transform.rotation = 0.0f;

        m_RopeLinks.push_back(link);
    }
}

float BalanceRopePlatform::GetRopeLength() const {
    float leftX = m_CenterPos.x - m_SideOffsetX;
    float rightX = m_CenterPos.x + m_SideOffsetX;

    float leftVerticalLen = std::abs(m_TopWheelY - m_LeftHookY);
    float topLen = std::abs(rightX - leftX);
    float rightVerticalLen = std::abs(m_TopWheelY - m_RightHookY);

    return leftVerticalLen + topLen + rightVerticalLen;
}

glm::vec2 BalanceRopePlatform::GetRopePoint(float distance) const {
    float leftX = m_CenterPos.x - m_SideOffsetX;
    float rightX = m_CenterPos.x + m_SideOffsetX;

    float leftVerticalLen = std::abs(m_TopWheelY - m_LeftHookY);
    float topLen = std::abs(rightX - leftX);
    float rightVerticalLen = std::abs(m_TopWheelY - m_RightHookY);

    float totalLen = leftVerticalLen + topLen + rightVerticalLen;

    if (totalLen <= 1.0f) {
        return { leftX, m_TopWheelY };
    }

    while (distance < 0.0f) {
        distance += totalLen;
    }

    while (distance >= totalLen) {
        distance -= totalLen;
    }

    if (distance < leftVerticalLen) {
        float t = distance / leftVerticalLen;
        float y = m_LeftHookY + (m_TopWheelY - m_LeftHookY) * t;
        return { leftX, y };
    }

    distance -= leftVerticalLen;

    if (distance < topLen) {
        float t = distance / topLen;
        float x = leftX + (rightX - leftX) * t;
        return { x, m_TopWheelY };
    }

    distance -= topLen;

    if (rightVerticalLen <= 1.0f) {
        return { rightX, m_TopWheelY };
    }

    float t = distance / rightVerticalLen;
    float y = m_TopWheelY + (m_RightHookY - m_TopWheelY) * t;

    return { rightX, y };
}

void BalanceRopePlatform::UpdateRopeLinks() {
    float leftX = m_CenterPos.x - m_SideOffsetX;
    float rightX = m_CenterPos.x + m_SideOffsetX;

    float leftBottomY = m_LeftHookY - m_RopeEndpointOverlap;
    float leftTopY = m_TopWheelY;

    float rightTopY = m_TopWheelY;
    float rightBottomY = m_RightHookY - m_RopeEndpointOverlap;

    int index = 0;

    auto placeLink = [&](const glm::vec2& pos, float rotation) {
        if (index >= static_cast<int>(m_RopeLinks.size())) {
            return;
        }

        m_RopeLinks[index]->m_Transform.translation = pos;
        m_RopeLinks[index]->m_Transform.rotation = rotation;
        m_RopeLinks[index]->m_Transform.scale = m_RopeLinkScale;
        m_RopeLinks[index]->SetVisible(true);

        index++;
    };

    auto getPhase = [](float scroll, float spacing) {
        float phase = std::fmod(scroll, spacing);
        if (phase < 0.0f) {
            phase += spacing;
        }
        return phase;
    };

    auto placeVertical = [&](float x, float y1, float y2, float scrollSign) {
        float len = std::abs(y2 - y1);
        if (len <= 1.0f) return;

        float spacing = 5.0f;
        float phase = getPhase(m_RopeScroll * scrollSign, spacing);
        float dir = (y2 > y1) ? 1.0f : -1.0f;

        for (float d = -phase; d <= len + spacing; d += spacing) {
            float y = y1 + dir * d;

            if ((dir > 0.0f && (y < std::min(y1, y2) || y > std::max(y1, y2))) ||
                (dir < 0.0f && (y < std::min(y1, y2) || y > std::max(y1, y2)))) {
                continue;
            }

            placeLink(glm::vec2(x, y), 0.0f);
        }
    };

    auto placeHorizontal = [&](float x1, float x2, float y, float scrollSign) {
        float len = std::abs(x2 - x1);
        if (len <= 1.0f) return;

        float spacing = 4.0f;
        float phase = getPhase(m_RopeScroll * scrollSign, spacing);
        float dir = (x2 > x1) ? 1.0f : -1.0f;

        for (float d = -phase; d <= len + spacing; d += spacing) {
            float x = x1 + dir * d;

            if (x < std::min(x1, x2) || x > std::max(x1, x2)) {
                continue;
            }

            placeLink(glm::vec2(x, y), glm::radians(90.0f));
        }
    };

    placeVertical(leftX, leftBottomY, leftTopY, 1.0f);
    placeHorizontal(leftX, rightX, m_TopWheelY, 1.0f);
    placeVertical(rightX, rightTopY, rightBottomY, -1.0f);

    for (int i = index; i < static_cast<int>(m_RopeLinks.size()); i++) {
        m_RopeLinks[i]->SetVisible(false);
    }
}

std::vector<std::shared_ptr<Util::GameObject>> BalanceRopePlatform::GetAllObjects() const {
    std::vector<std::shared_ptr<Util::GameObject>> objects = {
        m_LeftBoard,
        m_RightBoard,
        m_LeftVChain,
        m_RightVChain
    };

    for (auto& link : m_RopeLinks) {
        objects.push_back(link);
    }

    objects.push_back(m_LeftWheel);
    objects.push_back(m_RightWheel);

    return objects;
}