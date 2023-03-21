// test commit
#include <stdlib.h>
#include <tchar.h>
#ifndef UNICODE
#define UNICODE
#define UNICODE_WAS_UNDEFINED
#endif

#include <Windows.h>

#ifdef UNICODE_WAS_UNDEFINED
#undef UNICODE
#endif

#include <windows.h>

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "RenderState.h"
#include "utils.cpp"

std::ofstream DEBUG;

globalVariable RenderState renderState;

#include "buttons.cpp"
#include "renderer.cpp"
// #include "testObjektas.cpp"
#include "zaidimas.cpp"

globalVariable bool running = true;

void translateWindowMessages( const HWND &window, Input &input );
void clearInputChangedParameter( Input &input );
void initializeLevel( std::vector<Player *> &p, std::vector<Objektas *> &platformos, std::vector<Enemy *> &enemys,
                      std::vector<Objektas *> &ptrToAllObj );
LRESULT CALLBACK window_callback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
    LRESULT result = 0;

    switch ( uMsg ) {
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
        } break;

        case WM_SIZE: {
            RECT rect;
            GetClientRect( hwnd, &rect );
            renderState.widht = rect.right - rect.left;
            renderState.height = rect.bottom - rect.top;

            if ( renderState.memory ) VirtualFree( renderState.memory, 0, MEM_RELEASE );
            int bufferSize = renderState.widht * renderState.height * sizeof( unsigned int );

            renderState.memory = VirtualAlloc( 0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );

            // bitmap info
            renderState.bitmapInfo.bmiHeader.biSize = sizeof( renderState.bitmapInfo.bmiHeader );
            renderState.bitmapInfo.bmiHeader.biWidth = renderState.widht;
            renderState.bitmapInfo.bmiHeader.biHeight = renderState.height;
            renderState.bitmapInfo.bmiHeader.biPlanes = 1;
            renderState.bitmapInfo.bmiHeader.biBitCount = 32;
            renderState.bitmapInfo.bmiHeader.biCompression = BI_RGB;
        } break;


        default: {
            result = DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }
    return result;
}

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) {
    // create window class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = L"Game Window";
    window_class.lpfnWndProc = window_callback;

    // register calss
    RegisterClass( &window_class );

    // create window
    HWND window = CreateWindow( window_class.lpszClassName, L"zaidimas", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT, 800, 800, 0, 0, hInstance, 0 );

    HDC hdc = GetDC( window );

    Input input = {};
    std::vector<Player *> p;
    std::vector<Objektas *> platformos;
    std::vector<Enemy *> enemys;
    std::vector<Objektas *> ptrToAllObj;
    DeathCounter deathCounter( 4 );

    initializeLevel( p, platformos, enemys, ptrToAllObj );

    float deltaTime = 0.016666;
    LARGE_INTEGER frameBeginTime;
    QueryPerformanceCounter( &frameBeginTime );
    float lapsedRenderTime = deltaTime;
    float lapsedUpdateTime = deltaTime;

    float cpuFreq;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency( &perf );
        cpuFreq = (float)perf.QuadPart;
    }
    // game loop
    float updateFPS = 2040.0;
    float renderFPS = 60.0;
    float updateSec = 1.0 / updateFPS;
    float renderSec = 1.0 / renderFPS;
    while ( running ) {
        // input
        //  isvalom changed parametra pries ziurint ar mygtuko paspaudimas pasikeite kita frame
        clearInputChangedParameter( input );
        translateWindowMessages( window, input );
        // if ( lapsedUpdateTime >= updateSec ) {
        lapsedUpdateTime = 0.0;
        simulateGame( &input, deltaTime, p, ptrToAllObj, deathCounter );
        //}
        if ( lapsedRenderTime >= renderSec ) {
            lapsedRenderTime = 0.0;

            renderGame( p, ptrToAllObj, deathCounter.getPositionMap() );
            // render window
            StretchDIBits( hdc, 0, 0, renderState.widht, renderState.height, 0, 0, renderState.widht,
                           renderState.height, renderState.memory, &renderState.bitmapInfo, DIB_RGB_COLORS, SRCCOPY );
        }
        LARGE_INTEGER frameEndTime;
        QueryPerformanceCounter( &frameEndTime );
        deltaTime = (float)( frameEndTime.QuadPart - frameBeginTime.QuadPart ) / cpuFreq;
        if ( deltaTime > 0.055 ) deltaTime = 0.0;
        frameBeginTime = frameEndTime;
        lapsedRenderTime += deltaTime;
        lapsedUpdateTime += deltaTime;
    }
}

