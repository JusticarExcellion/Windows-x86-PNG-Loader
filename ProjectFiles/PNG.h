#ifndef PNG_H
#define PNG_H

typedef struct PNG_Signature
{
	uint8 Values[8];
}PNG_Signature;

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

typedef struct ICCProfile
{
	char   ProfileName[80];
	uint8  ProfileNameLength;
	char CompressionMethod;
	uint32 CompressedProfileLength;
	char*  CompressedProfile;
} ICCProfile;

typedef struct Color
{
	uint8 Red;
	uint8 Green;
	uint8 Blue;
} Color;

typedef struct PNG_Palette
{
	Color Colors[256];
}PNG_Palette;

typedef struct PNG_ChunkHeader
{
	uint32 Length;
	union
	{
		char Type[4];
		uint32 u32Type;
	};
}PNG_ChunkHeader;

typedef struct PNG_ChunkFooter
{
	uint32 CRC;
}PNG_ChunkFooter;

global uint8 PNGSignature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

internal bool32
ValidPNGSignature( PNG_Signature* Signature )
{
		bool32 valid = true;

		int index = 0;
		while( index < 8 )
		{
				if( Signature->Values[index] != PNGSignature[index] )
				{
						valid = false;
						break;
				}
				index++;
		}
		return valid;
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
UpdateCRC( uint32 crc,  char* buffer, PNG_ChunkHeader* Header )
{
	uint32 c = crc;
	uint32 n;

	for( int i = 0; i < 4; ++i )
	{
		c = CRC_TABLE[ (c ^ Header->Type[i]) & 0xFF ] ^ ( c >> 8 );
	}
	
	for( n = 0; n < Header->Length; ++n )
	{
		c = CRC_TABLE[ (c ^ buffer[n]) & 0xFF ] ^ ( c >> 8 );
	}

	return c;
}

internal uint32
CalculateCRC( char* buffer, PNG_ChunkHeader* Header )
{
	return UpdateCRC(0xFFFFFFFFL,  buffer, Header ) ^ 0xFFFFFFFFL;
}

//******* Filtering *******//
//TODO: Filtering

inline void
SubFilter()
{
}

inline void
UpFilter()
{
}

inline void
AvgFilter()
{
}

inline void
PaethFilter()
{
}

internal int
FilterScanline( ) // Give Pixel Channels and Header
{
}
#endif
