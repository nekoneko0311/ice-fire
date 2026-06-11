#ifndef BALANCE_ROPE_PLATFORM_HPP
#define BALANCE_ROPE_PLATFORM_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class BalanceRopePlatform {
public:
    BalanceRopePlatform(
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

        float leftFloorY,       // 左邊最低高度：地板表面 Y
        float rightFloorY,      // 右邊最低高度：地板表面 Y

        float leftInitialY,     // 左邊初始木板中心 Y
        float rightInitialY,    // 右邊初始木板中心 Y

        const glm::vec2& boardScale,
        const glm::vec2& vChainScale,
        const glm::vec2& ropeLinkScale,
        const glm::vec2& wheelScale
    );

    void BeginFrame();
    void Update(float dt);

    bool CheckCollisionWithObject(
        glm::vec2& objectPos,
        const glm::vec2& objectSize,
        float& velocityY,
        float weight
    );

    std::vector<std::shared_ptr<Util::GameObject>> GetAllObjects() const;

private:
    void AddWeight(bool isLeftBoard, float weight);

    bool ResolveOnBoard(
        float boardCenterX,
        float boardCenterY,
        glm::vec2& objectPos,
        const glm::vec2& objectSize,
        float& velocityY,
        float weight,
        bool isLeftBoard
    );

    void UpdateObjects();

    void CreateRopeLinks(const std::string& ropeLinkPath);
    void UpdateRopeLinks();

    float GetRopeLength() const;
    glm::vec2 GetRopePoint(float distance) const;

private:
    std::shared_ptr<Util::GameObject> m_LeftBoard;
    std::shared_ptr<Util::GameObject> m_RightBoard;

    std::shared_ptr<Util::GameObject> m_LeftVChain;
    std::shared_ptr<Util::GameObject> m_RightVChain;

    std::vector<std::shared_ptr<Util::GameObject>> m_RopeLinks;

    std::shared_ptr<Util::GameObject> m_LeftWheel;
    std::shared_ptr<Util::GameObject> m_RightWheel;

private:
    glm::vec2 m_CenterPos;

    float m_SideOffsetX = 0.0f;
    float m_BaseBoardY = 0.0f;
    float m_TopWheelY = 0.0f;
    float m_BoardWidth = 0.0f;
    float m_BoardHeight = 0.0f;

    float m_LeftFloorY = 0.0f;
    float m_RightFloorY = 0.0f;

    glm::vec2 m_BoardScale;
    glm::vec2 m_VChainScale;
    glm::vec2 m_RopeLinkScale;
    glm::vec2 m_WheelScale;

private:
    float m_LeftWeight = 0.0f;
    float m_RightWeight = 0.0f;

    float m_LeftBoardY = 0.0f;
    float m_RightBoardY = 0.0f;

    float m_LeftHookY = 0.0f;
    float m_RightHookY = 0.0f;

    // 正數：左邊下降，右邊上升
    // 負數：右邊下降，左邊上升
    float m_RopeOffset = 0.0f;
    float m_TargetRopeOffset = 0.0f;

    float m_LeftMaxRopeOffset = 0.0f;
    float m_RightMaxRopeOffset = 0.0f;

    float m_RopeScroll = 0.0f;
    float m_LastRopeOffset = 0.0f;

private:
    float m_MoveSpeed = 1.5f;

    float m_VBoardGap = 0.0f;
    float m_VApexInset = 2.0f;
    float m_RopeEndpointOverlap = 5.0f;

    float m_VChainYOffset = 32.0f;
    float m_VChainMeetYOffset = 65.0f;

    float m_RopeLinkSpacing = 6.0f;
};

#endif