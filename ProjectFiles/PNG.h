#ifndef PNG_H
#define PNG_H

typedef struct Color
{
	char Red;
	char Green;
	char Blue;
} Color;

typedef struct PNG_Header
{
	uint32 Width;
	uint32 Height;
	char BitDepth;
	char ColorType;
	char CompressionMethod;
	char FilterMethod;
	char InterlaceMethod;
}PNG_Header;

typedef struct PNG_Palette
{
	Color Colors[256];
}PNG_Palette;

typedef struct ICCProfile
{
	char   ProfileName[80];
	uint8  ProfileNameLength;
	char CompressionMethod;
	uint32 CompressedProfileLength;
	char*  CompressedProfile;
} ICCProfile;

typedef struct PNG
{
	int64 MemorySize;
	uint32 ImageDataLength;
	HANDLE File;
	PNG_Header* Header;
	PNG_Palette* Palette;
	ICCProfile ICCProfile;
	char  FileName[ WIN32_FILE_NAME_COUNT ];
	char* ImageData;
	void* Memory;
}PNG;

internal bool32
ValidPNGSignature( uint8 *Signature, int SignatureCount )
{
		bool32 valid = true;
		if( SignatureCount > 8 || SignatureCount < 0 )
		{
				valid = false;
				return valid;
		}

		int index = 0;
		while( index < 8 )
		{
				if( Signature[index] != PNGSignature[index] )
				{
						valid = false;
						break;
				}
				index++;
		}
		return valid;
}

internal bool32
GetICCProfile( PNG* png, uint32* offset, uint32 ChunkLength )
{
	bool32 Valid = false;
	png->ICCProfile.ProfileNameLength = (uint8)StringLength( ( (char*)png->Memory + *offset ) );
	if( png->ICCProfile.ProfileNameLength < 80 )
	{
		Valid = true;
	}
	CopyString( ( (char*)png->Memory + *offset ), png->ICCProfile.ProfileNameLength, png->ICCProfile.ProfileName, 79 );

	//NOTE: This is horribly ugly but the only way I can think of to get the
	//compression method, we add one byte onto the name length to add the null
	//terminator
	png->ICCProfile.CompressionMethod = ( (char*)png->Memory + (*offset + (png->ICCProfile.ProfileNameLength + 1) ) )[0];
	//NOTE: The 2 bytes are the null terminator of the string and the compression method
	png->ICCProfile.CompressedProfileLength = ( ChunkLength - ( png->ICCProfile.ProfileNameLength + 2 ) );
	png->ICCProfile.CompressedProfile = (char*)png->Memory + (*offset + png->ICCProfile.ProfileNameLength+2 );
	return Valid;
}

inline uint32
uint32_BigEndianToLittleEndian( uint32 value )
{
	return (uint32)( ( value & 0x000000FFL ) << 24 | ( value & 0x0000FF00L ) << 8 | ( value & 0x00FF0000L ) >> 8 | ( value & 0xFF000000L ) >> 24);
}

internal void
ComputeCRCTable()
{
	uint32 c;
	int n, k;

	for( n = 0; n < 256; ++n )
	{
		c = (uint32)n;
		for( k = 0; k < 8; ++k )
		{
			if( c & 1 )
			{
				c = 0xEDB88320L ^ ( c >> 1 );
			}
			else
			{
				c = c >> 1;
			}
		}
		CRC_TABLE[n] = c;
	}
}

internal uint32
UpdateCRC( uint32 crc, char* chunkType, char* buffer, int len )
{
	uint32 c = crc;
	int n;

	for( int i = 0; i < 4; ++i )
	{
		c = CRC_TABLE[ (c ^ chunkType[i]) & 0xFF ] ^ ( c >> 8 );
	}
	
	for( n = 0; n < len; ++n )
	{
		c = CRC_TABLE[ (c ^ buffer[n]) & 0xFF ] ^ ( c >> 8 );
	}

	return c;
}

internal uint32
CalculateCRC( char* chunkType, char* buffer, int bufferLen )
{
	return UpdateCRC(0xFFFFFFFFL, chunkType , buffer, bufferLen ) ^ 0xFFFFFFFFL;
}

