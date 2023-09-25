#pragma once

using Byte = unsigned char; //1byte
using Word = unsigned short; //2byte
using u32 = unsigned int;//4byte

struct Memory
{
	static constexpr u32 stackSize = 65536;
	Byte Data[stackSize];

	Byte operator[](u32 index) const
	{
		return Data[index];
	}
	Byte& operator[](u32 index)
	{
		return Data[index];
	}

	void Initialise()
	{
		memset(Data, 0, stackSize);
	}
};

enum Instructions
{
	LDA_IM = 0xA9,
	LDA_ZP = 0xA5,
	LDA_ZPX = 0xB5

};
struct CPU
{
	/*Registers*/
	Byte A, X, Y;

	/*Stack pointer*/
	Byte SP;

	/*Program counter*/
	Word PC;

	/*Cycles*/
	u32 cycles;

	/*Status*/
	enum StatusBit
	{
		Carry = 0x00,
		Zero = 0x01,
		Interrupt = 0x02,
		Decimal = 0x03,
		Break = 0x04,
		Reserve = 0x05,
		Overflow = 0x06,
		Negative = 0x07,
	};
	/*
	[N - Negative,V - Overflow,R - Reseve,B - Break,D - Decimal,I - Interrupt,Z - Zero,C - Carry]
	______7______,______6_____,_____5_____,____4____,_____3_____,_______2_____,____1___,____0____
	*/
	Byte Status;
	void setFlag(StatusBit flag)
	{
		Status |= 1 << flag;
	}
	void setFlag(StatusBit flag, Byte otherFlag)
	{
		Status |= otherFlag << flag;
	}
	void setLDAStatus()
	{
		if(A==0)setFlag(StatusBit::Zero);
		setFlag(StatusBit::Negative,A);
	}

	Memory memory;
	void Reset()
	{
		PC = 0xFFFC;
		SP = (Byte)0x0100 + 0xFD;
		A = X = Y = 0;
		Status = 0;
		memory.Initialise();
		cycles = 0;
	}

	Byte FetchByte()
	{
		cycles--;
		return memory[PC++];
	}

	Byte FetchByte(Byte addr)
	{
		cycles--;
		return memory[addr];
	}

	template <typename T>
	T Add(const T& b1, const T& b2)
	{
		cycles--;
		return b1 + b2;
	}

	void Execute(u32 in_cycles, Instructions instruction)
	{
		cycles = in_cycles;
		while (cycles > 0)
		{
			Byte ins = FetchByte();
			switch (ins)
			{
			case Instructions::LDA_IM:
			{
				A = FetchByte();
				setLDAStatus();
			}break;
			case Instructions::LDA_ZP:
			{
				Byte addr = FetchByte();
				A = FetchByte(addr);
			}break;
			case Instructions::LDA_ZPX:
			{
				Byte addr = FetchByte();
				A = FetchByte(Add(addr,X));
			}
			default:
				break;
			}
		}
	}

};