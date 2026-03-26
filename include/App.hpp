#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp"
#include "Util/Renderer.hpp"
#include "Util/GameObject.hpp"
#include <vector>
#include <string>
#include <memory>

class App {
public:
    enum class State {
        START,
        UPDATE,
        DEAD,
        PAUSE,
        END
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End();

private:
    // 碰撞偵測：player 碰撞 stone 或 trap
    bool IsColliding(const std::shared_ptr<Util::GameObject>& player, const std::shared_ptr<Util::GameObject>& target);
    // 關卡管理
    int m_CurrentLevelNum = 1;
    void LoadLevel(int level);
    void ClearLevel();
    State m_CurrentState = State::START;

    std::shared_ptr<Util::Renderer> m_Root;
    std::shared_ptr<Util::GameObject> m_PauseScreen;
    std::shared_ptr<Util::GameObject> m_DeadScreen;
    std::shared_ptr<Util::GameObject> m_Background;
    std::shared_ptr<Util::GameObject> m_Ice;
    std::shared_ptr<Util::GameObject> m_Fire;
    std::shared_ptr<Util::GameObject> m_Trap;
    std::vector<std::shared_ptr<Util::GameObject>> m_Stones;
    std::shared_ptr<Util::GameObject> m_IceDoor;
    std::shared_ptr<Util::GameObject> m_FireDoor;
    std::shared_ptr<Util::GameObject> m_Box;                //箱子

    // 座標文字
    std::shared_ptr<Util::GameObject> m_IcePosText;
    std::shared_ptr<Util::GameObject> m_FirePosText;


    //機關
    std::shared_ptr<Util::GameObject> m_Button;
    std::shared_ptr<Util::GameObject> m_Gear;
    glm::vec2 m_GearOriginalPos;

    float m_Gravity = 0.4f;
    float m_JumpForce = 12.0f;
    float m_MoveSpeed = 5.0f;
    float m_BoxMoveSpeedLimit = 3.0f;

    float m_IceVelocityY = 0.0f;
    float m_FireVelocityY = 0.0f;

    float m_BoxVelocityY = 0.0f; // 箱子的垂直速度
    bool m_BoxOnGround = false;  // 箱子是否在地板上

    bool m_IceOnGround = false;
    bool m_FireOnGround = false;
};

#endif