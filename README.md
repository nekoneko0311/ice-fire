# Ice-fire

## Start

1. Clone the game

   ```bash
   git clone git@github.com:nekoneko0311/ice-fire.git --recursive
   ```

2. Build

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
