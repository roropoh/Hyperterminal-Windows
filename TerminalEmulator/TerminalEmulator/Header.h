#define IDM_CONNECT     100
#define IDM_DISCONNECT  101
#define IDM_Port        102
#define IDM_Speed       103
#define IDM_Word        104
#define IDM_ZETA        105
#define IDM_Save        106
#define IDM_THETA       107
#define IDM_HELP        108
#define IDM_COM1        109
#define IDM_COM2        110
#define IDM_COM3        111
#define IDM_COM4        112
#define IDM_COM5        113
#define IDM_COM6        114
#define IDM_COM7        115
#define IDM_COM8        116

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
VOID sendToPort(WPARAM wParam);
VOID readFromPort(HWND hwnd);
DWORD WINAPI readThreadFunc(LPVOID hwnd);
VOID getcc(HWND hwnd, LPSTR str);
VOID redrawMenuBar(HWND hwnd);
int DisplayResourceNAMessageBox(HWND hwnd, LPCSTR msg);
int DisplayResourceNAMessageBox(HWND hwnd, LPCSTR msg, INT flag);
VOID disconnect(HWND hwnd);
