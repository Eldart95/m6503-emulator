#pragma once
#include "definitions.h"
struct Memory
{
	static constexpr u32 stackSize = 65536;
	Byte Data[stackSize];

	Byte operator[](u32 index) const;
	Byte& operator[](u32 index);

	void Initialise();
};
