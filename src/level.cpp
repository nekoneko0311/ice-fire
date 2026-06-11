#include "App.hpp"
#include "Util/Image.hpp"
#include "slope.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

void App::LoadLevel(int level) {
    ClearLevel();

    m_Score = 0;

    m_CurrentState = State::UPDATE;
    m_DeadScreen->SetVisible(false);
    m_PauseScreen->SetVisible(false);

    m_GameTime = 0.0f;

    m_IceVelocityY = 0.0f;
    m_FireVelocityY = 0.0f;
    m_BoxVelocityY = 0.0f;

    m_IceOnGround = false;
    m_FireOnGround = false;
    m_BoxOnGround = false;

    srand(time(NULL));
    const float TILE_SIZE = 23.0f;
    const int MAP_WIDTH = 39;
    const int MAP_HEIGHT = 29;
    std::string mapPath = "../resources/map/level" + std::to_string(level) + ".txt";
    float startX = -437.0f;
    float startY = 322.0f;

    //建立地圖
    {std::ifstream file(mapPath);
    if (!file.is_open()) {
        printf("Failed to open map file: %s\n", mapPath.c_str());
        return;
    }
    std::string line;
    for (int row = 0; row < MAP_HEIGHT && std::getline(file, line); ++row) {
        std::stringstream ss(line);
        std::string cell;

        for (int col = 0; col < MAP_WIDTH && ss >> cell; ++col) {
            float posX = startX + (col * TILE_SIZE);
            float posY = startY - (row * TILE_SIZE);
            printf("Loading cell at row %d, col %d: %s (posX: %.2f, posY: %.2f)\n", row, col, cell.c_str(), posX, posY);
            if (cell == "1") {
                int randomIdx = (std::rand() % 4) + 1;
                std::string stonePath = PIC_PATH + "stone" + std::to_string(randomIdx) + ".png";

                auto stone = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(stonePath), -1.0f
                );
                stone->m_Transform.translation = { posX, posY };
                m_Stones.push_back(stone);
                m_Root->AddChild(stone);
            }
            else if (cell == "2") {
                auto stone = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(PIC_PATH + "trapbottom.png"), -1.0f
                );
                stone->m_Transform.translation = { posX, posY - 7.5f};
                m_Stones.push_back(stone);
                m_Root->AddChild(stone);
                auto trap = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(PIC_PATH + "ice_traps/1.png"), -1.0f
                );
                trap->m_Transform.translation = { posX, posY + 4.0f };
                m_IceTraps.push_back(trap);
                m_Root->AddChild(trap);
            }
            else if (cell == "3") {
                auto stone = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(PIC_PATH + "trapbottom.png"), -1.0f
                );
                stone->m_Transform.translation = { posX, posY - 7.5f};
                m_Stones.push_back(stone);
                m_Root->AddChild(stone);
                auto trap = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(PIC_PATH + "fire_traps/1.png"), -1.0f
                );
                trap->m_Transform.translation = { posX, posY };
                m_FireTraps.push_back(trap);
                m_Root->AddChild(trap);
            }
            else if (cell == "4") {
                auto stone = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(PIC_PATH + "trapbottom.png"), -1.0f
                );
                stone->m_Transform.translation = { posX, posY - 7.5f};
                m_Stones.push_back(stone);
                m_Root->AddChild(stone);
                auto trap = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Image>(PIC_PATH + "traps/1.png"), -1.0f
                );
                trap->m_Transform.translation = { posX, posY };
                m_Traps.push_back(trap);
                m_Root->AddChild(trap);
            }
        }
    }
    file.close();
    }

    if (level == 1) {
        m_Ice->m_Transform.translation = { -350.0f, -190.0f };
        m_Fire->m_Transform.translation = { -350.0f, -280.0f };
        m_IceVelocityY = 0;
        m_FireVelocityY = 0;

        m_IceDoor->m_Transform.translation = { 370.0f, 240.0f };
        m_FireDoor->m_Transform.translation = { 300.0f, 240.0f };

        m_IceDoorFrameIndex = 0;
        m_FireDoorFrameIndex = 0;
        m_IceDoor->SetDrawable(std::make_shared<Util::Image>(m_IceDoorFrames[0]));
        m_FireDoor->SetDrawable(std::make_shared<Util::Image>(m_FireDoorFrames[0]));

        if (!m_Box) {
            m_Box = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(PIC_PATH + "box.png"), 0.1f
            );
            m_Root->AddChild(m_Box);
        }
        m_BoxVelocityY = 0.0f;
        m_BoxOnGround = false;
        m_Box->m_Transform.translation = { 10.0f, 150.0f };

        // --- 雙按鈕組合 (控制 b2gear1) ---
        button2_1_1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        button2_1_1->m_Transform.translation = { -300.0f, -10.0f };
        m_Root->AddChild(button2_1_1);

        button2_1_2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.0f);
        button2_1_2->m_Transform.translation = { 100.0f, 130.0f };
        m_Root->AddChild(button2_1_2);


        // --- 兩個按鈕控制的機關 (b2gear1) ---
        b2gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        b2gear1->m_Transform.translation = { 380.0f, 46.0f };
        b2gear1OriginalPos = b2gear1->m_Transform.translation; // 記錄原始位置
        m_Root->AddChild(b2gear1);


        // --- Switch 控制的機關 (sgear1) ---
        sgear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        sgear1->m_Transform.translation = { -385.0f, -27.0f };
        sgear1OriginalPos = sgear1->m_Transform.translation; // 記錄原始位置
        m_Root->AddChild(sgear1);


        // --- 拉桿 (switch1) ---
        switch1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        switch1->m_Transform.translation = { -150.0f, -130.0f };
        switch1State = false; // 初始狀態設為關閉
        m_Root->AddChild(switch1);

        InitDiamonds(
        {
            {-205.0f, 271.0f},
            {-20.0f, 225.0f}
        },
        {
            {-390.0f, 202.0f},
            {70.0f, 225.0f}
        }
    );
    }

    else if (level == 2) {
        m_Ice->m_Transform.translation = { -400.0f, -280.0f };
        m_Fire->m_Transform.translation = { -350.0f, -280.0f };
        m_IceVelocityY = 0;
        m_FireVelocityY = 0;

        m_IceDoor->m_Transform.translation = { -325.0f, 270.0f };
        m_FireDoor->m_Transform.translation = { -400.0f, 270.0f };

        m_IceDoorFrameIndex = 0;
        m_FireDoorFrameIndex = 0;
        m_IceDoor->SetDrawable(std::make_shared<Util::Image>(m_IceDoorFrames[0]));
        m_FireDoor->SetDrawable(std::make_shared<Util::Image>(m_FireDoorFrames[0]));
        
        if (!m_Box) {
            m_Box = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(PIC_PATH + "box.png"), 0.1f
            );
            m_Root->AddChild(m_Box);
        }
        m_BoxVelocityY = 0.0f;
        m_BoxOnGround = false;
        m_Box->m_Transform.translation = { -9999.0f, -9999.0f };

        button2_2_1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        button2_2_1->m_Transform.translation = { 280.0f, -145.0f };
        m_Root->AddChild(button2_2_1);

        button2_2_2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        button2_2_2->m_Transform.translation = { -245.0f, -145.0f };
        m_Root->AddChild(button2_2_2);

        button2_3_1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        button2_3_1->m_Transform.translation = { 150.0f, 225.0f };
        m_Root->AddChild(button2_3_1);

        button2_3_2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        button2_3_2->m_Transform.translation = { -150.0f, 225.0f };
        m_Root->AddChild(button2_3_2);


        // --- 兩個按鈕控制的機關 (b2gear2 與 b2gear3) ---

        b2gear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear4.png"), -1.0f);
        b2gear2->m_Transform.translation = { 0.0f, -100.0f };
        b2gear2OriginalPos = b2gear2->m_Transform.translation;
        m_Root->AddChild(b2gear2);

        b2gear3 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        b2gear3->m_Transform.translation = { 80.0f, 215.0f };
        b2gear3OriginalPos = b2gear3->m_Transform.translation;
        m_Root->AddChild(b2gear3);


        // --- 鐵鍊旋轉平台 ---
        m_ChainPlatform = std::make_shared<ChainPlatform>(
        PIC_PATH + "Lift1.png",   // 鐵鍊圖片
        PIC_PATH + "Lift2.png",   // 平台圖片
        glm::vec2(150.0f, 73.0f),    // 鐵鍊位置
        glm::vec2(150.0f, 20.0f),     // 平台位置

        glm::vec2(0.4f, 0.4f),      // 鐵鍊縮放
        glm::vec2(0.7f, 0.5f),      // 平台縮放

        160.0f,                     // 平台碰撞寬度
        12.0f                       // 平台碰撞高度，先用 12 比較貼圖
        );

        m_ChainPlatform->SetRotation(0.0f);

        m_Root->AddChild(m_ChainPlatform->GetChainObject());
        m_Root->AddChild(m_ChainPlatform->GetBoardObject());

        // --- 鐵鍊旋轉平台 ---
        m_ChainPlatform2 = std::make_shared<ChainPlatform>(
        PIC_PATH + "Lift1.png",   // 鐵鍊圖片
        PIC_PATH + "Lift2.png",   // 平台圖片
        glm::vec2(-150.0f, 73.0f),    // 鐵鍊位置
        glm::vec2(-150.0f, 20.0f),     // 平台位置

        glm::vec2(0.4f, 0.4f),      // 鐵鍊縮放
        glm::vec2(0.7f, 0.5f),      // 平台縮放

        160.0f,                     // 平台碰撞寬度
        12.0f                       // 平台碰撞高度，先用 12 比較貼圖
        );

        m_ChainPlatform->SetRotation(0.0f);

        m_Root->AddChild(m_ChainPlatform2->GetChainObject());
        m_Root->AddChild(m_ChainPlatform2->GetBoardObject());

        InitDiamonds(
        {
            {-230.0f, -295.0f},
            {-137.0f, -295.0f},
            {139.0f, -226.0f},
            {230.0f, -226.0f},
            {-95.0f, -119.0f},
            {207.0f, -119.0f},
            {-23.0f, 18.0f},
            {-23.0f, 248.0f},
        },
        {
            {-230.0f, -226.0f},
            {-137.0f, -226.0f},
            {139.0f, -295.0f},
            {230.0f, -295.0f},
            {95.0f, -119.0f},
            {-205.0f, -119.0f},
            {23.0f, 18.0f},
            {23.0f, 248.0f},
        }
    );
    }

    else if (level == 3) {
        m_Ice->m_Transform.translation = { 350.0f, -280.0f };
        m_Fire->m_Transform.translation = { -350.0f, -280.0f };
        m_IceVelocityY = 0;
        m_FireVelocityY = 0;

        m_IceDoor->m_Transform.translation = { 150.0f, -30.0f };
        m_FireDoor->m_Transform.translation = { -150.0f, -30.0f };

        m_IceDoorFrameIndex = 0;
        m_FireDoorFrameIndex = 0;
        m_IceDoor->SetDrawable(std::make_shared<Util::Image>(m_IceDoorFrames[0]));
        m_FireDoor->SetDrawable(std::make_shared<Util::Image>(m_FireDoorFrames[0]));

        if (!m_Box) {
            m_Box = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(PIC_PATH + "box.png"), 0.1f
            );
            m_Root->AddChild(m_Box);
        }
        m_BoxVelocityY = 0.0f;
        m_BoxOnGround = false;
        m_Box->m_Transform.translation = { -9999.0f, -9999.0f };

        // ========== 風扇 1 =======================================================

        std::vector<std::string> fanFrames;
        std::vector<std::string> windFrames;

        for (int i = 1; i <= 4; i++) {
            fanFrames.push_back(PIC_PATH + "fan(" + std::to_string(i) + ").png");
        }

        for (int i = 1; i <= 10; i++) {
            windFrames.push_back(PIC_PATH + "wind(" + std::to_string(i) + ").png");
        }

        m_Fan = std::make_shared<Fan>(fanFrames, windFrames);
        m_Root->AddChild(m_Fan->GetWindObject());
        m_Root->AddChild(m_Fan->GetFanObject());
        m_Fan->SetActive(true);

        m_Fan->SetPosition(
            glm::vec2(-390.0f, -7.0f),
            glm::vec2(0.0f, 140.0f)
        );

        m_Fan->SetScale(
            glm::vec2(0.45f, 0.45f),
            glm::vec2(0.45f, 0.85f)
        );


        // ========== 風扇 2 =======================================================

        m_Fan2 = std::make_shared<Fan>(fanFrames, windFrames);
        m_Root->AddChild(m_Fan2->GetWindObject());
        m_Root->AddChild(m_Fan2->GetFanObject());
        m_Fan2->SetActive(true);

        m_Fan2->SetPosition(
            glm::vec2(390.0f, -7.0f),
        glm::vec2(0.0f, 140.0f));

        m_Fan2->SetScale(
            glm::vec2(0.45f, 0.45f),
            glm::vec2(0.45f, 0.85f)
        );
        // // =====================================================================

        InitDiamonds(
        {
            {-240.0f, -295.0f},
            {-170.0f, -295.0f},
            {251.0f, -157.0f},
            {184.0f, -157.0f},
            {390.0f, 70.0f},
            {390.0f, 140.0f},
            {390.0f, 210.0f},
            {138.0f, 271.0f},
            {83.0f, 133.0f},
            {-158.0f, 64.0f},
        },
        {
            {240.0f, -295.0f},
            {170.0f, -295.0f},
            {-251.0f, -157.0f},
            {-184.0f, -157.0f},
            {-390.0f, 70.0f},
            {-390.0f, 140.0f},
            {-390.0f, 210.0f},
            {-138.0f, 271.0f},
            {-83.0f, 133.0f},
            {158.0f, 64.0f},
        }
    );
    }

    else if (level == 4) {
        m_Ice->m_Transform.translation = { 350.0f, 250.0f };
        m_Fire->m_Transform.translation = { -400.0f, -280.0f };
        m_IceVelocityY = 0;
        m_FireVelocityY = 0;

        m_IceDoor->m_Transform.translation = { -250.0f, -150.0f };
        m_FireDoor->m_Transform.translation = { -300.0f, -150.0f };

        m_IceDoorFrameIndex = 0;
        m_FireDoorFrameIndex = 0;
        m_IceDoor->SetDrawable(std::make_shared<Util::Image>(m_IceDoorFrames[0]));
        m_FireDoor->SetDrawable(std::make_shared<Util::Image>(m_FireDoorFrames[0]));

        if (!m_Box) {
            m_Box = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(PIC_PATH + "box.png"), 0.1f
            );
            m_Root->AddChild(m_Box);
        }
        m_BoxVelocityY = 0.0f;
        m_BoxOnGround = false;
        m_Box->m_Transform.translation = { 300.0f, 250.0f };

        // --- 按鈕 1 與其控制的機關 (button1 & b1gear1) ---
        button1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        button1->m_Transform.translation = { 150.0f, -80.0f };
        m_Root->AddChild(button1);

        b1gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        b1gear1->m_Transform.translation = { 300.0f, 0.0f };
        b1gear1OriginalPos = b1gear1->m_Transform.translation; // 記錄原始位置
        m_Root->AddChild(b1gear1);


        // --- 拉桿 2 與其控制的機關 (switch2 & sgear2) ---
        switch2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        switch2->m_Transform.translation = { -150.0f, -80.0f };
        switch2State = false; // 初始狀態設為關閉
        m_Root->AddChild(switch2);

        sgear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        sgear2->m_Transform.translation = { -300.0f, 0.0f };
        sgear2OriginalPos = sgear2->m_Transform.translation; // 記錄原始位置
        m_Root->AddChild(sgear2);
        // ============= 比重量平台 ==================

        m_BalanceRopePlatform = std::make_shared<BalanceRopePlatform>(
        PIC_PATH + "wood.png",
        PIC_PATH + "chain_left.png",
        PIC_PATH + "chain_link.png",
        PIC_PATH + "wheel.png",

        glm::vec2(-100.0f, -30.0f), // 整組位置，X 越小越左
        250.0f,                     // 左右平台距離中心
        -250.0f,                    // 木板平衡時 Y
        100.0f,                     // 輪子 Y，越大鏈越長

        130.0f,                     // 木板碰撞寬
        18.0f,                      // 木板碰撞高
        -300.0f,                    // 最低 Y，越小掉越低

        glm::vec2(0.55f, 0.55f),    // 木板 scale
        glm::vec2(0.55f, 0.55f),    // V 鏈 scale
        glm::vec2(0.45f, 0.45f),    // 小鏈節 scale
        glm::vec2(0.45f, 0.45f)     // 輪子 scale
        );

        for (auto& obj : m_BalanceRopePlatform->GetAllObjects()) {
            m_Root->AddChild(obj);
        }

        InitDiamonds(
        {
            {-250.0f, -100.0f}
        },
        {
            {-300.0f, -100.0f}
        }
    );
    }

    else if (level == 5) {
        m_Ice->m_Transform.translation = { 100.0f, -280.0f };
        m_Fire->m_Transform.translation = { -100.0f, -280.0f };
        m_IceVelocityY = 0;
        m_FireVelocityY = 0;

        m_IceDoor->m_Transform.translation = { 220.0f, -10.0f };
        m_FireDoor->m_Transform.translation = { -220.0f, -10.0f };

        m_IceDoorFrameIndex = 0;
        m_FireDoorFrameIndex = 0;
        m_IceDoor->SetDrawable(std::make_shared<Util::Image>(m_IceDoorFrames[0]));
        m_FireDoor->SetDrawable(std::make_shared<Util::Image>(m_FireDoorFrames[0]));

        if (!m_Box) {
            m_Box = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(PIC_PATH + "box.png"), 0.1f
            );
            m_Root->AddChild(m_Box);
        }
        m_BoxVelocityY = 0.0f;
        m_BoxOnGround = false;
        m_Box->m_Transform.translation = { -9999.0f, -9999.0f };

        // --- 拉桿 3 與其控制的機關 (switch3 & sgear3) ---
        switch3 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        switch3->m_Transform.translation = { -200.0f, -100.0f };
        switch3State = false; 
        m_Root->AddChild(switch3);

        sgear3 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        sgear3->m_Transform.translation = { -200.0f, 100.0f };
        sgear3OriginalPos = sgear3->m_Transform.translation; 
        m_Root->AddChild(sgear3);


        // --- 拉桿 4 與其控制的機關 (switch4 & sgear4) ---
        switch4 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        switch4->m_Transform.translation = { 0.0f, -100.0f };
        switch4State = false; 
        m_Root->AddChild(switch4);

        sgear4 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        sgear4->m_Transform.translation = { 0.0f, 100.0f };
        sgear4OriginalPos = sgear4->m_Transform.translation; 
        m_Root->AddChild(sgear4);


        // --- 拉桿 5 與其控制的機關 (switch5 & sgear5) ---
        switch5 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        switch5->m_Transform.translation = { 200.0f, -100.0f };
        switch5State = false; 
        m_Root->AddChild(switch5);

        sgear5 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        sgear5->m_Transform.translation = { 200.0f, 100.0f };
        sgear5OriginalPos = sgear5->m_Transform.translation; 
        m_Root->AddChild(sgear5);

        InitDiamonds(
        {
            {-250.0f, -100.0f}
        },
        {
            {-300.0f, -100.0f}
        }
    );
    }

}


