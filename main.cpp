#include <windows.h>
#include <vector>
#include <random>
#include <ctime>

const char g_szClassName[] = "myWindowClass";

// Button IDs
#define ID_BUTTON_START 1001
#define ID_BUTTON_RESTART 1002
#define ID_BUTTON_GAME_BASE 2000

// Game constants
const int BOARD_SIZE = 10;
const int MINES_COUNT = 10;
const int CELL_SIZE = 30;
const int BOARD_OFFSET_X = 50;
const int BOARD_OFFSET_Y = 100;

// Game state
enum GameState {
    MENU,
    PLAYING,
    GAME_OVER_WIN,
    GAME_OVER_LOSE
};

struct GameData {
    std::vector<std::vector<int>> board;  // -1 = mine, 0-8 = adjacent mines
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;
    GameState state;
    int revealedCount;
    HWND hStartButton;
    HWND hRestartButton;
    std::vector<std::vector<HWND>> gameButtons;
    
    GameData() : board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0)),
                 revealed(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false)),
                 flagged(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false)),
                 state(MENU), revealedCount(0) {}
};

GameData g_gameData;

// Function declarations
void InitializeGame();
void PlaceMines();
void CalculateNumbers();
void RevealCell(int row, int col);
void GameOver(bool won);
void ShowGameBoard();
void HideGameBoard();
void ShowMenu();
void ShowRestartButton();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            // Create start button
            g_gameData.hStartButton = CreateWindow(
                "BUTTON", "Start Game",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                100, 100, 100, 30,
                hwnd, (HMENU)(UINT_PTR)ID_BUTTON_START, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            // Create restart button (initially hidden)
            g_gameData.hRestartButton = CreateWindow(
                "BUTTON", "Restart",
                WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
                100, 100, 100, 30,
                hwnd, (HMENU)(UINT_PTR)ID_BUTTON_RESTART, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            // Initialize game buttons grid
            g_gameData.gameButtons.resize(BOARD_SIZE, std::vector<HWND>(BOARD_SIZE));
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    g_gameData.gameButtons[i][j] = CreateWindow(
                        "BUTTON", "",
                        WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON,
                        BOARD_OFFSET_X + j * CELL_SIZE, BOARD_OFFSET_Y + i * CELL_SIZE,
                        CELL_SIZE, CELL_SIZE,
                        hwnd, (HMENU)(UINT_PTR)(ID_BUTTON_GAME_BASE + i * BOARD_SIZE + j),
                        ((LPCREATESTRUCT)lParam)->hInstance, NULL);
                }
            }
            break;
        }
        
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            
            if (wmId == ID_BUTTON_START) {
                InitializeGame();
                ShowGameBoard();
                g_gameData.state = PLAYING;
                ShowWindow(g_gameData.hStartButton, SW_HIDE);
            }
            else if (wmId == ID_BUTTON_RESTART) {
                g_gameData.state = MENU;
                ShowMenu();
                HideGameBoard();
                ShowWindow(g_gameData.hRestartButton, SW_HIDE);
            }
            else if (wmId >= ID_BUTTON_GAME_BASE && wmId < ID_BUTTON_GAME_BASE + BOARD_SIZE * BOARD_SIZE) {
                if (g_gameData.state == PLAYING) {
                    int buttonId = wmId - ID_BUTTON_GAME_BASE;
                    int row = buttonId / BOARD_SIZE;
                    int col = buttonId % BOARD_SIZE;
                    
                    // Right click to flag (simulated with Ctrl+click)
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        if (!g_gameData.revealed[row][col]) {
                            g_gameData.flagged[row][col] = !g_gameData.flagged[row][col];
                            SetWindowText(g_gameData.gameButtons[row][col], 
                                        g_gameData.flagged[row][col] ? "🚩" : "");
                        }
                    }
                    // Left click to reveal
                    else {
                        if (!g_gameData.revealed[row][col] && !g_gameData.flagged[row][col]) {
                            RevealCell(row, col);
                        }
                    }
                }
            }
            break;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void InitializeGame()
{
    // Reset game data
    g_gameData.board = std::vector<std::vector<int>>(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    g_gameData.revealed = std::vector<std::vector<bool>>(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
    g_gameData.flagged = std::vector<std::vector<bool>>(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
    g_gameData.revealedCount = 0;
    
    // Clear all buttons and re-enable them
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            SetWindowText(g_gameData.gameButtons[i][j], "");
            EnableWindow(g_gameData.gameButtons[i][j], TRUE);
        }
    }
    
    PlaceMines();
    CalculateNumbers();
}

void PlaceMines()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, BOARD_SIZE - 1);
    
    int minesPlaced = 0;
    while (minesPlaced < MINES_COUNT) {
        int row = dis(gen);
        int col = dis(gen);
        
        if (g_gameData.board[row][col] != -1) {
            g_gameData.board[row][col] = -1;
            minesPlaced++;
        }
    }
}

void CalculateNumbers()
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (g_gameData.board[i][j] != -1) {
                int count = 0;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < BOARD_SIZE && nj >= 0 && nj < BOARD_SIZE) {
                            if (g_gameData.board[ni][nj] == -1) count++;
                        }
                    }
                }
                g_gameData.board[i][j] = count;
            }
        }
    }
}

void RevealCell(int row, int col)
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    if (g_gameData.revealed[row][col] || g_gameData.flagged[row][col]) return;
    
    g_gameData.revealed[row][col] = true;
    g_gameData.revealedCount++;
    
    // Disable the button to prevent further clicks
    EnableWindow(g_gameData.gameButtons[row][col], FALSE);
    
    if (g_gameData.board[row][col] == -1) {
        // Hit a mine
        SetWindowText(g_gameData.gameButtons[row][col], "💣");
        GameOver(false);
        return;
    }
    
    if (g_gameData.board[row][col] == 0) {
        // Empty cell, reveal neighbors
        SetWindowText(g_gameData.gameButtons[row][col], "");
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                RevealCell(row + di, col + dj);
            }
        }
    } else {
        // Number cell
        char numStr[2];
        sprintf_s(numStr, "%d", g_gameData.board[row][col]);
        SetWindowText(g_gameData.gameButtons[row][col], numStr);
    }
    
    // Check for win
    if (g_gameData.revealedCount == BOARD_SIZE * BOARD_SIZE - MINES_COUNT) {
        GameOver(true);
    }
}

void GameOver(bool won)
{
    g_gameData.state = won ? GAME_OVER_WIN : GAME_OVER_LOSE;
    
    // Reveal all mines and disable all buttons
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (g_gameData.board[i][j] == -1) {
                SetWindowText(g_gameData.gameButtons[i][j], "💣");
            }
            // Disable all buttons to prevent further interaction
            EnableWindow(g_gameData.gameButtons[i][j], FALSE);
        }
    }
    
    ShowRestartButton();
}

void ShowGameBoard()
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            ShowWindow(g_gameData.gameButtons[i][j], SW_SHOW);
        }
    }
}

void HideGameBoard()
{
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            ShowWindow(g_gameData.gameButtons[i][j], SW_HIDE);
        }
    }
}

void ShowMenu()
{
    ShowWindow(g_gameData.hStartButton, SW_SHOW);
}

void ShowRestartButton()
{
    ShowWindow(g_gameData.hRestartButton, SW_SHOW);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0,
        g_szClassName,
        "Minesweeper",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 500,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return static_cast<int>(Msg.wParam);
} 