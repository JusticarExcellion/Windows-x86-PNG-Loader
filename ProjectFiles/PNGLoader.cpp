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

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

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
global uint32 CRC_TABLE[256] = {}; //TODO: Could we entirely precompute this at
//compile time?

#include "StringUtils.h"
#include "PNG.h"
#include "BitReader.h"
#include "Huffman.h"

inline uint32
SwapEndianess( uint32 value )
{
	return (uint32)( ( value & 0x000000FFL ) << 24 | ( value & 0x0000FF00L ) << 8 | ( value & 0x00FF0000L ) >> 8 | ( value & 0xFF000000L ) >> 24);
}

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

internal bool32
GetPNGFile( char* PNG_Filename )
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
						if( WideCharToMultiByte( CP_UTF8 , WC_DEFAULTCHAR, Filename, -1, PNG_Filename, WIN32_FILE_NAME_COUNT, NULL, NULL  ) )
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

inline uint32
GetBits( uint8**Data, int bitLength )
{
	uint32 Result = 0;
	*Data += bitLength;
	return Result;
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
			if(wasDown != isDown)
				{
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

			if( (VKCode == VK_F4) && AltKeyDown){
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
			FillRect(hdc, &paint.rcPaint, (HBRUSH)(COLOR_WINDOW + 1) );
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

	#if PNG_INTERNAL
	LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
	LPVOID BaseAddress = 0;
#endif

	HANDLE File = NULL;
	char Filename[ WIN32_FILE_NAME_COUNT ];
	void* FileMemory = NULL;
	uint32 BytesPerPixel = 8;

	if( SUCCEEDED(hr) )
	{
		ComputeCRCTable();
		//Establish a while loop here so we can continuously open new png files
		if( GetPNGFile( Filename ) )
		{

			File = CreateFileA( Filename,
					  GENERIC_READ,
					  FILE_SHARE_READ,
					  0,
					  OPEN_EXISTING, 0, 0);

			if( File != INVALID_HANDLE_VALUE )
			{

				PNG_Signature* Signature = NULL;
				DWORD BytesRead;
				LARGE_INTEGER FileSize;
				uint32 Offset = 0;

				if( GetFileSizeEx( File, &FileSize ) )
				{
					FileMemory = VirtualAlloc( BaseAddress, (size_t)FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );

				if( FileMemory )
					{
						if( ReadFile( File, FileMemory, FileSize.LowPart, &BytesRead, 0 ) )
						{
							Signature = (PNG_Signature*)FileMemory;
							bool32 Valid = ValidPNGSignature( Signature );

							if( Valid )
							{
								Offset += sizeof( PNG_Signature );
								PNG_Header* Header = NULL;
								PNG_IDAT_Header* IDAT_Header = NULL;
								IDAT_Chunk* FirstDataChunk = NULL;
								IDAT_Chunk* LastDataChunk = NULL;

								while( Offset < FileSize.QuadPart && Valid )
								{ //Loading All of the data
									PNG_Chunk_Header* CHeader = NULL;
									void* CData = NULL;
									PNG_Chunk_Footer* CFooter = NULL;

									CHeader = (PNG_Chunk_Header*)((char*)FileMemory + Offset);
									CHeader->Length = SwapEndianess( CHeader->Length );

									#if DEBUG_PNG
									Assert( CHeader->Length < 2147483647 );
									Assert( (Offset + CHeader->Length) < FileSize.QuadPart ); // NOTE: Ensures that we never write to unallocated memory
									#endif
									#if BUILD_PNG
									Valid = (CHeader->Length < 2147483647);
									#endif

									CData = (void*)((char*)FileMemory + Offset + sizeof(PNG_ChunkHeader));

									char TextBuffer[256] = {};
									_snprintf_s( TextBuffer,sizeof(TextBuffer), "Chunk Type %c%c%c%c, (%u)\n", CHeader->Type[0],CHeader->Type[1],CHeader->Type[2],CHeader->Type[3], CHeader->Length );
									OutputDebugStringA( TextBuffer );

									CFooter = (PNG_Chunk_Footer*)((char*)FileMemory + Offset + ( sizeof(PNG_ChunkHeader) + CHeader->Length ) );
									CFooter->CRC = SwapEndianess( CFooter->CRC );

									uint32 CalculatedCRC = CalculateCRC( (char*)CData, CHeader );
									if( CFooter->CRC != CalculatedCRC )
									{
										OutputDebugStringA("CRC Check Failed - Chunk Data Invalid!!!\n");
										Valid = false;
										break;
									}
									else
									{
										OutputDebugStringA("CRC Check Succeeded - Chunk Data Valid!!!\n");

										if(CHeader->u32Type == FourCC("IHDR") )
										{
											OutputDebugStringA("Header Found\n");
											Header = (PNG_Header*)CData;
											Header->Width = SwapEndianess(Header->Width);
											Header->Height = SwapEndianess(Header->Height);
										}

										if(CHeader->u32Type == FourCC("IDAT") )
										{
											IDAT_Chunk* DataChunk = AllocateDataChunk();
											#if PNG_DEBUG
											Assert( DataChunk != NULL );
											#endif
											#if PNG_RELEASE
											if(!DataChunk)
											{
												Valid = false;
												break;
											}
											#endif
											DataChunk->Length = CHeader->Length;
											DataChunk->Data = (uint8*)CData;
											DataChunk->Next = NULL;
											//NOTE: Muratorism
											LastDataChunk = ( ( LastDataChunk ? LastDataChunk->Next : FirstDataChunk ) = DataChunk);
											if(!IDAT_Header)
											{
												IDAT_Header= (PNG_IDAT_Header*)CData;
												DataChunk->Data += sizeof(PNG_IDAT_Header);
												DataChunk->Length-= sizeof(PNG_IDAT_Header);
											}
										}

										if(CHeader->u32Type == FourCC("IEND"))
										{
											OutputDebugStringA("End found\n");
											break;
										}


										Offset += (CHeader->Length) + sizeof( PNG_ChunkHeader  ) + sizeof( PNG_ChunkFooter );
									}

								}

								Valid = (Header && IDAT_Header ); //TODO: There
								// should be a better way to validate these

								if( Valid )
								{
									uint8 CompressionMethod = ( IDAT_Header->ZlibFlagsCode & 0xF );
									uint8 CompressionInfo = ( IDAT_Header->ZlibFlagsCode ) >> 4;
									uint8 FCheck = (IDAT_Header->ZlibAdditionalFlags & 0x1F );
									uint8 FDict = (IDAT_Header->ZlibAdditionalFlags & 0x20 ) >> 5;
									uint8 FLevel = (IDAT_Header->ZlibAdditionalFlags ) >> 6;

									#if DEBUG_PNG
								Assert( Header->Width != 0 );
									Assert( Header->Height != 0 );
									Assert( Header->FilterMethod == 0 );
									Assert( Header->CompressionMethod == 0 );
									Assert( CompressionMethod == 8 );
									Assert( FDict == 0 );
									#endif
									#if BUILD_PNG
									Valid = Valid && ( Header->Width != 0 );
									Valid = Valid && ( Header->Height != 0 );
									Valid = Valid && ( Header->FilterMethod == 0 );
									Valid = Valid && ( Header->CompressionMethod == 0 );
									Valid = Valid && ( CompressionMethod == 8 );
									Valid = Valid && ( FDict == 0 );
									#endif

								}

								if( Valid )
								{
									uint8* UncompressedPixels = NULL;
									UncompressedPixels = (uint8*)malloc( Header->Width * Header->Height * BytesPerPixel );
									OutputDebugStringA("Decompressing PNG DATA...\n");

									IDAT_Chunk* Current = FirstDataChunk;
									BitStream BitData = {};
									uint32 BFINAL = 0;
									uint32 BTYPE = 0;
									while( BFINAL == 0 && Current )
									{
										BitData.Stream = Current->Data;
										BitData.StreamLen = Current->Length;

										Refill( &BitData );
										BFINAL = (uint32)Peek( &BitData, 1 );
										BTYPE = (uint32)Peek( &BitData, 3 ) >> 1;
										Consume( &BitData, 3 );

										if( BTYPE == 0 )
										{//NOTE: Uncompressed data just get
											//the length and copy it to the
											//ouput stream
											
											Flush( &BitData );
											Refill( &BitData );
											uint32 Len = (uint32)Peek( &BitData, 16 );
											uint32 NLen = (uint32)Peek( &BitData, 16 ) >> 16;
											Consume( &BitData, 32 );

											uint16 nLen = (uint16)(~NLen);

											if( Len != nLen )
											{
												OutputDebugStringA("Len != NLen!!!\n");
											}
										}
										else if( BTYPE == 3)
										{
											OutputDebugStringA("RESERVED DATA SET FOUND!!!!\n");
											Valid = false;
											break;
										}
										else
										{

											HuffmanTable HuffmanLiteralLength = AllocateHuffmanTable( 15 );
											HuffmanTable HuffmanDistance = AllocateHuffmanTable( 15 );

											if( BTYPE == 2 )
											{//NOTE: Decompress
											//the dynamic huffman table to use
											//to decompress the current data
											//block


												uint32 HLIT = (uint32)Peek( &BitData, 5 );
												uint32 HDIST = (uint32)Peek( &BitData, 10 ) >> 5;
												uint32 HCLEN = (uint32)Peek( &BitData, 14 )  >> 10;
												Consume( &BitData, 14 );

												HLIT += 257;
												HDIST++;
												HCLEN += 4;

												uint32 HCLENSwizzle[] =
													{
														16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
												};
												Assert( HCLEN < ArrayCount(HCLENSwizzle) );

												uint32 HCLENTable[ ArrayCount(HCLENSwizzle) ] = {};

												for( uint32 i = 0;
												i < HCLEN;
												++i )
												{
													if( BitData.BufferLen < 3 )
													{
														Refill( &BitData );
													}

													HCLENTable[HCLENSwizzle[i]] = (uint32)Peek( &BitData, 3 );
													Consume( &BitData, 3 );
												}

												HuffmanTable HuffmanDictionary = AllocateHuffmanTable( 8 );
												ComputeHuffman( ArrayCount(HCLENSwizzle), HCLENTable, &HuffmanDictionary );

												uint32 LiteralLengthDistanceTable[512] = {};
												uint32 TotalLength = HLIT + HDIST;
												uint32 Length = 0;

												while( Length < TotalLength )
												{
													uint32 RepeatCount = 1;
													uint32 Value = 0;
													uint32 LenCode = HuffmanDecode( &HuffmanDictionary, &BitData );
													if( BitData.BufferLen < 7 )
													{
														Refill( &BitData );
													}

													if( LenCode <= 15 )
													{
														Value = LenCode;
													}
													else if( LenCode == 16)
													{
														RepeatCount = (uint32)Peek( &BitData, 2 ) + 3;
														Consume( &BitData, 2 );
														Value = LiteralLengthDistanceTable[ Length - 1 ];
													}
													else if( LenCode == 17)
													{
														RepeatCount = (uint32)Peek( &BitData, 3 ) + 3;
														Consume( &BitData, 3 );
														Value = 0;
													}
													else if( LenCode == 18)
													{
														RepeatCount = (uint32)Peek( &BitData, 7 ) + 11;
														Consume( &BitData, 7 );
														Value = 0;
													}
													else
													{
														OutputDebugStringA("HCode Length is greater than 18!!!\n");
														#if PNG_DEBUG
														Assert( HCode < 18 );
														#endif

														Valid = false;
														break;
													}

													while( RepeatCount-- )
													{
														LiteralLengthDistanceTable[Length++] = Value;
													}
												}
												Assert( Length == TotalLength );
												ComputeHuffman( HLIT, LiteralLengthDistanceTable, &HuffmanLiteralLength );
												ComputeHuffman( HDIST, LiteralLengthDistanceTable + HLIT , &HuffmanDistance );
											}

											for(;;)
											{
												uint32 LiteralLength = HuffmanDecode( &HuffmanLiteralLength, &BitData );
												if( LiteralLength < 256 )
												{
													//TODO: Copy to output stream
													uint32 Value = LiteralLength;
													//Copy to Uncompressed Pixel
													//Data

												}
												else
												{
													if( LiteralLength == 256 )
													{
														OutputDebugStringA("End of Block\n");
														break;
													}
													else if( LiteralLength > 256 )
													{
														uint32 Length = LiteralLength - 256;
														uint32 Distance = HuffmanDecode( &HuffmanDistance, &BitData );
														for( uint32 Index = 0; Index < Length; ++Index )
														{
															//TODO: Write to
															//output stream
														}
													}
													else
													{
														Valid = false;
														break;
													}
												}
											}
										}
										Current = Current->Next;
									}

									if( RegisterClassA( &WindowClass ) )
									{
										HWND Window = CreateWindowExA
											(
												0,
												WindowClass.lpszClassName,
												"WIN64_PNG_Loader",
												WS_OVERLAPPEDWINDOW | WS_VISIBLE,
												//Size & Position
												CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
												0,
												0,
												instance,
												0
										);

										if( Window )
										{

											while( GlobalRunning )
											{
												ProcessPendingMessages();
											}
										}
									}
									else
									{ //NOTE: Failed to register window class with the OS

									}
									if( UncompressedPixels ) free( UncompressedPixels );
								}

								if( FirstDataChunk )
								{
									FreeDataChunks( FirstDataChunk );
								}
							}
						}
					}
				}
				CloseHandle( File );
				VirtualFree( FileMemory, 0, MEM_RELEASE );
			}
		}
	}

	CoUninitialize();
	return 0;
}