internal bool32
HandlePNGChunkData( PNG* png, uint32* offset, char* ChunkType, uint32 ChunkLength )
{
	bool32 Result = false;
	if( New_CompareString( "IHDR", 4, ChunkType, 4 ) )
	{
		OutputDebugStringA("Image Header Found!!!\n");
		png->Header = (PNG_Header*)( (char*)png->Memory + *offset );
		png->Header->Width = uint32_BigEndianToLittleEndian( png->Header->Width );
		png->Header->Height = uint32_BigEndianToLittleEndian( png->Header->Height );
		Result = true;
	}

	if( New_CompareString( "PLTE", 4, ChunkType, 4 ) )
	{
		OutputDebugStringA("Image Palette Found!!!\n");
		Result = true;
	}

	if( New_CompareString( "iCCP", 4, ChunkType, 4 ) )
	{
		OutputDebugStringA("Embedded ICC Profile Found!!!\n");

		if( GetICCProfile( png, offset, ChunkLength ) )
		{
			Result = true;
		}
	}

	if( New_CompareString( "IDAT", 4, ChunkType, 4 ) )
	{
		if( png->ImageData == NULL )
		{
			png->ImageData = (char*)png->Memory + *offset;
		}
		png->ImageDataLength += ChunkLength;

		OutputDebugStringA("Image Data Found!!!\n");
		Result = true;
	}

	if( New_CompareString( "IEND", 4, ChunkType, 4 ) )
	{
		OutputDebugStringA("End of Image!!!\n");
		Result = false;
	}
	else
	{
		Result = true;
	}

	return Result;
}

internal bool32
ReadChunk( PNG* png, uint32 *offset, DWORD* BytesRead )
{
	bool32 Result = false;
	uint32 B_ChunkLength = 0;
	uint32 L_ChunkLength = 0;

	Assert( *offset < png->MemorySize ); //NOTE: Asserting we never are writing
	//into memory that was not virtual allocated
	
	if( ReadFile( png->File, &B_ChunkLength, 4, BytesRead, 0 ) )
	{
		L_ChunkLength = uint32_BigEndianToLittleEndian( B_ChunkLength );

		Assert( L_ChunkLength < 2147483647 );
		Assert( (*offset + L_ChunkLength) < png->MemorySize ); // NOTE: Ensures that we never write to unallocated memory

		char ChunkType[5] = {};
		ChunkType[4] = '\0';
		if( ReadFile( png->File, &ChunkType, 4, BytesRead, 0 ) )
		{
			char TextBuffer[256] = {};
			_snprintf_s( TextBuffer,sizeof(TextBuffer), "Chunk Type %s\n", ChunkType);
			OutputDebugStringA( TextBuffer );

			if( ReadFile( png->File, ( (char*)png->Memory + *offset ), L_ChunkLength, BytesRead, 0 ) )
			{
				OutputDebugStringA("Chunk Data Read\n");

				uint32 L_ReadCRC = 0;
				uint32 B_ReadCRC = 0;
				if( ReadFile( png->File, &B_ReadCRC, 4, BytesRead, 0 ) )
				{
					L_ReadCRC = uint32_BigEndianToLittleEndian( B_ReadCRC );

					uint32 CalculatedCRC = CalculateCRC( ChunkType, ( (char*)png->Memory + *offset), L_ChunkLength );
					if( L_ReadCRC != CalculatedCRC )
					{//NOTE: Data is corrupted 
						//Error / crash
						//application
						OutputDebugStringA("CRC Check Failed - Chunk Data Invalid!!!\n");
					}
					else
					{
						OutputDebugStringA("CRC Check Succeeded - Chunk Data Valid!!!\n");
						Result = HandlePNGChunkData( png, offset, ChunkType, L_ChunkLength );
					}
				}
			}
			else
			{
				//TODO: Log Fail
			}

		}
		else
		{
			//TODO: Log Fail
		}

		*offset += L_ChunkLength;
	}
	else
	{
		//TODO: Log Fail
	}

	return Result;
}

internal bool32
ValidatePNG( PNG* png )
{
	bool32 Valid = true;

	if( png->MemorySize < 1)
	{
		Valid = false;
	}

	if( png->ImageDataLength < 1)
	{
		Valid = false;
	}

	uint8 BitDepth = (uint8)png->Header->BitDepth;
	switch( png->Header->ColorType )
	{
		case 2:
			Valid = (BitDepth == 8 || BitDepth == 16);
		break;
		case 3:
			Valid = (BitDepth == 1 || BitDepth == 2 || BitDepth == 4 || BitDepth == 8);
		break;
		case 4:
			Valid = (BitDepth == 8 || BitDepth == 16);
		break;
		case 6:
			Valid = (BitDepth == 8 || BitDepth == 16);
		break;
	}

	if( png->Header == NULL )
	{
		Valid = false;
	}

	if( png->ImageData == NULL )
	{
		Valid = false;
	}

	if( png->Memory == NULL )
	{
		Valid = false;
	}

	return Valid;
}
#endif
