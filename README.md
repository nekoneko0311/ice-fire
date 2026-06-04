# PTSD Template

This is a [PTSD](https://github.com/ntut-open-source-club/practical-tools-for-simple-design) framework template for students taking OOPL2024s.

## Quick Start

1. Use this template to create a new repository
   ![github screenshot](https://github.com/ntut-rick/ptsd-template/assets/126899559/ef62242f-03ed-481d-b858-12b730c09beb)

2. Clone your repository

   ```bash
   git clone YOUR_GIT_URL --recursive
   ```

3. Build your project

  > [!WARNING]
  > Please build your project in `Debug` because our `Release` path is broken D:
   
   ```sh
   cmake -DCMAKE_BUILD_TYPE=Debug -B build # -G Ninja
   ```
   better read [PTSD README](https://github.com/ntut-open-source-club/practical-tools-for-simple-design)

## 操作教學

1. 使用"WAD"、"⭠⭡⭢"進行基礎操作
2. 使用"N"以跳關
3. 使用"C"進入作弊模式(不會死，但不能飛)
4. 使用"Escape"暫停/繼續

## Notice

1. 如果地圖邊界沒有緊貼視窗大小，請在[PTSD/include/config.hpp]中調整成897*667
2. 如果無法操作，請切換為英文輸入法。
