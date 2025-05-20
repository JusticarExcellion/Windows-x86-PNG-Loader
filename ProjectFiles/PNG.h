#ifndef PNG_H
#define PNG_H

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

typedef struct PNG
{
	HANDLE File;
	PNG_Header* Header;
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
				c = 0xedb88320L ^ ( c >> 1 );
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

#endif
