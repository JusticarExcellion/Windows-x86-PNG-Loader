#ifndef BITREADER_H
#define BITREADER_H

typedef struct BitStream
{
	uint8 *Stream;
	uint32 StreamLen;

	uint64 Buffer;
	int32  BufferLen;
}BitStream;

inline uint64
ReadLE_64( uint8* Data )
{
	uint64 Result = (uint64)( ( Data[3] << 24 ) | ( Data[2] << 16 ) | ( Data[1] << 8 ) | ( *Data ) );
	return Result;
}

internal void
Refill( BitStream *BitStream )
{
	BitStream->Buffer |= ReadLE_64( BitStream->Stream ) << BitStream->BufferLen;
	BitStream->Stream += ( 63 - BitStream->BufferLen ) >> 3;
	BitStream->BufferLen |= 56;
}

internal uint64
Peek( BitStream *BitStream, int count )
{
	Assert( count > 0 && count < 57 );
	Assert( count <= BitStream->BufferLen );
	return BitStream->Buffer & ( ( 1ull << count ) - 1 );
}

internal void
Consume( BitStream *BitStream, int count )
{
	Assert( count <= BitStream->BufferLen );
	BitStream->Buffer >>= count;
	BitStream->BufferLen -= count;
}

internal void
Flush( BitStream *BitStream )
{
	BitStream->Buffer = 0;
	BitStream->BufferLen = 0;
}

internal uint32
ConsumeBits( BitStream *BitStream, int count )
{
	Assert( count <= 32 );
	uint32 Result = 0;
	while( (BitStream->BufferLen < count) && (BitStream->StreamLen > 0 ) )
	{
		uint32 Byte = (uint32)BitStream->Stream[0];
		BitStream->Stream += sizeof(uint8);
		BitStream->StreamLen-=sizeof(uint8);
		BitStream->Buffer |= (Byte << BitStream->BufferLen );
		BitStream->BufferLen +=8;
	}

	if( BitStream->BufferLen >= count )
	{
		BitStream->BufferLen -= count;
		Result = BitStream->Buffer & ( (1ull << count) - 1);
		BitStream->Buffer >>= count;
	}

	return Result;
}
#endif
