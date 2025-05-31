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
	uint32 LengthOfStream;
	int32 BitCount;
	uint64 BitBuffer;
} BitStream;

internal uint64
ReadLE_U64( BitStream *BitStream )
{
	return (uint64)( BitStream->Stream[3] << 24 ) | ( BitStream->Stream[2] << 16 ) | ( BitStream->Stream[1] << 8 ) | (BitStream->Stream[0] );
}

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
#endif
