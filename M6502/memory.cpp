#include "memory.h"
#include <string>
void Memory::Initialise()
{
	memset(Data, 0, stackSize);
}

Byte& Memory::operator[](u32 index)
{
	return Data[index];
}

Byte Memory::operator[](u32 index) const
{
	return Data[index];
}