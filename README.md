# Minesweeper

一個使用 Win32 API 開發的踩地雷遊戲。

## 遊戲特色

- **10x10 遊戲板**：經典的踩地雷遊戲大小
- **10 個地雷**：隨機分布在遊戲板上
- **開始選單**：點擊 "Start Game" 開始遊戲
- **重新開始**：遊戲結束後可點擊 "Restart" 重新遊玩
- **旗幟標記**：按住 Ctrl 鍵點擊可標記地雷位置

## 遊戲規則

1. 點擊 "Start Game" 開始遊戲
2. 左鍵點擊格子來揭開
3. 按住 Ctrl 鍵並點擊格子來標記/取消標記地雷
4. 數字表示周圍 8 個格子中地雷的數量
5. 避開所有地雷並揭開所有安全格子即可獲勝
6. 踩到地雷遊戲結束

## Build 與執行方式（Windows）

### 需求
- CMake 3.10 以上
- Visual Studio 2017 以上 或 MSVC 編譯器

### Build 步驟
1. 開啟命令提示字元或PowerShell，切換到本專案目錄：
   ```
   cd Minesweeper
   ```
2. 建立 build 資料夾並進入：
   ```
   mkdir build
   cd build
   ```
3. 產生 Visual Studio 專案檔：
   ```
   cmake .. -G "Visual Studio 17 2022"
   ```
   （或根據你的 Visual Studio 版本調整）
4. 使用 Visual Studio 開啟 `Minesweeper.sln`，或直接在命令列編譯：
   ```
   cmake --build . --config Release
   ```
5. 執行產生的 EXE 檔案：
   ```
   .\Release\Minesweeper.exe
   ```

## 操作說明

- **開始遊戲**：點擊 "Start Game" 按鈕
- **揭開格子**：左鍵點擊遊戲格子
- **標記地雷**：按住 Ctrl 鍵並點擊格子
- **重新開始**：遊戲結束後點擊 "Restart" 按鈕

執行後會開啟遊戲視窗，點擊 "Start Game" 開始遊玩！ 