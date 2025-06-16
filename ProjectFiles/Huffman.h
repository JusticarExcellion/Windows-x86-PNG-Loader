#ifndef HUFFMAN_H
#define HUFFMAN_H

#define HUFFMAN_MAX_BIT_COUNT 16
//NOTE: More Muratorism's should be looked at for better solutions
typedef struct HuffmanEntry
{
	uint16 Symbol;
	uint16 BitsLen;
} HuffmanEntry;

typedef struct HuffmanTable
{
	HuffmanEntry* Entries;
	uint32 EntryCount;
	int32 MaxCodeLength;
}HuffmanTable;

internal HuffmanTable
AllocateHuffmanTable( uint32 MaxCodeLen )
{
	Assert( MaxCodeLen <= HUFFMAN_MAX_BIT_COUNT );
	HuffmanTable Result = {};
	Result.MaxCodeLength = MaxCodeLen;
	Result.EntryCount = (1 << MaxCodeLen);
	Result.Entries = (HuffmanEntry*)malloc( sizeof(HuffmanEntry) * Result.EntryCount );
	return Result;
}

internal uint32
HuffmanDecode( HuffmanTable *HuffmanTable, BitStream *BitStream )
{ //NOTE: Decode will change the bitstream by refilling the buffer if the buffer
	//length is too small
	if( BitStream->BufferLen < HuffmanTable->MaxCodeLength )
	{
		Refill( BitStream );
	}
	uint32 TableIndex = (uint32)Peek( BitStream, HuffmanTable->MaxCodeLength );
	Assert( TableIndex < HuffmanTable->EntryCount );
	HuffmanEntry TableEntry = HuffmanTable->Entries[TableIndex];
	uint32 Result = TableEntry.Symbol;
	Consume( BitStream, TableEntry.BitsLen );

	Assert( TableEntry.BitsLen );

	return Result;
}

internal void
ComputeHuffman( uint32 CodeCount, uint32* CodeLength, HuffmanTable* Result , uint32 SymbolAddend = 0 )
{

	uint32 CodeLengthHist[ HUFFMAN_MAX_BIT_COUNT ] = {};
	for( uint32 SymbolIndex = 0;
		 SymbolIndex < CodeCount;
		 ++SymbolIndex )
	{
		uint32 Count = CodeLength[ SymbolIndex ];
		Assert( Count <= ArrayCount( CodeLengthHist ) );
		++CodeLengthHist[Count];
	}

	uint32 NextUnusedCode[ HUFFMAN_MAX_BIT_COUNT ];
	for( uint32 BitIndex = 1;
		 BitIndex < ArrayCount(NextUnusedCode);
		 ++BitIndex )
	{
		NextUnusedCode[ BitIndex ] = ( NextUnusedCode[ BitIndex - 1 ] +
									   CodeLengthHist[ BitIndex - 1 ] ) << 1;
	}

	for( uint32 SymbolIndex = 0;
		SymbolIndex < CodeCount;
	    ++SymbolIndex )
	{
		uint32 CodeLengthInBits = CodeLength[ SymbolIndex ];
		if( CodeLengthInBits )
		{
			Assert( CodeLengthInBits < ArrayCount( NextUnusedCode ) );
			uint32 Code = NextUnusedCode[CodeLengthInBits]++;
			uint32 UnusedBits = Result->MaxCodeLength - CodeLengthInBits;
			uint32 EntryCount = ( 1 << UnusedBits );

			for( uint32 EntryIndex = 0;
			EntryIndex < EntryCount;
			++EntryIndex )
			{
				uint32 BaseIndex = ( EntryIndex << CodeLengthInBits ) | Code;
				uint32 index = 0;
				for( int32 BitIndex = 0;
					 BitIndex <= (Result->MaxCodeLength / 2);
					 BitIndex++ )
				{
					uint32 Invert = (uint32)(Result->MaxCodeLength - ( BitIndex + 1 ) );
					index |= ( ( BaseIndex >> (uint32)BitIndex ) & 0x1 ) << Invert;
					index |= ( ( BaseIndex >> Invert ) & 0x1 ) << (uint32)BitIndex;
				}

				HuffmanEntry* Entry = Result->Entries + index;
				uint32 Symbol = SymbolIndex + SymbolAddend;
				Entry->BitsLen = (uint16)CodeLengthInBits;
				Entry->Symbol = (uint16)Symbol;

				Assert( Entry->BitsLen == CodeLengthInBits);
				Assert( Entry->Symbol == Symbol );
			}
		}
	}
}

#endif
