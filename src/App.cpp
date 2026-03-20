#include "App.hpp"
#include "config.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"

const std::string PIC_PATH = "../Resources/picture/";
const std::string FONT_PATH = "../Resources/font/";

void App::Start() {
    m_Root = std::make_shared<Util::Renderer>();

    // 1. 暫停與死亡畫面 (層級最高)
    m_PauseScreen = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "stop.png"), 20.0f);
    m_PauseScreen->SetVisible(false);
    m_Root->AddChild(m_PauseScreen);

    m_DeadScreen = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "dead.png"), 25.0f);
    m_DeadScreen->SetVisible(false);
    m_Root->AddChild(m_DeadScreen);

    // 2. 背景與角色
    m_Background = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "background.png"), -10.0f);
    m_Root->AddChild(m_Background);

    m_Ice = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "ice.png"), 0.0f);
    m_Fire = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "fire.png"), 0.0f);
    m_Root->AddChild(m_Ice);
    m_Root->AddChild(m_Fire);

    // 3. 地板與陷阱
    m_Trap = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "trap0.png"), -1.0f);
    m_Root->AddChild(m_Trap);

    // 4. 門 (暫時放在這裡，之後會移到 LoadLevel)
    if (!m_IceDoor) {
        m_IceDoor = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "ice_door.png"), -2.0f);
        m_FireDoor = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "fire_door.png"), -2.0f);
        m_Root->AddChild(m_IceDoor);
        m_Root->AddChild(m_FireDoor);
    }

    // 4. UI 文字
    std::string font = FONT_PATH + "arial.ttf";
    m_IcePosText = std::make_shared<Util::GameObject>(std::make_shared<Util::Text>(font, 20, "Ice: (0, 0)", Util::Color(51,153,255)), 10.0f);
    m_IcePosText->SetPivot({-1.0f, 1.0f});
    m_IcePosText->m_Transform.translation = { -570.0f, 340.0f };
    m_Root->AddChild(m_IcePosText);

    m_FirePosText = std::make_shared<Util::GameObject>(std::make_shared<Util::Text>(font, 20, "Fire: (0, 0)", Util::Color(255, 0, 0)), 10.0f);
    m_FirePosText->SetPivot({1.0f, 1.0f});
    m_FirePosText->m_Transform.translation = { 570.0f, 340.0f };
    m_Root->AddChild(m_FirePosText);

    // 初始化第一關
    LoadLevel(1);
    m_CurrentState = State::UPDATE;
}

void App::LoadLevel(int level) {
    ClearLevel(); // 清理舊有的物件

    // 重置角色位置
    m_Ice->m_Transform.translation = { -500.0f, 0.0f };
    m_Fire->m_Transform.translation = { 500.0f, 0.0f };
    m_IceVelocityY = 0;
    m_FireVelocityY = 0;

    // 設定門的位置
    m_IceDoor->m_Transform.translation = { -300.0f, -171.0f };
    m_FireDoor->m_Transform.translation = { 300.0f, -171.0f };

    // 依照關卡編號設定地形 (目前第二關與第一關相同)
    if (level == 1 || level == 2) {
        for (int i = 0; i < 50; ++i) {
            auto stone = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "stone1.png"), -1.0f);
            stone->m_Transform.translation = { -640.0f + (i * stone->GetScaledSize().x), -200.0f };
            m_Stones.push_back(stone);
            m_Root->AddChild(stone);
        }
        m_Trap->m_Transform.translation = { 0.0f, -171.0f };
    }

    m_CurrentLevelNum = level;
    LOG_INFO("Loading Level " + std::to_string(level));
}

