#include <stdint.h>
#include <windows.h>
#include <ShObjIdl.h>
#include <xmmintrin.h>

#if DEBUG_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) ( sizeof(Array) / sizeof( ( Array )[0] ) )

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef int32 bool32;
#define internal static
#define global static

//TODO: PNG STRUCTURE HERE 

global int64 Global_PerfCounterFrequency;
global bool GlobalRunning;


//TODO: PNG FUNCTIONS HERE

//NOTE: This is all for timing and testing purposes
inline LARGE_INTEGER
Win_GetWallClock()
{
	LARGE_INTEGER Result;
	QueryPerformanceCounter( &Result );
	return Result;
}

inline real32
Win_GetSecondsElapsed( LARGE_INTEGER Start, LARGE_INTEGER End )
{
	real32 Result =  ( (real32)( End.QuadPart - Start.QuadPart ) /
		(real32)( Global_PerfCounterFrequency ) );
	return Result;
}

internal void
ProcessPendingMessages()
{ //NOTE: Do we need this? Probably for capturing user input while app is not
	//active
	MSG Message;
	while( PeekMessage(&Message, 0, 0, 0, PM_REMOVE ) )
	{ //TODO: Handle the messages here
		switch( Message.message )
		{
			case WM_QUIT:
			{
					GlobalRunning = false;
			};
			default:
				TranslateMessage(&Message);
				DispatchMessage(&Message);
			break;
		}
	}
}

LRESULT CALLBACK
WindowsCallback(  HWND Window,
				  UINT Message,
				  WPARAM WParam,
				  LPARAM LParam
				)
{
	LRESULT Result = 0;
	switch( Message )
	{
		case WM_SIZE:
		{
		}break;
		case WM_DESTROY:
		{
			//TODO: Handle this by logging it as an error
			GlobalRunning = false;

		}break;

		case WM_SYSKEYDOWN:
		{
		}break;

		case WM_SYSKEYUP:
		{
		};

		case WM_KEYDOWN:
		{
		}break;
		case WM_KEYUP:
		{
			uint32 VKCode = (uint32)WParam;

			//NOTE: Since we are comparing WasDown to IsDown we must use
				//these comparisons to convert the bit tests to to actual 0 & 1
				//values
			bool32 wasDown = ( (LParam & (1 << 30) ) != 0 );
			bool32 isDown = ((LParam & (1 << 31)) == 0);
			if(wasDown != isDown){
				if(VKCode == 'W'){
				}
				else if(VKCode == 'A'){

				}
				else if(VKCode == 'S'){

				}
				else if(VKCode == 'D'){

				}
				else if(VKCode == 'Q'){

				}
				else if(VKCode == 'E'){

				}
				else if(VKCode == VK_UP){

				}
				else if(VKCode == VK_DOWN){

				}
				else if(VKCode == VK_RIGHT){

				}
				else if(VKCode == VK_LEFT){

				}
				else if(VKCode == VK_ESCAPE){
				}
				else if(VKCode == VK_SPACE){
				}
			}

			bool32 AltKeyDown = (LParam & (1 << 29));

			if((VKCode == VK_F4) && AltKeyDown){

				GlobalRunning = false;

			}

			if(VKCode == VK_F4){
				OutputDebugStringA("F4\n");
			}

		}break;

		case WM_CLOSE:
		{
			//TODO: Hanlde this as a message or popup for the user
			GlobalRunning = false;

		}break;

		case WM_PAINT:
		{

		    //NOTE: This is where we are going to Draw the PNG

			PAINTSTRUCT paint;

			//NOTE: Solid Fill Window
			HDC hdc  = BeginPaint( Window, &paint );
			FillRect( hdc, &paint.rcPaint, (HBRUSH)(COLOR_WINDOW + 1) );
			EndPaint(Window, &paint);

		}break;

		default:
		{
			//OutputDebugString("Default\n");
			Result = DefWindowProcA(Window,Message,WParam, LParam); 

		}break;
	}

	return Result;
}

int WINAPI
WinMain
(
		HINSTANCE instance,
		HINSTANCE PrevInstance,
		LPSTR CommandLine,
		int ShowCode
)
{
	WNDCLASSA WindowClass = {};

	WindowClass.lpfnWndProc   = WindowsCallback; // Windows OS Callback
	WindowClass.hInstance     = instance;
	WindowClass.style         = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpszClassName = "WIN32_APP";

	HRESULT hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );

	if( SUCCEEDED(hr) )
	{

		IFileOpenDialog* FileDialog;
		hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&FileDialog );

		if( SUCCEEDED(hr) )
		{
			//Filter Name, Pattern
			COMDLG_FILTERSPEC FilterSpec[1] = { L"PNG", L"*.png" };

			hr = FileDialog->SetFileTypes( 1, FilterSpec );
			if( SUCCEEDED(hr) )
			{
				hr  = FileDialog->Show( NULL );
				if( SUCCEEDED(hr) )
				{

					if( RegisterClassA( &WindowClass ) )
					{
						//TODO: Create an internal structure for holding the pixel data of a png
						HWND Window = CreateWindowExA
							(
								0,
								WindowClass.lpszClassName,
								"WIN32_APP",
								WS_OVERLAPPEDWINDOW | WS_VISIBLE,
								//Size & Position
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								0,
								0,
								instance,
								0
						);

						real32 TargetSecondsPerFrame = 1.0f / 60;
						UINT DesiredSchedulerMS = 1;
						bool32 SleepIsGranular = ( timeBeginPeriod( DesiredSchedulerMS ) == TIMERR_NOERROR );

						LARGE_INTEGER PerfCounterFrequencyResult;
						QueryPerformanceFrequency( &PerfCounterFrequencyResult );
						Global_PerfCounterFrequency = PerfCounterFrequencyResult.QuadPart;

						//TODO: Initialize the COM LIBRARY
						//TODO: Create a new dialog box to select a file with the extension .PNG
						//TODO: Get the selected file and then load the PNG Stream
						if( Window )
						{
							LARGE_INTEGER LastCounter = Win_GetWallClock();
							GlobalRunning = true;


							while( GlobalRunning )
							{

								//TODO: Render/Draw the PNG
								ProcessPendingMessages();

								LARGE_INTEGER WorkCounter = Win_GetWallClock();
								real32 WorkSecondsElapsed = Win_GetSecondsElapsed( LastCounter, WorkCounter );

								real32 SecondsElapsedForFrame = WorkSecondsElapsed;
								if( SecondsElapsedForFrame < TargetSecondsPerFrame )
								{
									if( SleepIsGranular )
									{
										DWORD SleepMS = (DWORD)( (TargetSecondsPerFrame - SecondsElapsedForFrame) * 1000.0f);
										if(SleepMS > 0)
										{
											Sleep(SleepMS);
										}
									}

									real32 TestSecondsElapsedForFrame = Win_GetSecondsElapsed( LastCounter, WorkCounter );


									if( TestSecondsElapsedForFrame < TargetSecondsPerFrame )
									{
										//TODO: Log the missed sleep here

									}

									while( SecondsElapsedForFrame < TargetSecondsPerFrame )
									{
										SecondsElapsedForFrame = Win_GetSecondsElapsed(LastCounter, Win_GetWallClock() );
									}
								}
								else
								{
									//TODO: MISSED FRAME RATE
									//TODO: Logging
								}
								LARGE_INTEGER EndCounter = Win_GetWallClock();
								LastCounter = EndCounter;
							}
						}
					}
					else
					{ //NOTE: Failed to register window class with the OS

					}
				}
			}
			FileDialog->Release();
		}
	}

	CoUninitialize();

	return 0;
}
