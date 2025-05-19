#ifndef PNG_H
#define PNG_H

typedef struct PNG
{
		HANDLE File;
}PNG;

global uint8 PNGSignature[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

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
#endif
