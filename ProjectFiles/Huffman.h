#ifndef HUFFMAN_H
#define HUFFMAN_H

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
	Assert( MaxCodeLen <= 16 );
	HuffmanTable Result = {};
	Result.EntryCount = (1 << MaxCodeLen);
	Result.Entries = (HuffmanEntry*)malloc( sizeof(HuffmanEntry) * Result.EntryCount );
	return Result;
}

internal void
CreateHuffmanTable( uint32 TableLen, uint32 *Table, HuffmanTable *HuffmanTable )
{
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
	return Result;
}

#endif
