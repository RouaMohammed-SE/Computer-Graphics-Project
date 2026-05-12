//#include <Windows.h>
//#include <cmath>
//#include <iostream>
//
//using namespace std;
//
//int clickCount = 0;
//int xc, yc;
//int x1_, y1_, x2_, y2_;
//int quarter = 1;
//
//// Draw thicker pixel
//void DrawThickPixel(HDC hdc, int x, int y, COLORREF C)
//{
//    SetPixel(hdc, x, y, C);
//    SetPixel(hdc, x + 1, y, C);
//    SetPixel(hdc, x, y + 1, C);
//    SetPixel(hdc, x + 1, y + 1, C);
//}
//
//// Draw points in the selected quarter
//void DrawQuarterPoints(HDC hdc,
//    int xc, int yc,
//    int x, int y,
//    int quarter,
//    COLORREF C)
//{
//    switch (quarter)
//    {
//    case 1: // Upper Right
//        DrawThickPixel(hdc, xc + x, yc - y, C);
//        DrawThickPixel(hdc, xc + y, yc - x, C);
//        break;
//
//    case 2: // Upper Left
//        DrawThickPixel(hdc, xc - x, yc - y, C);
//        DrawThickPixel(hdc, xc - y, yc - x, C);
//        break;
//
//    case 3: // Lower Left
//        DrawThickPixel(hdc, xc - x, yc + y, C);
//        DrawThickPixel(hdc, xc - y, yc + x, C);
//        break;
//
//    case 4: // Lower Right
//        DrawThickPixel(hdc, xc + x, yc + y, C);
//        DrawThickPixel(hdc, xc + y, yc + x, C);
//        break;
//    }
//}
//
//// Bresenham Circle Drawing second order
//void DRAWCircleBres(HDC hdc,
//    int xc, int yc,
//    int R,
//    int quarter,
//    COLORREF C)
//{
//    int x = 0;
//    int y = R;
//
//    int d = 1 - R;
//    int ch1 = 3;
//    int ch2 = 5 - 2 * R;
//
//    DrawQuarterPoints(hdc, xc, yc, x, y, quarter, C);
//
//    while (x < y)
//    {
//        if (d < 0)
//        {
//            d += ch1;
//            ch2 += 2;
//        }
//        else
//        {
//            d += ch2;
//            ch2 += 4;
//            y--;
//        }
//
//        ch1 += 2;
//        x++;
//
//        DrawQuarterPoints(hdc, xc, yc, x, y, quarter, C);
//    }
//}
//
//LRESULT WINAPI WndProc(HWND hwnd,
//    UINT mcode,
//    WPARAM wp,
//    LPARAM lp)
//{
//    HDC hdc;
//
//    switch (mcode)
//    {
//    case WM_LBUTTONDOWN:
//    {
//        int x = LOWORD(lp);
//        int y = HIWORD(lp);
//
//        // First click -> center
//        if (clickCount == 0)
//        {
//            xc = x;
//            yc = y;
//
//            clickCount++;
//        }
//        // Second click -> first radius
//        else if (clickCount == 1)
//        {
//            x1_ = x;
//            y1_ = y;
//
//            clickCount++;
//        }
//        // Third click -> second radius and draw
//        else if (clickCount == 2)
//        {
//            x2_ = x;
//            y2_ = y;
//
//            hdc = GetDC(hwnd);
//
//            int R1 = (int)sqrt(
//                (x1_ - xc) * (x1_ - xc) +
//                (y1_ - yc) * (y1_ - yc));
//
//            int R2 = (int)sqrt(
//                (x2_ - xc) * (x2_ - xc) +
//                (y2_ - yc) * (y2_ - yc));
//
//            int rmin = min(R1, R2);
//            int rmax = max(R1, R2);
//
//            if (rmax == rmin)
//                rmax++;
//
//            // Fill using concentric circles
//            for (int r = rmin; r <= rmax; r++)
//            {
//                float t = float(r - rmin) /
//                    float(rmax - rmin);
//
//                int red = int(255 * t);
//                int green = int(255 * (1 - t));
//                int blue = (red + green) / 2;
//
//                DRAWCircleBres(hdc, xc, yc, r, quarter, RGB(red, green, blue));
//            }
//
//            ReleaseDC(hwnd, hdc);
//
//            clickCount = 0;
//        }
//        break;
//    }
//
//    case WM_CLOSE:
//        DestroyWindow(hwnd);
//        break;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//
//    default:
//        return DefWindowProc(hwnd, mcode, wp, lp);
//    }
//
//    return 0;
//}
//
//int APIENTRY WinMain(
//    HINSTANCE h,
//    HINSTANCE p,
//    LPSTR c,
//    int nsh)
//{
//    AllocConsole();
//
//    FILE* stream;
//
//    freopen_s(&stream, "CONOUT$", "w", stdout);
//    freopen_s(&stream, "CONIN$", "r", stdin);
//
//    cout << "Choose Quarter:\n";
//    cout << "1 -> Upper Right\n";
//    cout << "2 -> Upper Left\n";
//    cout << "3 -> Lower Left\n";
//    cout << "4 -> Lower Right\n";
//
//    cin >> quarter;
//
//    WNDCLASS wc = {};
//
//    wc.cbClsExtra = 0;
//    wc.cbWndExtra = 0;
//
//    wc.hbrBackground =
//        (HBRUSH)GetStockObject(LTGRAY_BRUSH);
//
//    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//
//    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
//
//    wc.hInstance = h;
//
//    wc.lpfnWndProc = WndProc;
//
//    wc.lpszClassName = L"myclass";
//
//    wc.lpszMenuName = NULL;
//
//    wc.style = CS_HREDRAW | CS_VREDRAW;
//
//    RegisterClass(&wc);
//
//    HWND hwnd = CreateWindow(
//        L"myclass",
//        L"Fill Circle With Other Circles",
//        WS_OVERLAPPEDWINDOW,
//        0,
//        0,
//        800,
//        600,
//        NULL,
//        NULL,
//        h,
//        0);
//
//    ShowWindow(hwnd, nsh);
//
//    UpdateWindow(hwnd);
//
//    MSG msg;
//
//    while (GetMessage(&msg, NULL, 0, 0) > 0)
//    {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return 0;
//}