// TicTacToe.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TicTacToe.h"
#include <windowsx.h>
#include <string>

#define MAX_LOADSTRING 100
#define WINSTARTX 600 // Window starting x position
#define WINSTARTY 200 // Window starting y position
#define WINHEIGHT 550 // Window height
#define WINWIDTH 650 // Window width

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        WINSTARTX, WINSTARTY, WINWIDTH, WINHEIGHT, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

// Game constants and variables
#define P1COLOR RGB(0, 0, 0)
#define P2COLOR RGB(64, 224, 208)
#define SCORECOLOR RGB(0, 0, 0)

const int CELL_SIZE = 100;
HBRUSH hbr1, hbr2;
HFONT hFont;
HICON hIcon1, hIcon2;
int playerTurn = 1; 
int gameBoard[9] = { 0 };
int winner = 0;
int wins[3];   // For storing winning line positions
int score[2] = { 0, 0 };  // [0] = player 1 score, [2] player 2 score
int gameCount = 1;

BOOL GetGameBoardRect(HWND hwnd, RECT* pRect)
{
    RECT rc;

    if (GetClientRect(hwnd, &rc))
    {
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        pRect->left = (width - CELL_SIZE * 3) / 2;
        pRect->top = (height - CELL_SIZE * 3) / 2 + 20;
        pRect->right = pRect->left + CELL_SIZE * 3;
        pRect->bottom = pRect->top + CELL_SIZE * 3;

        return TRUE;
    }

    SetRectEmpty(pRect);
    return FALSE;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL);
    LineTo(hdc, x2, y2);
}

int GetCellNumberFromPoint(HWND hwnd, int x, int y)
{
    POINT pt = { x, y };
    RECT rc;

    if (GetGameBoardRect(hwnd, &rc)) {
        if (PtInRect(&rc, pt))
        {
            // User clicked inside board
            // Normalize ( 0 to 3 * CELL_SIZE)
            x = pt.x - rc.left;
            y = pt.y - rc.top;
           
            int column = x / CELL_SIZE;
            int row = y / CELL_SIZE;

            // Convert to index ( 0 to 8 )
            return column + row * 3;
        }
    }
    return -1; // Click outside the board
}

BOOL GetCellRect(HWND hWnd, int index, RECT* pRect)
{
    RECT rcBoard;

    SetRectEmpty(pRect);

    if (index < 0 || index > 8)
    {
        return FALSE;
    }

    if (GetGameBoardRect(hWnd, &rcBoard))
    {
        // Convert index from 0 to 8 into x,y pair
        int x = index % 3; // Column number
        int y = index / 3; // Row number

        pRect->left = rcBoard.left + x * CELL_SIZE + 1;
        pRect->top = rcBoard.top + y * CELL_SIZE + 1;
        pRect->right = pRect->left + CELL_SIZE - 1;
        pRect->bottom = pRect->top + CELL_SIZE - 1;

        return TRUE;
    }

    return FALSE;
}

/*
    Returns:
    0 - No winner
    1 - Player wins
    2 - Player wins
    3 - Draw
*/
int GetWinner(int wins[])
{
    // All combinations to check
    int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7, 2,5,8, 0,4,8, 2,4,6 };

    for (int i = 0; i < ARRAYSIZE(cells); i += 3)
    {
        if (gameBoard[cells[i]] != 0 && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]])
        {
            // There is a winner
            wins[0] = cells[i];
            wins[1] = cells[i + 1];
            wins[2] = cells[i + 2];

            return gameBoard[cells[i]];
        }
    }

    // Check for empty cells, so we know is it a draw or not
    for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
    {
        if (gameBoard[i] == 0)
        {
            return 0;
        }
    }
    // Now we know it's a draw
    return 3;
}

void ShowTurn(HWND hwnd, HDC hdc)
{
    RECT rc;
    const WCHAR szTurn1[] = L"Turn: Player 1";
    const WCHAR szTurn2[] = L"Turn: Player 2";

    const WCHAR* pszTurnText = NULL;

    switch (winner)
    {
    case 0: // Continue to play
        pszTurnText = (playerTurn == 1) ? szTurn1 : szTurn2;
        break;
    case 1: // Player 1 wins
        pszTurnText = L"Player 1 is the winner!";
        break;
    case 2: // Player 2 wins
        pszTurnText = L"Player 2 is the winner!";
        break;
    case 3: // Draw
        pszTurnText = L"It's a draw!";
        break;
    }

    if (GetClientRect(hwnd, &rc) && pszTurnText != NULL)
    {
        rc.top = rc.bottom - 65;
        FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
        SetTextColor(hdc, (playerTurn == 1 || winner == 1) ? P1COLOR : P2COLOR);
        SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
    }
}

