#ifndef STRING_UTILS_H
#define STRING_UTILS_H

internal bool32
New_CompareString( char* String1, int String1Count, char* String2, int String2Count )
{ //NOTE: We Could rework this to look for the Null terminator instead of
	//relying on the string count and we can say that if one string is longer
	//than the other we just return false automatically
	bool32 Result = false;
	for( int i = 0; i < String1Count && i < String2Count; i++ )
	{
		if( String1[i] != String2[i] )
		{
			break;
		}
		else if( ( i == ( String1Count - 1 ) ) && ( i == (String2Count - 1) ) )
		{
			Result = true;
		}
	}
	return Result;
}

internal bool32
CopyString( char* CopyStr, int CpStrLen, char* Dest, int DstLen )
{   //NOTE: Does not assume that either string is terminated with a null
	//character or that they are even the same length
	bool32 Valid = false;
	if( DstLen >= CpStrLen && CpStrLen > 0 ) Valid = true;
	int index = 0;
	while( ( index < CpStrLen ) && ( index < DstLen ) && Valid )
	{
		*Dest++ = *CopyStr++;
		index++;
	}
	*Dest++ = 0;
	return Valid;
}

inline uint32
StringLength( char* String )
{   //NOTE: Assumes the string being given is null terminated
	uint32 Count = 0;
	while( *String++ )
	{
		Count++;
	}

	return Count;
}

#endif
