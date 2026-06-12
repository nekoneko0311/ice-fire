#include "App.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Text.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include "slope.hpp"

bool App::IsColliding(const std::shared_ptr<Util::GameObject>& p,
                      const std::shared_ptr<Util::GameObject>& t) {
    if (!p || !t) return false;

    glm::vec2 pP = p->m_Transform.translation;
    glm::vec2 pS = p->GetScaledSize();
    glm::vec2 tP = t->m_Transform.translation;
    glm::vec2 tS = t->GetScaledSize();

    float pW = pS.x;
    float pH = pS.y;
    float tW = tS.x;
    float tH = tS.y;

    if (t == m_IceDoor || t == m_FireDoor) {
        tW *= 0.35f;   // 門寬度判定
        tH *= 0.60f;   // 門高度判定
    }



    return (pP.x - pW / 2.0f < tP.x + tW / 2.0f &&
            pP.x + pW / 2.0f > tP.x - tW / 2.0f &&
            pP.y - pH / 2.0f < tP.y + tH / 2.0f &&
            pP.y + pH / 2.0f > tP.y - tH / 2.0f);
}

void App::HandleMechanics(float iceDx, float fireDx, const Uint8* keys) {
    (void)keys;

    auto handleAdvancedPush = [&]() {
        const float PUSH_SPEED = 3.5f;
        const float SANDWICH_SPEED = 2.0f;

        if (iceDx != 0 && IsColliding(m_Ice, m_Box)) {
            float icePos = m_Ice->m_Transform.translation.x;
            float boxPos = m_Box->m_Transform.translation.x;
            bool isPushing = (icePos < boxPos && iceDx > 0) || (icePos > boxPos && iceDx < 0);
            if (isPushing) {
                float finalDx = (iceDx > 0 ? 1.0f : -1.0f);
                bool isSandwich = IsColliding(m_Box, m_Fire);
                if (isSandwich) {
                    finalDx *= SANDWICH_SPEED;
                    m_Fire->m_Transform.translation.x += finalDx;
                } else {
                    finalDx *= PUSH_SPEED;
                }
                m_Box->m_Transform.translation.x += finalDx;
                bool hitSomething = false;
                for (auto& s : m_Stones) {
                    if (IsColliding(m_Box, s)) {
                        hitSomething = true;
                        break;
                    }
                }

                if (sgear1 && IsColliding(m_Box, sgear1)) hitSomething = true;
                else if (sgear2 && IsColliding(m_Box, sgear2)) hitSomething = true;
                else if (sgear3 && IsColliding(m_Box, sgear3)) hitSomething = true;
                else if (sgear4 && IsColliding(m_Box, sgear4)) hitSomething = true;
                else if (sgear5 && IsColliding(m_Box, sgear5)) hitSomething = true;
                else if (b1gear1 && IsColliding(m_Box, b1gear1)) hitSomething = true;
                else if (b2gear1 && IsColliding(m_Box, b2gear1)) hitSomething = true;
                else if (b2gear2 && IsColliding(m_Box, b2gear2)) hitSomething = true;
                else if (b2gear3 && IsColliding(m_Box, b2gear3)) hitSomething = true;

                if (hitSomething) {
                    m_Box->m_Transform.translation.x -= finalDx;
                    if (isSandwich) m_Fire->m_Transform.translation.x -= finalDx;
                    iceDx = 0;
                } else {
                    iceDx = finalDx;
                    if (isSandwich) fireDx = finalDx;
                }
            }
        }

        if (fireDx != 0 && IsColliding(m_Fire, m_Box)) {
            float firePos = m_Fire->m_Transform.translation.x;
            float boxPos = m_Box->m_Transform.translation.x;
            bool isPushing = (firePos < boxPos && fireDx > 0) || (firePos > boxPos && fireDx < 0);
            if (isPushing) {
                float finalDx = (fireDx > 0 ? 1.0f : -1.0f);
                bool isSandwich = IsColliding(m_Box, m_Ice);
                if (isSandwich) {
                    finalDx *= SANDWICH_SPEED;
                    m_Ice->m_Transform.translation.x += finalDx;
                } else {
                    finalDx *= PUSH_SPEED;
                }
                m_Box->m_Transform.translation.x += finalDx;
                bool hitSomething = false;
                for (auto& s : m_Stones) {
                    if (IsColliding(m_Box, s)) {
                        hitSomething = true;
                        break;
                    }
                }

                if (sgear1 && IsColliding(m_Box, sgear1)) hitSomething = true;
                else if (sgear2 && IsColliding(m_Box, sgear2)) hitSomething = true;
                else if (sgear3 && IsColliding(m_Box, sgear3)) hitSomething = true;
                else if (sgear4 && IsColliding(m_Box, sgear4)) hitSomething = true;
                else if (sgear5 && IsColliding(m_Box, sgear5)) hitSomething = true;
                else if (b1gear1 && IsColliding(m_Box, b1gear1)) hitSomething = true;
                else if (b2gear1 && IsColliding(m_Box, b2gear1)) hitSomething = true;
                else if (b2gear2 && IsColliding(m_Box, b2gear2)) hitSomething = true;
                else if (b2gear3 && IsColliding(m_Box, b2gear3)) hitSomething = true;

                if (hitSomething) {
                    m_Box->m_Transform.translation.x -= finalDx;
                    if (isSandwich) m_Ice->m_Transform.translation.x -= finalDx;
                    fireDx = 0;
                } else {
                    fireDx = finalDx;
                    if (isSandwich) iceDx = finalDx;
                }
            }
        }
    };

    handleAdvancedPush();
    auto handleHorizontalObstacle = [&](std::shared_ptr<Util::GameObject> character, float& dx) {
        if (dx == 0) return;
        character->m_Transform.translation.x += dx; // 先移動
        
        bool hitGear = false;
        if (sgear1 && IsColliding(character, sgear1)) hitGear = true;
        else if (sgear2 && IsColliding(character, sgear2)) hitGear = true;
        else if (sgear3 && IsColliding(character, sgear3)) hitGear = true;
        else if (sgear4 && IsColliding(character, sgear4)) hitGear = true;
        else if (sgear5 && IsColliding(character, sgear5)) hitGear = true;
        else if (b1gear1 && IsColliding(character, b1gear1)) hitGear = true;
        else if (b2gear1 && IsColliding(character, b2gear1)) hitGear = true;
        else if (b2gear2 && IsColliding(character, b2gear2)) hitGear = true;
        else if (b2gear3 && IsColliding(character, b2gear3)) hitGear = true;

        if (hitGear) {
            character->m_Transform.translation.x -= dx; // 撞到機關才退回
            dx = 0;
        } 
        // 【修正】把原本不管怎樣都減回來的 else 整段刪除！沒撞到就不動它！
    };

    handleHorizontalObstacle(m_Ice, iceDx);
    handleHorizontalObstacle(m_Fire, fireDx);

    m_IceVelocityY -= m_Gravity;
    m_FireVelocityY -= m_Gravity;
    m_Ice->m_Transform.translation.y += m_IceVelocityY;
    m_Fire->m_Transform.translation.y += m_FireVelocityY;

    bool iG = false, fG = false;
    std::vector<std::shared_ptr<Util::GameObject>> collisionGroup = m_Stones;
    if (m_Box) collisionGroup.push_back(m_Box);
    
    if (sgear1) collisionGroup.push_back(sgear1);
    if (sgear2) collisionGroup.push_back(sgear2);
    if (sgear3) collisionGroup.push_back(sgear3);
    if (sgear4) collisionGroup.push_back(sgear4);
    if (sgear5) collisionGroup.push_back(sgear5);

    if (b1gear1) collisionGroup.push_back(b1gear1);

    if (b2gear1) collisionGroup.push_back(b2gear1);
    if (b2gear2) collisionGroup.push_back(b2gear2);
    if (b2gear3) collisionGroup.push_back(b2gear3);

    for (const auto& obj : collisionGroup) {
        float objW = obj->GetScaledSize().x;
        float objH = obj->GetScaledSize().y;


        float objLeft   = obj->m_Transform.translation.x - (objW / 2.0f);
        float objRight  = obj->m_Transform.translation.x + (objW / 2.0f);
        float objTop    = obj->m_Transform.translation.y + (objH / 2.0f);
        float objBottom = obj->m_Transform.translation.y - (objH / 2.0f);

        if (IsColliding(m_Ice, obj)) {
            float iceHalfW = m_Ice->GetScaledSize().x / 2.0f;
            float iceHalfH = m_Ice->GetScaledSize().y / 2.0f;
            float iceLeft = m_Ice->m_Transform.translation.x - iceHalfW;
            float iceRight = m_Ice->m_Transform.translation.x + iceHalfW;
            float iceTop = m_Ice->m_Transform.translation.y + iceHalfH;
            float iceBottom = m_Ice->m_Transform.translation.y - iceHalfH;

            float overlapLeft = iceRight - objLeft;
            float overlapRight = objRight - iceLeft;
            float overlapTop = iceTop - objBottom;
            float overlapBottom = objTop - iceBottom;

            float minOverlapX = std::min(overlapLeft, overlapRight);
            float minOverlapY = std::min(overlapTop, overlapBottom);

            if (minOverlapX < minOverlapY) {
                if (overlapLeft < overlapRight) {
                    m_Ice->m_Transform.translation.x = objLeft - iceHalfW;
                } else {
                    m_Ice->m_Transform.translation.x = objRight + iceHalfW;
                }
            } else {
                if (m_IceVelocityY <= 0 && m_Ice->m_Transform.translation.y > obj->m_Transform.translation.y) {
                    m_IceVelocityY = 0;
                    m_Ice->m_Transform.translation.y = objTop + iceHalfH;
                    iG = true;
                } else if (m_IceVelocityY > 0 && m_Ice->m_Transform.translation.y < obj->m_Transform.translation.y) {
                    m_IceVelocityY = 0;
                    m_Ice->m_Transform.translation.y = objBottom - iceHalfH;
                }
            }
        }
        if (IsColliding(m_Fire, obj)) {
            float fireHalfW = m_Fire->GetScaledSize().x / 2.0f;
            float fireHalfH = m_Fire->GetScaledSize().y / 2.0f;
            float fireLeft = m_Fire->m_Transform.translation.x - fireHalfW;
            float fireRight = m_Fire->m_Transform.translation.x + fireHalfW;
            float fireTop = m_Fire->m_Transform.translation.y + fireHalfH;
            float fireBottom = m_Fire->m_Transform.translation.y - fireHalfH;

            float overlapLeft = fireRight - objLeft;
            float overlapRight = objRight - fireLeft;
            float overlapTop = fireTop - objBottom;
            float overlapBottom = objTop - fireBottom;

            float minOverlapX = std::min(overlapLeft, overlapRight);
            float minOverlapY = std::min(overlapTop, overlapBottom);

            if (minOverlapX < minOverlapY) {
                if (overlapLeft < overlapRight) {
                    m_Fire->m_Transform.translation.x = objLeft - fireHalfW;
                } else {
                    m_Fire->m_Transform.translation.x = objRight + fireHalfW;
                }
            } else {
                if (m_FireVelocityY <= 0 && m_Fire->m_Transform.translation.y > obj->m_Transform.translation.y) {
                    m_FireVelocityY = 0;
                    m_Fire->m_Transform.translation.y = objTop + fireHalfH;
                    fG = true;
                } else if (m_FireVelocityY > 0 && m_Fire->m_Transform.translation.y < obj->m_Transform.translation.y) {
                    m_FireVelocityY = 0;
                    m_Fire->m_Transform.translation.y = objBottom - fireHalfH;
                }
            }
        }
    }

    // ===== ChainPlatform 碰撞 =====
    if (m_ChainPlatform) {
        m_ChainPlatform->BeginFrame();

        glm::vec2 icePos = m_Ice->m_Transform.translation;
        glm::vec2 firePos = m_Fire->m_Transform.translation;

        bool iceOnChain = m_ChainPlatform->CheckCollisionWithPlayer(
            m_Ice->m_Transform.translation,
            icePos,
            m_Ice->GetScaledSize(),
            m_IceVelocityY
        );

        bool fireOnChain = m_ChainPlatform->CheckCollisionWithPlayer(
            m_Fire->m_Transform.translation,
            firePos,
            m_Fire->GetScaledSize(),
            m_FireVelocityY
        );

        m_Ice->m_Transform.translation = icePos;
        m_Fire->m_Transform.translation = firePos;

        if (iceOnChain) {
            iG = true;
        }

        if (fireOnChain) {
            fG = true;
        }

        m_ChainPlatform->Update(0.0166f);
    }

    if (m_ChainPlatform2) {
        m_ChainPlatform2->BeginFrame();

        glm::vec2 icePos = m_Ice->m_Transform.translation;
        glm::vec2 firePos = m_Fire->m_Transform.translation;

        bool iceOnChain = m_ChainPlatform2->CheckCollisionWithPlayer(
            m_Ice->m_Transform.translation,
            icePos,
            m_Ice->GetScaledSize(),
            m_IceVelocityY
        );

        bool fireOnChain = m_ChainPlatform2->CheckCollisionWithPlayer(
            m_Fire->m_Transform.translation,
            firePos,
            m_Fire->GetScaledSize(),
            m_FireVelocityY
        );

        m_Ice->m_Transform.translation = icePos;
        m_Fire->m_Transform.translation = firePos;

        if (iceOnChain) {
            iG = true;
        }

        if (fireOnChain) {
            fG = true;
        }

        m_ChainPlatform2->Update(0.0166f);
    }

    // ===== 比重量平台：冰人、火人 =====
    if (m_BalanceRopePlatform) {
        m_BalanceRopePlatform->BeginFrame();

        glm::vec2 icePos = m_Ice->m_Transform.translation;
        glm::vec2 firePos = m_Fire->m_Transform.translation;

        bool iceOnBalance = m_BalanceRopePlatform->CheckCollisionWithObject(
            icePos,
            m_Ice->GetScaledSize(),
            m_IceVelocityY,
            12.0f
        );

        bool fireOnBalance = m_BalanceRopePlatform->CheckCollisionWithObject(
            firePos,
            m_Fire->GetScaledSize(),
            m_FireVelocityY,
            12.0f
        );

        m_Ice->m_Transform.translation = icePos;
        m_Fire->m_Transform.translation = firePos;

        if (iceOnBalance) {
            iG = true;
        }

        if (fireOnBalance) {
            fG = true;
        }
    }


    m_IceOnGround = iG;
    m_FireOnGround = fG;

    ApplySlopeToPlayer(m_Ice, m_IceVelocityY, m_IceOnGround, iceDx);
    ApplySlopeToPlayer(m_Fire, m_FireVelocityY, m_FireOnGround, fireDx);

    // 箱子垂直碰撞邏輯
    std::vector<std::shared_ptr<Util::GameObject>> boxCollisionGroup = m_Stones;

    if (sgear1) boxCollisionGroup.push_back(sgear1);
    if (sgear2) boxCollisionGroup.push_back(sgear2);
    if (sgear3) boxCollisionGroup.push_back(sgear3);
    if (sgear4) boxCollisionGroup.push_back(sgear4);
    if (sgear5) boxCollisionGroup.push_back(sgear5);

    if (b1gear1) boxCollisionGroup.push_back(b1gear1);

    if (b2gear1) boxCollisionGroup.push_back(b2gear1);
    if (b2gear2) boxCollisionGroup.push_back(b2gear2);
    if (b2gear3) boxCollisionGroup.push_back(b2gear3);

    if (m_Box) {
        m_BoxOnGround = false;
        m_BoxVelocityY -= m_Gravity;
        m_Box->m_Transform.translation.y += m_BoxVelocityY;
        for (const auto &obj: boxCollisionGroup) {
            if (!obj) continue;
            if (IsColliding(m_Box, obj)) {
                float objTop = obj->m_Transform.translation.y + obj->GetScaledSize().y / 2.0f;
                float objBottom = obj->m_Transform.translation.y - obj->GetScaledSize().y / 2.0f;
                float boxHalfH = m_Box->GetScaledSize().y / 2.0f;
                if (m_BoxVelocityY <= 0.0f && m_Box->m_Transform.translation.y > obj->m_Transform.translation.y) {
                    m_Box->m_Transform.translation.y = objTop + boxHalfH;
                    m_BoxVelocityY = 0.0f;
                    m_BoxOnGround = true;
                } else if (m_BoxVelocityY > 0.0f && m_Box->m_Transform.translation.y < obj->m_Transform.translation.y) {
                    m_Box->m_Transform.translation.y = objBottom - boxHalfH;
                    m_BoxVelocityY = 0.0f;
                }
            }
        }
        // ===== 箱子也可以壓比重量平台 =====
        if (m_BalanceRopePlatform) {
            glm::vec2 boxPos = m_Box->m_Transform.translation;

            bool boxOnBalance = m_BalanceRopePlatform->CheckCollisionWithObject(
                boxPos,
                m_Box->GetScaledSize(),
                m_BoxVelocityY,
                24.0f
            );

            m_Box->m_Transform.translation = boxPos;

            if (boxOnBalance) {
                m_BoxOnGround = true;
            }
        }
    }

    if (m_BalanceRopePlatform) {
        m_BalanceRopePlatform->Update(0.0166f);
    }

    auto isStandingOnTop = [&](std::shared_ptr<Util::GameObject> character, std::shared_ptr<Util::GameObject> platform) {
        if (!character || !platform) return false;
        float charHalfW = character->GetScaledSize().x / 2.0f;
        float charHalfH = character->GetScaledSize().y / 2.0f;
        float platHalfW = platform->GetScaledSize().x / 2.0f;
        float platHalfH = platform->GetScaledSize().y / 2.0f;

        float charLeft = character->m_Transform.translation.x - charHalfW;
        float charRight = character->m_Transform.translation.x + charHalfW;
        float charBottom = character->m_Transform.translation.y - charHalfH;

        float platLeft = platform->m_Transform.translation.x - platHalfW;
        float platRight = platform->m_Transform.translation.x + platHalfW;
        float platTop = platform->m_Transform.translation.y + platHalfH;

        return (charRight > platLeft) && (charLeft < platRight) && std::abs(charBottom - platTop) < 5.0f;
    };

    // ===== 1. 按鈕狀態更新 =====
    auto checkButton = [&](std::shared_ptr<Util::GameObject> btn) -> bool {
        if (!btn) return false;
        bool pressed = IsColliding(m_Ice, btn) || IsColliding(m_Fire, btn) || (m_Box && IsColliding(m_Box, btn));
        btn->SetVisible(!pressed);
        return pressed;
    };

    bool btn1Pressed = checkButton(button1);
    bool btn2_1_1Pressed = checkButton(button2_1_1);
    bool btn2_1_2Pressed = checkButton(button2_1_2);
    bool btn2_2_1Pressed = checkButton(button2_2_1);
    bool btn2_2_2Pressed = checkButton(button2_2_2);
    bool btn2_3_1Pressed = checkButton(button2_3_1);
    bool btn2_3_2Pressed = checkButton(button2_3_2);


    // ===== 2. 拉桿狀態更新 =====
    auto handleSingleSwitch = [&](std::shared_ptr<Util::GameObject> character, float dx, bool isIce, std::shared_ptr<Util::GameObject> sw, bool& swState) {
        if (!sw || !character) return;
        if (IsColliding(character, sw)) {
            float charX = character->m_Transform.translation.x;
            float swX = sw->m_Transform.translation.x;
            bool pushingRight = isIce ? keys[SDL_SCANCODE_D] : keys[SDL_SCANCODE_RIGHT];
            bool pushingLeft = isIce ? keys[SDL_SCANCODE_A] : keys[SDL_SCANCODE_LEFT];

            if (charX < swX && dx > 0 && pushingRight && swState) {
                swState = false;
                sw->SetDrawable(std::make_shared<Util::Image>(PIC_PATH + "switch1_1.png"));
            } else if (charX > swX && dx < 0 && pushingLeft && !swState) {
                swState = true;
                sw->SetDrawable(std::make_shared<Util::Image>(PIC_PATH + "switch1_2.png"));
            }
        }
    };

    handleSingleSwitch(m_Ice, iceDx, true, switch1, switch1State);
    handleSingleSwitch(m_Fire, fireDx, false, switch1, switch1State);
    handleSingleSwitch(m_Ice, iceDx, true, switch2, switch2State);
    handleSingleSwitch(m_Fire, fireDx, false, switch2, switch2State);
    handleSingleSwitch(m_Ice, iceDx, true, switch3, switch3State);
    handleSingleSwitch(m_Fire, fireDx, false, switch3, switch3State);
    handleSingleSwitch(m_Ice, iceDx, true, switch4, switch4State);
    handleSingleSwitch(m_Fire, fireDx, false, switch4, switch4State);
    handleSingleSwitch(m_Ice, iceDx, true, switch5, switch5State);
    handleSingleSwitch(m_Fire, fireDx, false, switch5, switch5State);


    // ===== 3. 機關位移與角色同步 =====
    auto moveGear = [&](std::shared_ptr<Util::GameObject> gear, glm::vec2 targetPos) {
        if (!gear) return;
        glm::vec2 oldPos = gear->m_Transform.translation;
        bool iceOn = isStandingOnTop(m_Ice, gear);
        bool fireOn = isStandingOnTop(m_Fire, gear);

        float speed = 2.0f;
        if (gear->m_Transform.translation.x < targetPos.x) {
            gear->m_Transform.translation.x = std::min(gear->m_Transform.translation.x + speed, targetPos.x);
        } else if (gear->m_Transform.translation.x > targetPos.x) {
            gear->m_Transform.translation.x = std::max(gear->m_Transform.translation.x - speed, targetPos.x);
        }

        if (gear->m_Transform.translation.y < targetPos.y) {
            gear->m_Transform.translation.y = std::min(gear->m_Transform.translation.y + speed, targetPos.y);
        } else if (gear->m_Transform.translation.y > targetPos.y) {
            gear->m_Transform.translation.y = std::max(gear->m_Transform.translation.y - speed, targetPos.y);
        }

        glm::vec2 delta = gear->m_Transform.translation - oldPos;
        if (delta.x != 0.0f || delta.y != 0.0f) {
            if (iceOn) {
                m_Ice->m_Transform.translation += delta;
                m_IceVelocityY = 0.0f;
                m_IceOnGround = true;
            }
            if (fireOn) {
                m_Fire->m_Transform.translation += delta;
                m_FireVelocityY = 0.0f;
                m_FireOnGround = true;
            }
        }
    };

    glm::vec2 target;

    if (sgear1) {
        target = sgear1OriginalPos;
        if (switch1State) target.y -= 75.0f;
        moveGear(sgear1, target);
    }

    if (b2gear1) {
        target = b2gear1OriginalPos;
        if (btn2_1_1Pressed || btn2_1_2Pressed) target.y -= 75.0f;
        moveGear(b2gear1, target);
    }

    if (b2gear2) {
        target = b2gear2OriginalPos;
        if (btn2_2_1Pressed || btn2_2_2Pressed) target.y -= 100.0f;
        moveGear(b2gear2, target);
    }

    if (b2gear3) {
        target = b2gear3OriginalPos;
        if (btn2_3_1Pressed || btn2_3_2Pressed) target.x -= 92.0f;
        moveGear(b2gear3, target);
    }

    if (sgear2) {
        target = sgear2OriginalPos;
        if (switch2State) target.y -= 75.0f;
        moveGear(sgear2, target);
    }

    if (b1gear1) {
        target = b1gear1OriginalPos;
        if (btn1Pressed) target.y -= 75.0f;
        moveGear(b1gear1, target);
    }

    if (sgear3) {
        target = sgear3OriginalPos;
        if (switch3State) target.x += 75.0f;
        moveGear(sgear3, target);
    }

    if (sgear4) {
        target = sgear4OriginalPos;
        if (switch4State) target.x -= 75.0f;
        moveGear(sgear4, target);
    }

    if (sgear5) {
        target = sgear5OriginalPos;
        if (switch5State) target.x += 75.0f;
        moveGear(sgear5, target);
    }
    // 9. 門的動畫處理與關卡切換
    m_IceDoorOpening = IsColliding(m_Ice, m_IceDoor);
    m_FireDoorOpening = IsColliding(m_Fire, m_FireDoor);
    m_DoorAnimCounter++;

    if (m_DoorAnimCounter >= m_DoorAnimSpeed) {
        m_DoorAnimCounter = 0;
        if (m_IceDoorOpening) {
            if (m_IceDoorFrameIndex < (int)m_IceDoorFrames.size() - 1) m_IceDoorFrameIndex++;
        } else {
            if (m_IceDoorFrameIndex > 0) m_IceDoorFrameIndex--;
        }
        m_IceDoor->SetDrawable(std::make_shared<Util::Image>(m_IceDoorFrames[m_IceDoorFrameIndex]));

        if (m_FireDoorOpening) {
            if (m_FireDoorFrameIndex < (int)m_FireDoorFrames.size() - 1) m_FireDoorFrameIndex++;
        } else {
            if (m_FireDoorFrameIndex > 0) m_FireDoorFrameIndex--;
        }
        m_FireDoor->SetDrawable(std::make_shared<Util::Image>(m_FireDoorFrames[m_FireDoorFrameIndex]));
    }

    // 檢查通關條件
    if (m_IceDoorFrameIndex == (int)m_IceDoorFrames.size() - 1 &&
        m_FireDoorFrameIndex == (int)m_FireDoorFrames.size() - 1 &&
        IsColliding(m_Ice, m_IceDoor) && IsColliding(m_Fire, m_FireDoor)) {
            m_CurrentLevelNum ++;
            LoadLevel(m_CurrentLevelNum);
            return;
        }

    // 10. 陷阱與死亡判定
    bool iceDead = false, fireDead = false;
    if (!ischeatingmode){
        for (const auto& trap : m_Traps) {
            if (IsColliding(m_Ice, trap)) { iceDead = true; break; }
        }
        if (!iceDead) {
            for (const auto& trap : m_IceTraps) {
                if (IsColliding(m_Ice, trap)) { iceDead = true; break; }
            }
        }

        for (const auto& trap : m_Traps) {
            if (IsColliding(m_Fire, trap)) { fireDead = true; break; }
        }
        if (!fireDead) {
            for (const auto& trap : m_FireTraps) {
                if (IsColliding(m_Fire, trap)) { fireDead = true; break; }
            }
        }
    }
    if (iceDead || fireDead) {
        m_CurrentState = State::DEAD;
        m_DeadScreen->SetVisible(true);
    }
}