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
        const std::string& boardPath,          // wood.png
        const std::string& vChainPath,         // chain_left.png，V字鏈
        const std::string& ropeLinkPath,       // chain.png，一小節直鏈
        const std::string& wheelPath,          // wheel.png
        const glm::vec2& centerPos,            // 整組中心 X
        float sideOffsetX,                     // 左右平台離中心距離
        float baseBoardY,                      // 平衡時木板 Y
        float topWheelY,                       // 輪子 Y
        float boardWidth,                      // 單塊木板碰撞寬
        float boardHeight,                     // 單塊木板碰撞高
        float floorY,                          // 重的一邊下降到碰到這個地板 Y
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
    // ===== 左右木板 =====
    std::shared_ptr<Util::GameObject> m_LeftBoard;
    std::shared_ptr<Util::GameObject> m_RightBoard;

    // ===== 木板上的 V 字鏈 =====
    std::shared_ptr<Util::GameObject> m_LeftVChain;
    std::shared_ptr<Util::GameObject> m_RightVChain;

    // ===== 一條鏈：用很多小節 chain.png 排出來 =====
    std::vector<std::shared_ptr<Util::GameObject>> m_RopeLinks;

    // ===== 兩個輪子 =====
    std::shared_ptr<Util::GameObject> m_LeftWheel;
    std::shared_ptr<Util::GameObject> m_RightWheel;

private:
    glm::vec2 m_CenterPos;

    float m_SideOffsetX = 0.0f;
    float m_BaseBoardY = 0.0f;
    float m_TopWheelY = 0.0f;
    float m_BoardWidth = 0.0f;
    float m_BoardHeight = 0.0f;
    float m_FloorY = 0.0f;

    glm::vec2 m_BoardScale;
    glm::vec2 m_VChainScale;
    glm::vec2 m_RopeLinkScale;
    glm::vec2 m_WheelScale;

private:
    // ===== 重量 =====
    float m_LeftWeight = 0.0f;
    float m_RightWeight = 0.0f;

    // ===== 木板目前 Y =====
    float m_LeftBoardY = 0.0f;
    float m_RightBoardY = 0.0f;

    float m_LeftHookY = 0.0f;
    float m_RightHookY = 0.0f;

    // ===== 同一條鏈的位移 =====
    // 正數：左邊下降，右邊上升
    // 負數：右邊下降，左邊上升
    float m_RopeOffset = 0.0f;
    float m_TargetRopeOffset = 0.0f;
    float m_MaxRopeOffset = 0.0f;

    // 讓 chain.png 看起來在滑動
    float m_RopeScroll = 0.0f;
    float m_LastRopeOffset = 0.0f;

private:
    // ===== 可調參數 =====
    float m_MoveSpeed = 2.0f;
    float m_VBoardGap = 0.0f;          // V鏈底部跟木板的距離
    float m_VApexInset = 2.0f;         // V鏈頂端往下吃一點，避免露縫
    float m_RopeEndpointOverlap = 5.0f; // 直鏈往V鏈裡面多插一點
    // V 字鏈位置
    float m_VChainYOffset = 32.0f;
    float m_VChainMeetYOffset = 65.0f;

    // chain link 間距，太大會斷，太小會很密
    float m_RopeLinkSpacing = 6.0f;
};

#endif