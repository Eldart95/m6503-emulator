#pragma once
#ifndef CATCH2
#include <iostream>
#endif
#include <string>
using Byte = unsigned char; //1byte -- 8bit
using Word = unsigned short; //2byte
using u32 = unsigned int;//4byte

static Word StartupAddr = 0xE000;
static Byte LowStartupAddr = 0x00;
static Byte HighStartupAddr = 0xE0;

enum Instructions
{
	LDA_IM = 0xA9, //Immediate
	LDA_ABS = 0xAD, //Absolute 
	LDA_ZP = 0xA5, //Zero Page
	LDA_ZPX = 0xB5, //X-Indexed Zero Page
	LDA_ABSX = 0xBD, //X-Indexed Absolute
	LDA_ABSY = 0xB9, //Y-Indexed Absolute
	LDA_ZPX_IND = 0xA1, //X-Indexed Zero Page Indirect
	LDA_ZPY_IND = 0xB1, //Y-Indexed Zero Page Indirect
};