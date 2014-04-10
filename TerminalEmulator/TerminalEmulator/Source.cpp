/*---------------------------------------------------------------------------------------------------
-- SOURCE FILE: TerminalEmulator.cpp - An application that will transmit all characters typed on 
--                               the keyboard to the serial port and display all characters received
--                               via the serial port. 
--
-- PROGRAM: terminalemulator
--
-- FUNCTIONS:
-- VOID sendToPort(WPARAM wParam);
-- VOID readFromPort(HWND hwnd);
-- DWORD WINAPI readThreadFunc(LPVOID hwnd);
--
-- DATE: September 28, 2013
--
-- REVISIONS: September 29, 2013
--
-- DESIGNER: Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER: Robin Hsieh (Wei Chung) A00657820
--
-- NOTES:
-- To use this application, you must have com1 ready to be used for the serial port on lab computers.
-- Once opened, after setting all the CommConfig under settings, and clicking connect, will start the
-- program to start listening and sending to the serial ports through the function calls of sendToPort()
-- and readFromPort() functions. This program will only work if there is something coming from the port,
-- so it can write to the port. (Cannot be triggered at the same exact time) 
--
-- September 29, 2013
-- Added a new list of functions to make it more efficient
---------------------------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "Header.h"

static TCHAR Name[] = TEXT("Robin Hsieh's Comm Shell");

#pragma warning (disable: 4096)

LPTSTR    lpszCommName;
COMMCONFIG    cc;
HANDLE hComm;
char buffer[1];
bool connection = false;
HANDLE readThrd;
HANDLE writeThrd;
DWORD threadId;
int xCoord;
int yCoord = 0;

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
                           LPSTR lspszCmdParam, int nCmdShow)
{
    HWND hwnd;
    MSG Msg;
    WNDCLASSEX Wcl;

    Wcl.cbSize = sizeof (WNDCLASSEX);
    Wcl.style = CS_HREDRAW | CS_VREDRAW;
    Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
    Wcl.hIconSm = NULL; // use small version of large icon
    Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style
    
    Wcl.lpfnWndProc = WndProc;
    Wcl.hInstance = hInst;
    Wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); //white background
    Wcl.lpszClassName = Name;
    
    Wcl.lpszMenuName = TEXT("MYMENU"); // The menu Class
    Wcl.cbClsExtra = 0;      // no extra memory needed
    Wcl.cbWndExtra = 0; 
    
    if (!RegisterClassEx (&Wcl))
        return 0;

    hwnd = CreateWindow (Name, Name, WS_OVERLAPPEDWINDOW, 10, 10,
                               600, 400, NULL, NULL, hInst, NULL);
    
    EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_DISABLED);
    EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_DISABLED);

    ShowWindow (hwnd, nCmdShow);
    UpdateWindow (hwnd);




    while (GetMessage (&Msg, NULL, 0, 0))
    {
           TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }




    return Msg.wParam;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{
    
    switch (Message)
    {
        // Whenever the window has input from keyboard, it will send input to serial port
        case WM_CHAR:
            {
                // 27 is the <ESC> key input from the keyboard in ASCII code
                if(wParam == 27)
                {
                    disconnect(hwnd);
                }
                else if(connection == true)
                {
                    sendToPort(wParam);
                }                
            }
        break;
        
        case WM_COMMAND:
            switch (LOWORD (wParam))
            {
                // when connect is pressed, it will create file with the proper settings
                case IDM_CONNECT:
                
                connection = true;
                if ((hComm = CreateFile (lpszCommName, GENERIC_READ | GENERIC_WRITE, 0,
                               NULL, OPEN_EXISTING, 0, NULL))
                            == INVALID_HANDLE_VALUE)
                {
                    DisplayResourceNAMessageBox(hwnd, TEXT("Error opening COM port:"));
                    return FALSE;
                }
                EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_DISABLED);
                EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_ENABLED);
                redrawMenuBar(hwnd);

                PurgeComm(hComm, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);

                // reading from serial port
                readThrd = CreateThread(NULL, 0, readThreadFunc, (LPVOID)hwnd, 0, &threadId );
                    
                break;

                case IDM_DISCONNECT:
                    disconnect(hwnd);
                                   
                break;

                case IDM_COM1:
                    getcc(hwnd, TEXT("com1"));
                    break;
                case IDM_COM2:
                    getcc(hwnd, TEXT("com2"));
                    break;
                case IDM_COM3:
                    getcc(hwnd, TEXT("com3"));
                    break;
                case IDM_COM4:
                    getcc(hwnd, TEXT("com4"));
                    break;
                case IDM_COM5:
                    getcc(hwnd, TEXT("com5"));
                    break;
                case IDM_COM6:
                    getcc(hwnd, TEXT("com6"));
                    break;
                case IDM_COM7:
                    getcc(hwnd, TEXT("com7"));
                    break;
                case IDM_COM8:
                    getcc(hwnd, TEXT("com8"));
                    break;

                case IDM_HELP:
                    DisplayResourceNAMessageBox(hwnd, TEXT("How to use this application:\n\nSelect Comm Port by going into the Settings --> Port Settings \nFill in all the required fields, then click OK\nNow connectiong will be ready to be used.\n\nIf the connection to a serial port is failing, please check the serial port cable for any damanged, or click the right port to connect to.\nIf all else fails, throw away your computer, and buy a new one."), 0);
                    break;
            }
        break;

        case WM_DESTROY:    // Terminate program
              PostQuitMessage (0);
        break;
        default:
            return DefWindowProc (hwnd, Message, wParam, lParam);
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendToPort
--
-- DATE:                September 28, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           VOID sendToPort(WPARAM wParam)
--                                  WPARAM wParam: the characters being typed by user through keyboard (ASCII characters)
--
-- RETURNS:             void.
--
-- NOTES:
-- Call this function to send characters to the serial port for this application. 
----------------------------------------------------------------------------------------------------------------------*/
VOID sendToPort(WPARAM wParam)
{
    char a = wParam;
    DWORD written;

    WriteFile(hComm, &a, 1, &written, NULL);

}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: readFromPort
--
-- DATE:                September 28, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           VOID readFromPort(HWND hwnd)
--                      HWND hwnd: Passes in the handle so it can be used to display characters on the active window.
--
-- RETURNS:             void.
--
-- NOTES:
-- Call this function to receive information passed in from the serial port.
----------------------------------------------------------------------------------------------------------------------*/
VOID readFromPort(HWND hwnd)
{

    OVERLAPPED olp = {0};
    HDC hdc;
    DWORD dwBytesTransferred;
    COMMTIMEOUTS cto;
    int widthOfCharacter = 11;
    int heightofCharacter = 20;
    RECT rect;
    int marginWidth = 10;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left - marginWidth;
    
    GetCommTimeouts(hComm, &cto);
    // Set the new timeouts
    cto.ReadIntervalTimeout = MAXDWORD;
    cto.ReadTotalTimeoutConstant = 0;
    cto.ReadTotalTimeoutMultiplier = 0;

    SetCommTimeouts(hComm,&cto);
        
        
    ReadFile(hComm, buffer, 1, &dwBytesTransferred, NULL);

    if(dwBytesTransferred > 0)
    {
        hdc = GetDC (hwnd);
        if((xCoord+widthOfCharacter) >= width)
        {
            yCoord += heightofCharacter;
            xCoord = 0;
        }
        TextOut(hdc, xCoord, yCoord, (LPCSTR)buffer, 1);
        xCoord += widthOfCharacter;
        ReleaseDC (hwnd, hdc); // Release device context
    }
    Sleep(20);
    
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: readThreadFunc
--
-- DATE:                September 28, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           DWORD WINAPI readThreadFunc(LPVOID hwnd)
--                                  LPVOID hwnd: passes in the handle to redirect it into the readFromPort
--
-- RETURNS:             Returns the 0 after the process has been completed.
--
-- NOTES:
-- Call this function when the application needs a new thread. 
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI readThreadFunc(LPVOID hwnd)
{
    while(connection == true)
    {
        readFromPort((HWND)hwnd);
        //Sleep(20);
    }
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: getcc
--
-- DATE:                September 29, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           VOID getcc(HWND hwnd, LPSTR str)
--                                  HWND hwnd: handle to the window
--                                    LPSTR str: string that is being passed in for the CommName
--
-- RETURNS:             VOID
--
-- NOTES:
-- Call this function after a COM Port has been selected
----------------------------------------------------------------------------------------------------------------------*/
VOID getcc(HWND hwnd, LPSTR str)
{
    lpszCommName = str;
    cc.dwSize = sizeof(COMMCONFIG);
    cc.wVersion = 0x100;
    GetCommConfig (hComm, &cc, &cc.dwSize);
    int WindowOpened = CommConfigDialog (lpszCommName, hwnd, &cc);
    if (!WindowOpened)
    {

        DisplayResourceNAMessageBox(hwnd, TEXT("Error, COM port not selected or not available. Try again."));
        return;
    }

    

    //SetCommState(hComm, &cc.dcb);

    EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_ENABLED);
    redrawMenuBar(hwnd);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: redrawMenuBar
--
-- DATE:                September 29, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           VOID redrawMenuBar(HWND hwnd)
--                                  HWND hwnd: handle to the window
--
-- RETURNS:             VOID
--
-- NOTES:
-- Call this function to redraw the menubar
----------------------------------------------------------------------------------------------------------------------*/
VOID redrawMenuBar(HWND hwnd)
{

    DrawMenuBar(hwnd);    
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: DisplayResourceNAMessageBox
--
-- DATE:                September 29, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           int DisplayResourceNAMessageBox(HWND hwnd, LPCSTR msg)
--                                  HWND hwnd: handle to the window
--                                    LPCSTR msg: error message being passed in
--
-- RETURNS:             int - returns the message ID
--
-- NOTES:
-- Calls the DisplayResourceNAMessageBox with the default value for the flag MB_ICONWARNING
----------------------------------------------------------------------------------------------------------------------*/
int DisplayResourceNAMessageBox(HWND hwnd, LPCSTR msg)
{
    return DisplayResourceNAMessageBox(hwnd, msg, MB_ICONWARNING);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: DisplayResourceNAMessageBox (Overloaded Function)
--
-- DATE:                September 29, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           int DisplayResourceNAMessageBox(HWND hwnd, LPCSTR msg)
--                                  HWND hwnd: handle to the window
--                                    LPCSTR msg: error message being passed in
--                                    INT flag: message box options
--
-- RETURNS:             int - returns the message ID
--
-- NOTES:
-- Calls an error message box containing the appropriate message regarding the error
----------------------------------------------------------------------------------------------------------------------*/
int DisplayResourceNAMessageBox(HWND hwnd, LPCSTR msg, INT flag)
{

    int msgboxID = MessageBox(
        hwnd,
        msg,
        TEXT("Help!"),
        flag | MB_OK
    );

    switch (msgboxID)
    {
    case IDOK:
        // TODO: add code
        break;
    }

    return msgboxID;
}




/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: disconnect
--
-- DATE:                September 29, 2013
--
-- REVISIONS:           (Date and Description)
--
-- DESIGNER:            Robin Hsieh (Wei Chung) A00657820
--
-- PROGRAMMER:          Robin Hsieh (Wei Chung) A00657820
--
-- INTERFACE:           VOID disconnect(HWND hwnd)
--                                  HWND hwnd: handle to the window
--
-- RETURNS:             VOID
--
-- NOTES:
-- Call this function to disconnect from serial port
----------------------------------------------------------------------------------------------------------------------*/
VOID disconnect(HWND hwnd)
{
    if(CloseHandle(hComm) == 0)    // Call this function to close port.
    {
        DisplayResourceNAMessageBox(hwnd, TEXT("Port Closeing isn't successed."));
        return;
    }
    EnableMenuItem(GetMenu(hwnd), IDM_CONNECT, MF_ENABLED);
    EnableMenuItem(GetMenu(hwnd), IDM_DISCONNECT, MF_DISABLED);
    redrawMenuBar(hwnd);
    connection = false;  
}
                    