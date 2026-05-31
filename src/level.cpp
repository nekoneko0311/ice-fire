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

    m_CurrentLevelNum = level;
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
        m_Ice->m_Transform.translation = { -350.0f, -280.0f };
        m_Fire->m_Transform.translation = { -380.0f, -280.0f };
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

        auto btn1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        btn1->m_Transform.translation = { -300.0f, -10.0f };
        m_Buttons.push_back(btn1);
        m_Root->AddChild(btn1);

        // 按鈕 2
        auto btn2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.0f);
        btn2->m_Transform.translation = { 100.0f, 130.0f };
        m_Buttons.push_back(btn2);
        m_Root->AddChild(btn2);


        // --- 4. 齒輪/移動地板 (Gears) ---

        auto gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        gear1->m_Transform.translation = { 380.0f, 46.0f };
        m_Gears.push_back(gear1);
        m_GearOriginalPositions.push_back(gear1->m_Transform.translation);
        m_Root->AddChild(gear1);


        auto gear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        gear2->m_Transform.translation = { -390.0f, -27.0f };
        m_Gears.push_back(gear2);
        m_GearOriginalPositions.push_back(gear2->m_Transform.translation);
        m_Root->AddChild(gear2);


        // --- 鐵鍊旋轉平台 ---
            // m_ChainPlatform = std::make_shared<ChainPlatform>(
            //     PIC_PATH + "lift1.png",   // 鐵鍊圖片
            //     PIC_PATH + "lift2.png",   // 平台圖片

            //     glm::vec2(150.0f, -100.0f),    // 鐵鍊位置
            //     glm::vec2(150.0f, -140.0f),     // 平台位置

            //     glm::vec2(0.3f, 0.3f),      // 鐵鍊縮放
            //     glm::vec2(0.3f, 0.3f),      // 平台縮放

            //     160.0f,                     // 平台碰撞寬度
            //     12.0f                       // 平台碰撞高度，先用 12 比較貼圖
            // );

            // m_ChainPlatform->SetRotation(0.0f);

            // m_Root->AddChild(m_ChainPlatform->GetChainObject());
            // m_Root->AddChild(m_ChainPlatform->GetBoardObject());

        // --- 5. 拉桿 (Switches) ---
        auto sw = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        sw->m_Transform.translation = { -150.0f, -130.0f };
        m_Switches.push_back(sw);
        m_SwitchStates.push_back(false); // 初始狀態設為關閉
        m_Root->AddChild(sw);

        // // ==========風扇=======================================================

        // std::vector<std::string> fanFrames;
        // std::vector<std::string> windFrames;

        // for (int i = 1; i <= 4; i++) {
        //     fanFrames.push_back(PIC_PATH + "fan(" + std::to_string(i) + ").png");
        // }
        // for (int i = 1; i <= 10; i++) {
        //     windFrames.push_back(PIC_PATH + "wind(" + std::to_string(i) + ").png");
        // }

        // m_Fan = std::make_shared<Fan>(fanFrames, windFrames);
        // m_Root->AddChild(m_Fan->GetWindObject());
        // m_Root->AddChild(m_Fan->GetFanObject());
        // m_Fan->SetActive(true);

        // m_Fan->SetPosition(
        //     glm::vec2(150.0f, -300.0f),
        //     glm::vec2(0.0f, 120.0f)
        // );

        // m_Fan->SetScale(
        //     glm::vec2(0.45f, 0.45f),
        //     glm::vec2(0.45f, 0.85f)
        // );
        // // =====================================================================

        InitDiamonds();
    }

    else if (level == 2) {
        m_Ice->m_Transform.translation = { -350.0f, -190.0f };
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
        m_Box->m_Transform.translation = { 10.0f, 150.0f };

        auto btn1 = std::make_shared<Util::GameObject>(
    std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f
);
        btn1->m_Transform.translation = { 280.0f, -145.0f };
        m_Buttons.push_back(btn1);
        m_Root->AddChild(btn1);

        auto btn2 = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f
        );
        btn2->m_Transform.translation = { -245.0f, -145.0f };
        m_Buttons.push_back(btn2);
        m_Root->AddChild(btn2);

        auto btn3 = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f
        );
        btn3->m_Transform.translation = { 280.0f, 145.0f };
        m_Buttons.push_back(btn3);
        m_Root->AddChild(btn3);

        auto btn4 = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f
        );
        btn4->m_Transform.translation = { -245.0f, 145.0f };
        m_Buttons.push_back(btn4);
        m_Root->AddChild(btn4);


        // --- 4. 齒輪/移動地板 (Gears) ---
        // 齒輪 1
        auto gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        gear1->m_Transform.translation = { -5.0f, -11500.0f };
        gear1->m_Transform.rotation = glm::radians(90.0f);
        m_Gears.push_back(gear1);
        m_GearOriginalPositions.push_back(gear1->m_Transform.translation);
        m_Root->AddChild(gear1);

        // 齒輪 2 (垂直旋轉的)
        auto gear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        gear2->m_Transform.translation = { -390.0f, 27.0f };
        m_Gears.push_back(gear2);
        m_GearOriginalPositions.push_back(gear2->m_Transform.translation);
        m_Root->AddChild(gear2);

        // --- 新增 gearMoveUp ---gear3
        auto gearMoveUp = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        gearMoveUp->m_Transform.translation = { 5.0f, -109.0f };
        gearMoveUp->m_Transform.rotation = glm::radians(90.0f);
        m_Gears.push_back(gearMoveUp);
        m_GearOriginalPositions.push_back(gearMoveUp->m_Transform.translation);
        m_Root->AddChild(gearMoveUp);

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


        // // ==========風扇=======================================================

        // std::vector<std::string> fanFrames;
        // std::vector<std::string> windFrames;

        // for (int i = 1; i <= 4; i++) {
        //     fanFrames.push_back(PIC_PATH + "fan(" + std::to_string(i) + ").png");
        // }
        // for (int i = 1; i <= 10; i++) {
        //     windFrames.push_back(PIC_PATH + "wind(" + std::to_string(i) + ").png");
        // }

        // m_Fan = std::make_shared<Fan>(fanFrames, windFrames);
        // m_Root->AddChild(m_Fan->GetWindObject());
        // m_Root->AddChild(m_Fan->GetFanObject());
        // m_Fan->SetActive(true);

        // m_Fan->SetPosition(
        //     glm::vec2(150.0f, -300.0f),
        //     glm::vec2(0.0f, 120.0f)
        // );

        // m_Fan->SetScale(
        //     glm::vec2(0.45f, 0.45f),
        //     glm::vec2(0.45f, 0.85f)
        // );
        // // =====================================================================

        InitDiamonds();
    }

    else if (level == 3) {
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

        auto btn1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        btn1->m_Transform.translation = { -300.0f, -10.0f };
        m_Buttons.push_back(btn1);
        m_Root->AddChild(btn1);

        // 按鈕 2
        auto btn2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.0f);
        btn2->m_Transform.translation = { 100.0f, 130.0f };
        m_Buttons.push_back(btn2);
        m_Root->AddChild(btn2);


        // --- 4. 齒輪/移動地板 (Gears) ---
        // 齒輪 1
        auto gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        gear1->m_Transform.translation = { 380.0f, 46.0f };
        m_Gears.push_back(gear1);
        m_GearOriginalPositions.push_back(gear1->m_Transform.translation);
        m_Root->AddChild(gear1);

        // 齒輪 2 (垂直旋轉的)
        auto gear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        gear2->m_Transform.translation = { -390.0f, -27.0f };
        m_Gears.push_back(gear2);
        m_GearOriginalPositions.push_back(gear2->m_Transform.translation);
        m_Root->AddChild(gear2);


        
        // --- 5. 拉桿 (Switches) ---
        auto sw = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        sw->m_Transform.translation = { -150.0f, -130.0f };
        m_Switches.push_back(sw);
        m_SwitchStates.push_back(false); // 初始狀態設為關閉
        m_Root->AddChild(sw);

        // // ==========風扇=======================================================

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
        // // =====================================================================

        InitDiamonds();
    }

    else if (level == 4) {
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

        auto btn1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        btn1->m_Transform.translation = { -300.0f, -10.0f };
        m_Buttons.push_back(btn1);
        m_Root->AddChild(btn1);

        // 按鈕 2
        auto btn2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.0f);
        btn2->m_Transform.translation = { 100.0f, 130.0f };
        m_Buttons.push_back(btn2);
        m_Root->AddChild(btn2);


        // --- 4. 齒輪/移動地板 (Gears) ---
        // 齒輪 1
        auto gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        gear1->m_Transform.translation = { 380.0f, 46.0f };
        m_Gears.push_back(gear1);
        m_GearOriginalPositions.push_back(gear1->m_Transform.translation);
        m_Root->AddChild(gear1);

        // 齒輪 2 (垂直旋轉的)
        auto gear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        gear2->m_Transform.translation = { -390.0f, -27.0f };
        m_Gears.push_back(gear2);
        m_GearOriginalPositions.push_back(gear2->m_Transform.translation);
        m_Root->AddChild(gear2);




        
        // --- 5. 拉桿 (Switches) ---
        auto sw = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        sw->m_Transform.translation = { -150.0f, -130.0f };
        m_Switches.push_back(sw);
        m_SwitchStates.push_back(false); // 初始狀態設為關閉
        m_Root->AddChild(sw);

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

        // // ==========風扇=======================================================

        // std::vector<std::string> fanFrames;
        // std::vector<std::string> windFrames;

        // for (int i = 1; i <= 4; i++) {
        //     fanFrames.push_back(PIC_PATH + "fan(" + std::to_string(i) + ").png");
        // }
        // for (int i = 1; i <= 10; i++) {
        //     windFrames.push_back(PIC_PATH + "wind(" + std::to_string(i) + ").png");
        // }

        // m_Fan = std::make_shared<Fan>(fanFrames, windFrames);
        // m_Root->AddChild(m_Fan->GetWindObject());
        // m_Root->AddChild(m_Fan->GetFanObject());
        // m_Fan->SetActive(true);

        // m_Fan->SetPosition(
        //     glm::vec2(150.0f, -300.0f),
        //     glm::vec2(0.0f, 120.0f)
        // );

        // m_Fan->SetScale(
        //     glm::vec2(0.45f, 0.45f),
        //     glm::vec2(0.45f, 0.85f)
        // );
        // // =====================================================================

        InitDiamonds();
    }

    else if (level == 5) {
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

        auto btn1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.5f);
        btn1->m_Transform.translation = { -300.0f, -10.0f };
        m_Buttons.push_back(btn1);
        m_Root->AddChild(btn1);

        // 按鈕 2
        auto btn2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "button1.png"), -1.0f);
        btn2->m_Transform.translation = { 100.0f, 130.0f };
        m_Buttons.push_back(btn2);
        m_Root->AddChild(btn2);


        // --- 4. 齒輪/移動地板 (Gears) ---
        // 齒輪 1
        auto gear1 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear1.png"), -1.0f);
        gear1->m_Transform.translation = { 380.0f, 46.0f };
        m_Gears.push_back(gear1);
        m_GearOriginalPositions.push_back(gear1->m_Transform.translation);
        m_Root->AddChild(gear1);

        // 齒輪 2 (垂直旋轉的)
        auto gear2 = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "gear2.png"), -1.0f);
        gear2->m_Transform.translation = { -390.0f, -27.0f };
        m_Gears.push_back(gear2);
        m_GearOriginalPositions.push_back(gear2->m_Transform.translation);
        m_Root->AddChild(gear2);




        
        // --- 5. 拉桿 (Switches) ---
        auto sw = std::make_shared<Util::GameObject>(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"), -1.5f);
        sw->m_Transform.translation = { -150.0f, -130.0f };
        m_Switches.push_back(sw);
        m_SwitchStates.push_back(false); // 初始狀態設為關閉
        m_Root->AddChild(sw);

        // // ==========風扇=======================================================

        // std::vector<std::string> fanFrames;
        // std::vector<std::string> windFrames;

        // for (int i = 1; i <= 4; i++) {
        //     fanFrames.push_back(PIC_PATH + "fan(" + std::to_string(i) + ").png");
        // }
        // for (int i = 1; i <= 10; i++) {
        //     windFrames.push_back(PIC_PATH + "wind(" + std::to_string(i) + ").png");
        // }

        // m_Fan = std::make_shared<Fan>(fanFrames, windFrames);
        // m_Root->AddChild(m_Fan->GetWindObject());
        // m_Root->AddChild(m_Fan->GetFanObject());
        // m_Fan->SetActive(true);

        // m_Fan->SetPosition(
        //     glm::vec2(150.0f, -300.0f),
        //     glm::vec2(0.0f, 120.0f)
        // );

        // m_Fan->SetScale(
        //     glm::vec2(0.45f, 0.45f),
        //     glm::vec2(0.45f, 0.85f)
        // );
        // // =====================================================================

        InitDiamonds();
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

    // 3. 清理機關類向量 (Buttons, Gears, Switches)
    // 這是你提到的重點：將原本單一的 cleanup 改為迴圈清理向量
    for (auto& btn : m_Buttons) m_Root->RemoveChild(btn);
    m_Buttons.clear();

    for (auto& gear : m_Gears) m_Root->RemoveChild(gear);
    m_Gears.clear();
    m_GearOriginalPositions.clear(); // 座標向量也要清空

    for (auto& sw : m_Switches) m_Root->RemoveChild(sw);
    m_Switches.clear();
    m_SwitchStates.clear(); // 清空開關狀態向量

    // 4. 清理其餘單一物件 (鑽石等)
    auto cleanup = [&](std::shared_ptr<Util::GameObject>& obj) {
        if (obj) {
            m_Root->RemoveChild(obj);
            obj = nullptr;
        }
    };
    cleanup(m_RedDiamond);
    cleanup(m_BlueDiamond);

    // 清理風扇
    if (m_Fan) {
        m_Root->RemoveChild(m_Fan->GetWindObject());
        m_Root->RemoveChild(m_Fan->GetFanObject());
        m_Fan = nullptr;
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

    // 5. 清理斜坡
    for (auto& slope : m_Slopes) {
        if (slope.GetImage()) m_Root->RemoveChild(slope.GetImage());
    }
    m_Slopes.clear();

    // 6. 重置角色狀態與物理參數
    m_IceVelocityY = 0;
    m_FireVelocityY = 0;
    // 如果你有一個全域的開關總變數也可以重置，但主要應該是清空 m_SwitchStates
}