void DrawIconCentered(HDC hdc, RECT* pRect, HICON hIcon)
{
    const int ICON_WIDTH = GetSystemMetrics(SM_CXICON);
    const int ICON_HEIGHT = GetSystemMetrics(SM_CYICON);

    if (pRect != NULL)
    {
        int left = pRect->left + ((pRect->right - pRect->left) - ICON_WIDTH) / 2 - 8;
        int top = pRect->top + ((pRect->bottom - pRect->top) - ICON_HEIGHT) / 2 - 8;

        DrawIconEx(hdc, left, top, hIcon, 50, 50, 0, NULL, DI_NORMAL);
    }
}

void ShowWinningLine(HWND hwnd, HDC hdc)
{
    RECT rcWin;
    HBRUSH winLine = CreateSolidBrush(RGB(64, 64, 64));

    for (int i = 0; i < 3; ++i)
    {
        if (GetCellRect(hwnd, wins[i], &rcWin))
        {
            //FillRect(hdc, &rcWin, (winner == 1) ? hbr1 : hbr2);  // Color winning line in player color
            FillRect(hdc, &rcWin, winLine);
            DrawIconCentered(hdc, &rcWin, (winner == 1) ? hIcon1 : hIcon2);
        }
    }
}

void ChangeFont(HDC& hdc, const int size, const WCHAR* font)
{
    LOGFONT logFont;

    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logFont);
    logFont.lfHeight = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    logFont.lfWidth = 0;
    logFont.lfWeight = FW_LIGHT;
    lstrcpy((LPWSTR)&logFont.lfFaceName, font);

    hFont = CreateFontIndirect(&logFont);
    SelectObject(hdc, hFont);
}

void NewGameMsgBox(HWND& hWnd)
{
    int ret = MessageBox(hWnd, L"Do you want to start a new game?", L"New Game", MB_YESNO | MB_ICONQUESTION);

    if (IDYES == ret)
    {
        // Reset and start a new game
        playerTurn = 1;
        winner = 0;
        gameCount++;
        ZeroMemory(gameBoard, sizeof(gameBoard));
        // Force a paint message
        InvalidateRect(hWnd, NULL, TRUE); // Post WM_PAINT to out windowProc. It get queued in out msg queue
        UpdateWindow(hWnd); // Forces immediate handling of WM_PAINT
    }
}

void DisplayPlayerText(HDC hdc, const int txtx, const int txty, const int iconx, const int icony, const WCHAR* text)
{
    int ret = wcscmp(text, L"Player 1");

    SetTextColor(hdc, (ret == 0) ? P1COLOR : P2COLOR);
    TextOut(hdc, txtx, txty, text, wcslen(text));
    DrawIconEx(hdc, iconx, icony, (ret == 0) ? hIcon1 : hIcon2, 35, 35, 0, NULL, DI_NORMAL);
}

void DisplayGameNumberAndScore(HDC hdc, RECT rcClient, const WCHAR* font)
{
    const WCHAR gmText[] = L"GAME ";
    wchar_t gmCount[50];
    wchar_t scPlayer1[100];
    wchar_t scPlayer2[100];
    _itow_s(gameCount, gmCount, 10);
    _itow_s(score[0], scPlayer1, 10);
    _itow_s(score[1], scPlayer2, 10);


    SetTextColor(hdc, SCORECOLOR);
    ChangeFont(hdc, 28, font);
    TextOut(hdc, ((rcClient.left + rcClient.right) / 2) - 55, 15, gmText, ARRAYSIZE(gmText));
    TextOut(hdc, ((rcClient.left + rcClient.right) / 2) + 40, 15, gmCount, wcslen(gmCount));
    TextOut(hdc, ((rcClient.left + rcClient.right) / 2) - (20 + 12 * wcslen(scPlayer1)), 60, scPlayer1, wcslen(scPlayer1));
    TextOut(hdc, ((rcClient.left + rcClient.right) / 2) - 1, 56, L":", 1);
    TextOut(hdc, ((rcClient.left + rcClient.right) / 2) + (7 + 12 * wcslen(scPlayer2)), 60, scPlayer2, wcslen(scPlayer2));

}