void App::Update() {
    if (Util::Input::IfExit()) { m_CurrentState = State::END; return; }

    // --- 死亡狀態 ---
    if (m_CurrentState == State::DEAD) {
        if (Util::Input::IsKeyDown(Util::Keycode::R)) {
            m_DeadScreen->SetVisible(false);
            LoadLevel(m_CurrentLevelNum); // 死亡後按 R 重載當前關卡
            m_CurrentState = State::UPDATE;
        }
        m_Root->Update();
        return;
    }

    // --- 暫停邏輯 ---
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
        if (m_CurrentState == State::UPDATE) {
            m_CurrentState = State::PAUSE;
            m_PauseScreen->SetVisible(true);
        } else if (m_CurrentState == State::PAUSE) {
            m_CurrentState = State::UPDATE;
            m_PauseScreen->SetVisible(false);
        }
    }
    if (m_CurrentState == State::PAUSE) { m_Root->Update(); return; }

    // --- 遊戲更新 ---
    if (m_CurrentState == State::UPDATE) {
        // 新增按 N 進入下一關
        if (Util::Input::IsKeyDown(Util::Keycode::N)) {
            LoadLevel(m_CurrentLevelNum + 1);
            return;
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        float iceDx = 0.0f, fireDx = 0.0f;

        if (keys[SDL_SCANCODE_A]) iceDx -= m_MoveSpeed;
        if (keys[SDL_SCANCODE_D]) iceDx += m_MoveSpeed;
        if (Util::Input::IsKeyDown(Util::Keycode::W) && m_IceOnGround) { m_IceVelocityY = m_JumpForce; m_IceOnGround = false; }

        if (keys[SDL_SCANCODE_LEFT]) fireDx -= m_MoveSpeed;
        if (keys[SDL_SCANCODE_RIGHT]) fireDx += m_MoveSpeed;
        if (Util::Input::IsKeyDown(Util::Keycode::UP) && m_FireOnGround) { m_FireVelocityY = m_JumpForce; m_FireOnGround = false; }

        // 物理位移
        m_IceVelocityY -= m_Gravity;
        m_FireVelocityY -= m_Gravity;
        m_Ice->m_Transform.translation += glm::vec2(iceDx, m_IceVelocityY);
        m_Fire->m_Transform.translation += glm::vec2(fireDx, m_FireVelocityY);

        // 雙人過關判定
        if (IsColliding(m_Ice, m_IceDoor) && IsColliding(m_Fire, m_FireDoor)) {
            LoadLevel(m_CurrentLevelNum + 1);
            return;
        }

        // 陷阱碰撞判定 (碰到就死)
        if (IsColliding(m_Ice, m_Trap) || IsColliding(m_Fire, m_Trap)) {
            m_CurrentState = State::DEAD;
            m_DeadScreen->SetVisible(true);
        }

        // 地板碰撞
        bool iG = false, fG = false;
        for (const auto& stone : m_Stones) {
            float sTop = stone->m_Transform.translation.y + (stone->GetScaledSize().y / 2.0f);
            if (m_IceVelocityY <= 0 && IsColliding(m_Ice, stone)) {
                m_IceVelocityY = 0;
                m_Ice->m_Transform.translation.y = sTop + (m_Ice->GetScaledSize().y / 2.0f);
                iG = true;
            }
            if (m_FireVelocityY <= 0 && IsColliding(m_Fire, stone)) {
                m_FireVelocityY = 0;
                m_Fire->m_Transform.translation.y = sTop + (m_Fire->GetScaledSize().y / 2.0f);
                fG = true;
            }
        }
        m_IceOnGround = iG; m_FireOnGround = fG;

        m_IcePosText->SetDrawable(std::make_shared<Util::Text>(FONT_PATH + "arial.ttf", 20, "Ice: (" + std::to_string((int)m_Ice->m_Transform.translation.x) + "," + std::to_string((int)m_Ice->m_Transform.translation.y) + ")", Util::Color(51,153,255)));
        m_FirePosText->SetDrawable(std::make_shared<Util::Text>(FONT_PATH + "arial.ttf", 20, "Fire: (" + std::to_string((int)m_Fire->m_Transform.translation.x) + "," + std::to_string((int)m_Fire->m_Transform.translation.y) + ")", Util::Color(255,0,0)));

        m_Root->Update();
    }
}

bool App::IsColliding(const std::shared_ptr<Util::GameObject>& p, const std::shared_ptr<Util::GameObject>& t) {
    glm::vec2 pP = p->m_Transform.translation, pS = p->GetScaledSize();
    glm::vec2 tP = t->m_Transform.translation, tS = t->GetScaledSize();
    return (pP.x - pS.x/4 < tP.x + tS.x/2 && pP.x + pS.x/4 > tP.x - tS.x/2 &&
            pP.y - pS.y/2 < tP.y + tS.y/2 && pP.y + pS.y/2 > tP.y - tS.y/2);
}

void App::ClearLevel() {
    for (auto& stone : m_Stones) m_Root->RemoveChild(stone);
    m_Stones.clear();

    // 增加新物件之後要記得在這邊新增清理
}

void App::End() { LOG_TRACE("End"); }