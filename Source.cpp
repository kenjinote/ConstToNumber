#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include "const.h"

TCHAR szClassName[] = TEXT("ConstToNumber");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;
	static HWND hEdit2;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SetFocus(hEdit1);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, LOWORD(lParam) - 20, 32, TRUE);
		MoveWindow(hEdit2, 10, 50, LOWORD(lParam) - 20, 32, TRUE);
		break;
	case WM_COMMAND:
		if (lParam == (LPARAM)hEdit1 && ((wParam >> 16) & 0xFFFF) == EN_CHANGE)
		{
			DWORD dwSize = GetWindowTextLength(hEdit1);
			if (dwSize == 0) {
				return 0;
			}
			LPWSTR lpszText1 = (LPWSTR)GlobalAlloc(0, (dwSize + 1) * sizeof(WCHAR));
			if (lpszText1 == 0) return 0;
			LPWSTR lpszText2 = (LPWSTR)GlobalAlloc(0, (dwSize + 1) * sizeof(WCHAR));
			if (lpszText2 == 0) {
				GlobalFree(lpszText1);
				return 0;
			}
			GetWindowText(hEdit1, lpszText1, dwSize + 1);
			LPWSTR p = lpszText1;
			LPWSTR q = lpszText2;
			while (*p) {
				if (*p != L' ' && *p != L'\n' && *p != L'\r' && *p != L'\t' && *p != L'(' && *p != L')') {
					*q = *p;
					q++;
				}
				p++;
			}
			*q = 0;
			INT64 nValue = 0;
			LPCWSTR seps = L"|";
			LPWSTR context = 0;
			LPWSTR token = wcstok_s(lpszText2, seps, &context);
			bool error = false;
			while (token != NULL) {
				bool find = false;
				for (auto i : const_list) {
					if (lstrcmp(token, i.szName) == 0) {
						nValue += i.nValue;
						find = true;
						break;
					}
				}
				if (find == false) {
					WCHAR szText[1024];
					wsprintf(szText, L"%sの定義が見つかりませんでした。", token);
					SetWindowText(hEdit2, szText);
					error = true;
					break;
				}
				token = wcstok_s(NULL, seps, &context);
			}
			GlobalFree(lpszText1);
			GlobalFree(lpszText2);
			if (error == false) {
				WCHAR szText[1024];
				wsprintf(szText, L"%I64d", nValue);
				SetWindowText(hEdit2, szText);
				SendMessage(hEdit2, EM_SETSEL, 0, -1);
				SetFocus(hEdit2);
			}
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	MSG msg;
	WNDCLASS wndclass = {
		0,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("ConstToNumber"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		640,
		320,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
