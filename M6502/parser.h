#pragma once
#include "m6502main.h"
#include "definitions.h"

std::pair<std::string, std::string> get_instruction_and_operands(std::string& in_str)
{
    std::string first, second;
    std::string::size_type pos = in_str.find(' ', 0);

    second = in_str.substr(pos + 1);
    first = in_str.substr(0, pos);

    return { first,second };
}

enum Instruction string_to_instruction(std::string& in_str)
{
    
}


class parser
{
public:
	static void parse(std::string& str)
	{
        std::pair<std::string, std::string> statement = get_instruction_and_operands(str);
        
		//CPU& cpu = CPU::GetCPU();
		//cpu.memory[cpu.PC] = Instructions::LDA_ABS;
	}

    
};
