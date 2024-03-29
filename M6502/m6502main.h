#pragma once

using Byte = unsigned char; //1byte -- 8bit
using Word = unsigned short; //2byte
using u32 = unsigned int;//4byte

static int InstructionAddr = 0xFFFC;

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
	LDA_ABS = 0xAD,
	LDA_ZP = 0xA5,
	LDA_ZPX = 0xB5

};
struct CPU
{
	/*Constructor*/
	CPU() { Reset(); }

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
	Byte N : 1, V : 1, R : 1, B : 1, D : 1, I : 1, Z : 1, C : 1;
	Byte Status;
	void setFlag(StatusBit flag,bool b)
	{
		Status |= b << flag;
	}
	void setFlag(StatusBit flag)
	{
		Status |= 1 << flag;
	}
	void setLDAStatus()
	{
		setFlag(StatusBit::Zero, A == 0);
		setFlag(StatusBit::Negative,(A & 0b10000000) > 0);
	}

	Memory memory;
	void Reset()
	{
		PC = 0xFFFC;
		SP = (Byte)0x0100 + 0xFD;
		A = X = Y = 0;
		N = V = R = B = D = I = Z = C = 0;
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
			case Instructions::LDA_ABS:
			{
				Byte low = FetchByte();
				Byte high = FetchByte();
				A = Add(low,high);
				setLDAStatus();
			}break;
			case Instructions::LDA_ZP:
			{
				Byte addr = FetchByte();
				A = FetchByte(addr);
				setLDAStatus();
			}break;
			case Instructions::LDA_ZPX:
			{
				Byte addr = FetchByte();
				A = FetchByte(Add(addr,X));
				setLDAStatus();
			}
			default:
				break;
			}
		}
	}

};