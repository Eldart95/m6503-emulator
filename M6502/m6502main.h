#pragma once

using Byte = unsigned char; //1byte -- 8bit
using Word = unsigned short; //2byte
using u32 = unsigned int;//4byte

static Word StartupAddr = 0xE000;
static Byte LowStartupAddr = 0x00;
static Byte HighStartupAddr = 0xE0;

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
	LDA_ZPX = 0xB5,
	LDA_ABSX = 0xBD

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
		Z = (A >> 7 | 0);
		N = (A << 7) & 1;
		setFlag(StatusBit::Zero, A == 0);
		setFlag(StatusBit::Negative,(A & 0b10000000) > 0);
	}
	Byte GetStatus()
	{
		return N | V | R | B | D | I | Z | C;
	}

	Memory memory;
	void Reset()
	{
		memory.Initialise();
		PC = 0xFFFC; // then also read FFFD 
		memory[PC] = LowStartupAddr;
		PC++; //FFFD
		memory[PC] = HighStartupAddr;
		
		PC = StartupAddr;
		cycles = 0;
		SP = (Byte)0x0100 + 0xFD;
		A = X = Y = 0;
		N = V = R = B = D = I = Z = C = 0;
		Status = 0;
	}

	Byte FetchByte()
	{
		cycles++;
		return memory[PC++];
	}

	Byte FetchByte(Byte addr)
	{
		cycles++;
		return memory[addr];
	}

	Byte FetchByte(Word addr)
	{
		cycles++;
		return memory[addr];
	}

	template <typename T>
	T Add(const T& b1, const T& b2)
	{
		cycles++;
		return b1 + b2;
	}

	void Execute(Instructions instruction)
	{
		bool run = true;
		while (run)
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
				Word addr = low + high;
				A = FetchByte(addr);
				setLDAStatus();
			}break;
			case Instructions::LDA_ABSX:
			{
				Byte low = FetchByte();
				Byte high = FetchByte();
				Word addr = low + X;
				A = FetchByte(addr);
				if (addr < low)
				{
					/*page cross, since we overflew and went back to 0*/
					/*we can also check if low + X > 0xff, same thing*/
					Word effectiveAddr = Word(high) << 8 | low;
					A = FetchByte(effectiveAddr);
				}
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
				A = FetchByte(Add(addr, X));
				setLDAStatus();
			}break;
			default:
				run = false;
				cycles--; // fetched the last byte "for nothing"
				break;
			}
		}
	}

};