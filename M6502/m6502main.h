#pragma once
#ifndef CATCH2
#include <iostream>
#endif
#include "definitions.h"
#include "memory.h"

struct CPU
{
	static CPU& GetCPU()
	{
		static CPU    instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}

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

	void printStatus() const
	{
		std::string status = std::string("CPU status report: \n")
			+= std::string("Registers: \n")
			+= std::string("A: ") += std::to_string(A)
			+= std::string("   X: ") += std::to_string(X)
			+= std::string("   Y: ") += std::to_string(Y)
			+= std::string("\n Program Counter: ") += std::to_string(PC)
			+= std::string("\n Stack Pointer: ") += std::to_string(SP)
			+= std::string("\n Status: ") += std::to_string(Status)
			+= std::string("\n Total Cycles: ") += std::to_string(cycles);

		std::cout << status << "\n";
	}

	void Execute()
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
				A = FetchByte((Word)(addr + high));
				if (255 < addr)
				{
					/*page cross, since we overflew and went back to 0*/
					/*we can also check if low + X > 0xff, same thing*/
					cycles++;
				}
				setLDAStatus();
			}break;
			case Instructions::LDA_ABSY:
			{
				Byte low = FetchByte();
				Byte high = FetchByte();
				Word addr = low + Y;
				A = FetchByte((Word)(addr + high));
				if (255 < addr)
				{
					/*page cross, since we overflew and went back to 0*/
					/*we can also check if low + X > 0xff, same thing*/
					cycles++;
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
				Byte effectiveAddr = FetchByte(addr) + X;
				A = FetchByte(effectiveAddr);
				setLDAStatus();
			}break;
			case Instructions::LDA_ZPX_IND:
			{
				Byte pointerToAddr = FetchByte();
				Byte addrToRead = FetchByte(Byte(pointerToAddr + X));
				Byte lowAddr = FetchByte(Byte(addrToRead));
				Byte highAddr = FetchByte(Byte(addrToRead + 1));
				Word effectiveAddr = (Word)highAddr + Word(lowAddr);
				A = FetchByte(effectiveAddr);
				setLDAStatus();
			}break;
			case Instructions::LDA_ZPY_IND:
			{
				Byte pointerToAddr = FetchByte();
				Byte addrToRead = FetchByte(Byte(pointerToAddr + Y));
				Byte lowAddr = FetchByte(Byte(addrToRead));
				Byte highAddr = FetchByte(Byte(addrToRead + 1));
				Word effectiveAddr = (Word)highAddr + Word(lowAddr);
				A = FetchByte(effectiveAddr);
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