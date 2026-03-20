#include "App.hpp"
#include "Core/Context.hpp"

int main(int, char**) {
    auto context = Core::Context::GetInstance();
    App app;

    while (!context->GetExit()) {
        switch (app.GetCurrentState()) {
            case App::State::START:
                app.Start();
                break;

            case App::State::UPDATE:
                app.Update(); // 正常遊戲更新
                break;

            case App::State::PAUSE:
                app.Update(); // 暫停時也要進入 Update 偵測 ESC 鍵切換回來
                break;
            case App::State::DEAD:
                app.Update(); // 死亡狀態也要進入 Update 偵測 R 鍵重開
                break;

            case App::State::END:
                app.End();
                context->SetExit(true);
                break;
            default:
                break;
        }
        context->Update(); // 讓 SDL 處理視窗事件 (如 Alt+F4)
    }
    return 0;
}