void App::ClearLevel() {
    // 1. 清理石頭
    for (auto& stone : m_Stones) m_Root->RemoveChild(stone);
    m_Stones.clear();

    // 2. 清理陷阱群 (Traps, IceTraps, FireTraps)
    for (auto& t : m_Traps) m_Root->RemoveChild(t);
    m_Traps.clear();
    for (auto& t : m_IceTraps) m_Root->RemoveChild(t);
    m_IceTraps.clear();
    for (auto& t : m_FireTraps) m_Root->RemoveChild(t);
    m_FireTraps.clear();

    // ===== 清除所有 Buttons =====
    if (button1) { m_Root->RemoveChild(button1); button1 = nullptr; }
    if (button2_1_1) { m_Root->RemoveChild(button2_1_1); button2_1_1 = nullptr; }
    if (button2_1_2) { m_Root->RemoveChild(button2_1_2); button2_1_2 = nullptr; }
    if (button2_2_1) { m_Root->RemoveChild(button2_2_1); button2_2_1 = nullptr; }
    if (button2_2_2) { m_Root->RemoveChild(button2_2_2); button2_2_2 = nullptr; }
    if (button2_3_1) { m_Root->RemoveChild(button2_3_1); button2_3_1 = nullptr; }
    if (button2_3_2) { m_Root->RemoveChild(button2_3_2); button2_3_2 = nullptr; }

    // ===== 清除所有 Switches 並重置狀態 =====
    if (switch1) { m_Root->RemoveChild(switch1); switch1 = nullptr; }
    if (switch2) { m_Root->RemoveChild(switch2); switch2 = nullptr; }
    if (switch3) { m_Root->RemoveChild(switch3); switch3 = nullptr; }
    if (switch4) { m_Root->RemoveChild(switch4); switch4 = nullptr; }
    if (switch5) { m_Root->RemoveChild(switch5); switch5 = nullptr; }

    switch1State = false;
    switch2State = false;
    switch3State = false;
    switch4State = false;
    switch5State = false;

    // ===== 清除所有 Gears =====
    if (sgear1) { m_Root->RemoveChild(sgear1); sgear1 = nullptr; }
    if (sgear2) { m_Root->RemoveChild(sgear2); sgear2 = nullptr; }
    if (sgear3) { m_Root->RemoveChild(sgear3); sgear3 = nullptr; }
    if (sgear4) { m_Root->RemoveChild(sgear4); sgear4 = nullptr; }
    if (sgear5) { m_Root->RemoveChild(sgear5); sgear5 = nullptr; }

    if (b1gear1) { m_Root->RemoveChild(b1gear1); b1gear1 = nullptr; }

    if (b2gear1) { m_Root->RemoveChild(b2gear1); b2gear1 = nullptr; }
    if (b2gear2) { m_Root->RemoveChild(b2gear2); b2gear2 = nullptr; }
    if (b2gear3) { m_Root->RemoveChild(b2gear3); b2gear3 = nullptr; }

    // ===== 重置所有原始座標 =====
    sgear1OriginalPos = { 9999.0f, 9999.0f };
    sgear2OriginalPos = { 9999.0f, 9999.0f };
    sgear3OriginalPos = { 9999.0f, 9999.0f };
    sgear4OriginalPos = { 9999.0f, 9999.0f };
    sgear5OriginalPos = { 9999.0f, 9999.0f };

    b1gear1OriginalPos = { 9999.0f, 9999.0f };

    b2gear1OriginalPos = { 9999.0f, 9999.0f };
    b2gear2OriginalPos = { 9999.0f, 9999.0f };
    b2gear3OriginalPos = { 9999.0f, 9999.0f };
    // auto cleanup = [&](std::shared_ptr<Util::GameObject>& obj) {
    //     if (obj) {
    //         m_Root->RemoveChild(obj);
    //         obj = nullptr;
    //     }
    // };
    // ===== 清理紅寶石 =====
    for (auto& diamond : m_RedDiamonds) {
        if (diamond) {
            m_Root->RemoveChild(diamond);
        }
    }
    m_RedDiamonds.clear();

    // ===== 清理藍寶石 =====
    for (auto& diamond : m_BlueDiamonds) {
        if (diamond) {
            m_Root->RemoveChild(diamond);
        }
    }
    m_BlueDiamonds.clear();

    m_RedDiamondBasePos.clear();
    m_BlueDiamondBasePos.clear();
    m_RedDiamondCollected.clear();
    m_BlueDiamondCollected.clear();

    if (m_Fan) {
        m_Root->RemoveChild(m_Fan->GetWindObject());
        m_Root->RemoveChild(m_Fan->GetFanObject());
        m_Fan = nullptr;
    }

    if (m_Fan2) {
        m_Root->RemoveChild(m_Fan2->GetWindObject());
        m_Root->RemoveChild(m_Fan2->GetFanObject());
        m_Fan2 = nullptr;
    }

    if (m_ChainPlatform) {
        m_Root->RemoveChild(m_ChainPlatform->GetChainObject());
        m_Root->RemoveChild(m_ChainPlatform->GetBoardObject());
        m_ChainPlatform = nullptr;
    }

    if (m_ChainPlatform2) {
        m_Root->RemoveChild(m_ChainPlatform2->GetChainObject());
        m_Root->RemoveChild(m_ChainPlatform2->GetBoardObject());
        m_ChainPlatform2 = nullptr;
    }

    if (m_BalanceRopePlatform) {
        for (auto& obj : m_BalanceRopePlatform->GetAllObjects()) {
            m_Root->RemoveChild(obj);
        }
        m_BalanceRopePlatform = nullptr;
    }

    m_IceVelocityY = 0;
    m_FireVelocityY = 0;
}