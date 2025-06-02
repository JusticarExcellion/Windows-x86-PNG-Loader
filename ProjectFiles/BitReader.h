#ifndef BIT_READER_H
#define BIT_READER_H

// NOTE: This code is sourced and inspired by Fabien "ryg" Giesen, and his blog post
// "Reading bits in far too many ways (part 2)", particularly his section
// discussing the fourth variant on the subject.
// This code is far too smart for me but at least I am smart enough to use it.
// - Xander

typedef struct BitStream
{
	uint8 *Stream;
	uint32 Length;
	int32 BitCount;
	uint32 BitBuffer;
} BitStream;

/*
internal void
Refill_LSB( BitStream *BitStream )
{
	BitStream->BitBuffer |= ReadLE_U64( BitStream ) << BitStream->BitCount;
	BitStream->Stream += ( 63 - BitStream->BitCount ) >> 3;
	BitStream->BitCount |= 56;
}

internal uint64
Peek_LSB( BitStream *BitStream, int count )
{
	Assert( count >= 0 && count <= 56 );
	Assert( count <= BitStream->BitCount );
	return BitStream->BitBuffer & ( ( 1ull << count ) - 1 );
}

internal void
Consume_LSB( BitStream *BitStream , int count )
{
	Assert( count <= BitStream->BitCount );
	BitStream->BitBuffer >>= count;
	BitStream->BitCount -= count;
}
*/

internal uint32
ConsumeBits( BitStream *BitStream, int count )
{
	Assert( count <= 32 );
	uint32 Result = 0;
	while( (BitStream->BitCount < count) && (BitStream->Length > 0 ) )
	{
		uint32 Byte = (uint32)BitStream->Stream[0];
		BitStream->Stream += sizeof(uint8);
		BitStream->Length-=sizeof(uint8);
		BitStream->BitBuffer |= (Byte << BitStream->BitCount);
		BitStream->BitCount+=8;
	}

	if( BitStream->BitCount >= count )
	{
		BitStream->BitCount -= count;
		Result = BitStream->BitBuffer & ( (1ull << count) - 1);
		BitStream->BitBuffer >>= count;
	}

	return Result;
}

internal void
FlushBitBuffer( BitStream *BitStream )
{
	BitStream->BitBuffer = 0;
	BitStream->BitCount = 0;
}
#endif
