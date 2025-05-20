/*******************************************************************
* Title: PNG Loader
* Author: Xander Bruce
* Date: 5-19-2025
* Desc: Loads a PNG on the Windows Platform, that's it
********************************************************************/

#include <stdint.h>
#include <windows.h>
#include <ShObjIdl.h>
#include <stdio.h>
#include <xmmintrin.h>

#if DEBUG_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) ( sizeof(Array) / sizeof( ( Array )[0] ) )
#define WIN32_FILE_NAME_COUNT MAX_PATH

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

global int64 Global_PerfCounterFrequency;
global bool GlobalRunning;
global uint8 PNGSignature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
global uint32 CRC_TABLE[256] = {}; //TODO: Could we entirely precompute this at
//compile time?

#include "PNG.h"


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

internal bool32
New_CompareString( char* String1, int String1Count, char* String2, int String2Count )
{ //NOTE: We Could rework this to look for the Null terminator instead of
	//relying on the string count and we can say that if one string is longer
	//than the other we just return false automatically
	bool32 Result = false;
	for( int i = 0; i < String1Count && i < String2Count; i++ )
	{
		if( String1[i] != String2[i] )
		{
			break;
		}
		else if( ( i == ( String1Count - 1 ) ) && ( i == (String2Count - 1) ) )
		{
			Result = true;
		}
	}
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

internal bool32
GetPNGFile( PNG *png )
{
	bool32 Valid = false;
	HRESULT hr;

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
				IShellItem *pItem;
				hr = FileDialog->GetResult( &pItem );
				if( SUCCEEDED(hr) )
				{
					PWSTR Filename;
					hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &Filename );
					if( SUCCEEDED(hr) )
					{
						if( WideCharToMultiByte( CP_UTF8 , WC_DEFAULTCHAR, Filename, -1, png->FileName, WIN32_FILE_NAME_COUNT, NULL, NULL  ) )
						{
							Valid = true;
						}
						CoTaskMemFree( Filename );
					}
					pItem->Release();
				}
			}
		}
	}
	FileDialog->Release();


	return Valid;
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

	GlobalRunning = true;

	PNG png = {};

	if( SUCCEEDED(hr) )
	{
		if( GetPNGFile( &png ) )
		{

			png.File = CreateFileA( png.FileName,
						  GENERIC_READ,
						  FILE_SHARE_READ,
						  0, 
						  OPEN_EXISTING, 0, 0);

			if( png.File != INVALID_HANDLE_VALUE )
			{

				uint8 FileSignature[8] = {};
				DWORD BytesRead;

				//NOTE: GET RID OF THIS
				if( ReadFile( png.File, FileSignature, 8, &BytesRead, 0) )
				{
					if( ValidPNGSignature( FileSignature, 8 ) )
					{ //NOTE: PNG is valid now load all the
						//PNG data and render

						//TODO: We can probably pull out
						//loading the chunks for the png
						//NOTE: B for BigEndian, L for
						//little Endian
						uint32 B_ChunkLength = 0;
						//NOTE: This is for testing
						uint32 L_ChunkLength = 0;
						if( ReadFile( png.File, &B_ChunkLength, 4, &BytesRead, 0 ) )
						{
							L_ChunkLength = uint32_BigEndianToLittleEndian( B_ChunkLength );
						}

						Assert( L_ChunkLength > 0 );
						Assert( L_ChunkLength < 2147483647 );
						//NOTE: Chunk Length cannot exceed
						//2^31 - 1

						char ChunkType[5] = {};
						if( ReadFile( png.File, &ChunkType, 4, &BytesRead, 0 ) )
						{
						}
						ChunkType[4] = '\0';

						ComputeCRCTable();

						//TODO: This is just bad change this!!!
						//let's just allocate some small
						//amount of virtual memory and if
						//our chunk length is greater than
						//our current memory size we virtual
						//free and reallocate as necessary
						//or we need we can just try to
						//guess how much memory we might
						//need by virtual allocating the
						//size of the png
						char HeaderChunkData[ 50 ] = {};

						if( New_CompareString( ChunkType, 5, "IHDR\0", 5 ) )
						{ //Load all header info
							OutputDebugStringA("PNG HEADER FOUND!!!\nLoading PNG DETAILS!!!\n");

							if( ReadFile( png.File, HeaderChunkData, L_ChunkLength, &BytesRead, 0 ) )
							{
								OutputDebugStringA("Chunk Data Read\n");
							}

							uint32 L_ReadCRC = 0;
							uint32 B_ReadCRC = 0;
							if( ReadFile( png.File, &B_ReadCRC, 4, &BytesRead, 0 ) )
							{
								L_ReadCRC = uint32_BigEndianToLittleEndian( B_ReadCRC );

								//TODO: Check the CRC to see
								//if the data is corrupted
								uint32 CalculatedCRC = CalculateCRC( ChunkType, HeaderChunkData, L_ChunkLength );
								if( L_ReadCRC != CalculatedCRC )
								{//NOTE: Data is corrupted 
									//Error / crash
									//application
									OutputDebugStringA("CRC Check Failed - Chunk Data Invalid!!!\n");
									GlobalRunning = false;
								}
								else
								{
									OutputDebugStringA("CRC Check Succeeded - Chunk Data Valid!!!\n");
									png.Header = (PNG_Header*)HeaderChunkData;
									png.Header->Width = uint32_BigEndianToLittleEndian( png.Header->Width );
									png.Header->Height = uint32_BigEndianToLittleEndian( png.Header->Height );
								}
							}

						}

						if( ReadFile( png.File, &B_ChunkLength, 4, &BytesRead, 0 ) )
						{
							L_ChunkLength = uint32_BigEndianToLittleEndian( B_ChunkLength );
						}

						Assert( L_ChunkLength > 0 );
						Assert( L_ChunkLength < 2147483647 );

						if( ReadFile( png.File, &ChunkType, 4, &BytesRead, 0 ) )
						{
							ChunkType[4] = '\0';
							char TextBuffer[256] = {};
							_snprintf_s( TextBuffer,sizeof(TextBuffer), "Chunk Type %s\n", ChunkType);
							OutputDebugStringA( TextBuffer );
						}
						ChunkType[4] = '\0';

						char PLTEChunkData[ 1000 ] = {};
						if( New_CompareString( ChunkType, 4, "PLTE", 4 ) )
						{ //Load all Palette Info

							OutputDebugStringA("PNG Palette Found!!!\nLoading Palette!!!\n");

							if( L_ChunkLength % 3 != 0 )
							{
								OutputDebugStringA("ERROR: Palette Chunk Length is not divisible by 3!!!\nExiting!!!\n");
								GlobalRunning = false;
							}

							if( ReadFile( png.File, PLTEChunkData, L_ChunkLength, &BytesRead, 0 ) )
							{
								OutputDebugStringA("Chunk Data Read\n");
							}

							uint32 L_ReadCRC = 0;
							uint32 B_ReadCRC = 0;
							if( ReadFile( png.File, &B_ReadCRC, 4, &BytesRead, 0 ) )
							{
								L_ReadCRC = uint32_BigEndianToLittleEndian( B_ReadCRC );

								//TODO: Check the CRC to see
								//if the data is corrupted
								uint32 CalculatedCRC = CalculateCRC( ChunkType, PLTEChunkData, L_ChunkLength );
								if( L_ReadCRC != CalculatedCRC )
								{//NOTE: Data is corrupted 
									//Error / crash
									//application
									OutputDebugStringA("CRC Check Failed - Chunk Data Invalid!!!\n");
									MessageBoxA( NULL, "ERROR: CHUNK DATA INVALID\0", "CRC CHECK FAILED\0",MB_OK | MB_ICONERROR );
									GlobalRunning = false;
								}
								else
								{
									OutputDebugStringA("CRC Check Succeeded - Chunk Data Valid!!!\n");
									png.Palette = (PNG_Palette*)PLTEChunkData;
								}
							}

						}


						if( RegisterClassA( &WindowClass ) )
						{
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

							if( Window )
							{
								LARGE_INTEGER LastCounter = Win_GetWallClock();

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

				CloseHandle( png.File );
			}
		}
	}

	CoUninitialize();
	return 0;
}