void ResultMsgBox(HWND hWnd, const WCHAR* text, const WCHAR* boxTitle)
{
    MessageBox(hWnd,
        (winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!",
        L"There is a winner!",
        MB_OK | MB_ICONINFORMATION);
    NewGameMsgBox(hWnd);
}

// TODO:
// MAKE A CLASS OF GLOBAL VARIABLES AND FUNCTIONS

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        hbr1 = CreateSolidBrush(P1COLOR);
        hbr2 = CreateSolidBrush(P2COLOR);

        // Load player icons
        hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
        hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case ID_FILE_NEWGAME:
        {
            NewGameMsgBox(hWnd);
        }
        break;
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        if (playerTurn == 0)
        {
            break;
        }

        int index = GetCellNumberFromPoint(hWnd, xPos, yPos);

        HDC hdc = GetDC(hWnd);
        if (hdc != NULL)
        {
            // Get cell dimension from its index
            if (index != -1)
            {
                RECT rcCell;

                if ((gameBoard[index] == 0) && GetCellRect(hWnd, index, &rcCell))
                {
                    gameBoard[index] = playerTurn;

                    DrawIconCentered(hdc, &rcCell, (playerTurn == 1) ? hIcon1 : hIcon2);
                    // Check if there is a winner
                    winner = GetWinner(wins);

                    if (winner == 1 || winner == 2)
                    {
                        (winner == 1) ? score[0]++ : score[1]++;
                        ShowWinningLine(hWnd, hdc);
                        // There is a winner
                        ResultMsgBox(hWnd,
                                    (winner == 1) ? L"Player 1 is the winner!" : L"Player 2 is the winner!",
                                    L"There is a winner!");
                    }
                    else if (winner == 3)
                    {
                        // It's a draw
                        ResultMsgBox(hWnd, 
                                    L"There is no winner! :(",
                                    L"It's a draw!");
                    }
                    else if (winner == 0)  
                    {
                        // No winner so continue
                        playerTurn = (playerTurn == 2) ? 1 : 2;
                    }
                    // Display turn
                    SelectObject(hdc, hFont);
                    ShowTurn(hWnd, hdc);

                }
            }
            ReleaseDC(hWnd, hdc);
        }
    }
    break;
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

        pMinMax->ptMinTrackSize.x = CELL_SIZE * 6;
        pMinMax->ptMinTrackSize.y = CELL_SIZE * 6;
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        RECT rc;

        ps.hdc = hdc;

        if (GetGameBoardRect(hWnd, &rc))
        {
            RECT rcClient;
            LOGFONT logFont;

            // Display player text and turn
            if (GetClientRect(hWnd, &rcClient))
            {
                const WCHAR szPlayer1[] = L"Player 1";
                const WCHAR szPlayer2[] = L"Player 2";
                const WCHAR font[] = L"Impact";
                
                // Change text font and size
                ChangeFont(hdc, 18, font);
                SetBkMode(hdc, TRANSPARENT);
                
                // Display player 1 text 
                DisplayPlayerText(hdc, 16, 16, 33, 50, szPlayer1);
                // Display player 2 text
                DisplayPlayerText(hdc, rcClient.right - 90, 16, rcClient.right - 68, 50, szPlayer2);
                
                // Display game count and score
                DisplayGameNumberAndScore(hdc, rcClient, font);

                // Display turn
                ChangeFont(hdc, 18, font);
                ShowTurn(hWnd, hdc);
            }

            //FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)); // Colors background and doesn't draw outer border
            //Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);  // Colors background and draws outer border
            for (int i = 0; i < 4; ++i)
            {
                // Draw vertical lines
                DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);

                // Draw horizontal lines
                DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
            }

            // Draw all occupied cells
            RECT rcCell;

            for (int i = 0; i < ARRAYSIZE(gameBoard); ++i)
            {
                if (gameBoard[i] != 0 && GetCellRect(hWnd, i, &rcCell))
                {
                    DrawIconCentered(hdc, &rcCell, (gameBoard[i] == 1) ? hIcon1 : hIcon2);
                }
            }
            // Show winner
            if (winner == 1 || winner == 2)
            {
                ShowWinningLine(hWnd, hdc);
            }
        }
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        DeleteObject(hbr1);
        DeleteObject(hbr2);
        // Dispose icon images
        DestroyIcon(hIcon1);
        DestroyIcon(hIcon2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