void initializeLevel( std::vector<Player *> &p, std::vector<Objektas *> &platformos, std::vector<Enemy *> &enemys,
                      std::vector<Objektas *> &ptrToAllObj ) {
    // DEBUG.open( "debugOut.txt" );
    std::ifstream failas;
    std::string buffer[ 5 ];
    failas.open( "level1.txt" );
    if ( !failas.is_open() ) {
        failas.open( "level1.txt" );
    }
    if ( !failas.is_open() ) {
        running = false;
    }
    while ( !failas.eof() ) {
        failas >> buffer[ 0 ];
        if ( buffer[ 0 ] == "P" ) {
            failas >> buffer[ 1 ];
            failas >> buffer[ 2 ];
            p.push_back( new Player( std::stof( buffer[ 1 ] ), std::stof( buffer[ 2 ] ) ) );
        } else if ( buffer[ 0 ] == "G" ) {
            failas >> buffer[ 1 ];
            failas >> buffer[ 2 ];
            failas >> buffer[ 3 ];
            failas >> buffer[ 4 ];
            platformos.push_back( new Objektas( std::stof( buffer[ 1 ] ), std::stof( buffer[ 2 ] ),
                                                std::stof( buffer[ 3 ] ), std::stof( buffer[ 4 ] ) ) );
        } else if ( buffer[ 0 ] == "E" ) {
            failas >> buffer[ 1 ];
            failas >> buffer[ 2 ];
            enemys.push_back( new Enemy( std::stof( buffer[ 1 ] ), std::stof( buffer[ 2 ] ) ) );
        }
        // DEBUG << "aaa";
        // DEBUG << buffer[0];
    }
    for ( int i = 0; i < enemys.size(); i++ ) {
        ptrToAllObj.push_back( (Objektas *)enemys[ i ] );
    }
    for ( int i = 0; i < platformos.size(); i++ ) {
        ptrToAllObj.push_back( (Objektas *)platformos[ i ] );
    }
}
void clearInputChangedParameter( Input &input ) {
    for ( int i = 0; i < 4; i++ ) {
        input.buttons[ i ].changed = false;
    }
}
void translateWindowMessages( const HWND &window, Input &input ) {
    MSG message;
    while ( PeekMessage( &message, window, 0, 0, PM_REMOVE ) ) {
        switch ( message.message ) {
            case WM_KEYUP:
            case WM_KEYDOWN: {
                unsigned int buttonCode = (unsigned int)message.wParam;
                bool isDown = ( ( message.lParam & ( 1 << 31 ) ) == 0 );

                switch ( buttonCode ) {
                    case VK_UP: {
                        input.buttons[ ButtonUp ].down = isDown;
                        input.buttons[ ButtonUp ].changed = true;
                    } break;
                    case VK_DOWN: {
                        input.buttons[ ButtonDown ].down = isDown;
                        input.buttons[ ButtonDown ].changed = true;
                    } break;
                    case VK_LEFT: {
                        input.buttons[ ButtonLeft ].down = isDown;
                        input.buttons[ ButtonLeft ].changed = true;
                    } break;
                    case VK_RIGHT: {
                        input.buttons[ ButtonRight ].down = isDown;
                        input.buttons[ ButtonRight ].changed = true;
                    } break;
                    case VK_ESCAPE: {
                        input.buttons[ ButtonEsc ].down = isDown;
                        input.buttons[ ButtonEsc ].changed = true;
                        running = false;
                    } break;
                }
            } break;


            default: {
                TranslateMessage( &message );
                DispatchMessage( &message );
            }
        }
    }
}