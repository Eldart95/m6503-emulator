#define CATCH_CONFIG_MAIN 
#include "catch2.hpp"
#include "m6502main.h"

TEST_CASE("sizeof types test", "cpu") 
{
    REQUIRE(sizeof(Byte)==1);
    REQUIRE(sizeof(Word)==2);
    REQUIRE(sizeof(u32) == 4);

    CPU cpu;
    cpu.Reset();

    REQUIRE(sizeof(cpu.A) == 1);
    REQUIRE(sizeof(cpu.X) == 1);
    REQUIRE(sizeof(cpu.Y) == 1);
    REQUIRE(sizeof(cpu.SP) == 1);
    REQUIRE(sizeof(cpu.Status) == 1);
    REQUIRE(sizeof(cpu.PC) == 2);

}

SCENARIO("cpu reset")
{
    GIVEN("reset")
    {
        CPU cpu;
        cpu.Reset();

        REQUIRE(cpu.SP == (Byte)0x0100 + 0xFD);
        REQUIRE(cpu.PC == 0xFFFC);
        REQUIRE(cpu.Status == 0);
    }
}

SCENARIO("flag settings")
{
    CPU cpu;
    cpu.Reset();
    GIVEN("setting negative flag")
    {
        cpu.setFlag(cpu.StatusBit::Carry);
        REQUIRE(cpu.Status == 0x01);
    }
    cpu.Reset();
    GIVEN("setting zero flag")
    {
        cpu.setFlag(cpu.StatusBit::Zero);
        REQUIRE(cpu.Status == 0x02);
    }
    cpu.Reset();
    GIVEN("setting interrupt flag")
    {
        cpu.setFlag(cpu.StatusBit::Interrupt);
        REQUIRE(cpu.Status == 4);
    }
    cpu.Reset();
    GIVEN("setting decimal flag")
    {
        cpu.setFlag(cpu.StatusBit::Decimal);
        REQUIRE(cpu.Status == 8);
    }
    cpu.Reset();
    GIVEN("setting break flag")
    {
        cpu.setFlag(cpu.StatusBit::Break);
        REQUIRE(cpu.Status == 16);
    }
    cpu.Reset();
    GIVEN("setting overflow flag")
    {
        cpu.setFlag(cpu.StatusBit::Overflow);
        REQUIRE(cpu.Status == 64);
    }
    cpu.Reset();
    GIVEN("setting negative flag")
    {
        cpu.setFlag(cpu.StatusBit::Negative);
        REQUIRE(cpu.Status == 128);
    }
    cpu.Reset();
}

SCENARIO("Reading instructions")
{
    CPU cpu;
    GIVEN("reading instruction LDA_IMMEDIATE")
    {
        WHEN("value of a is not zero")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_IM;
            cpu.memory[0xFFFD] = 0x42;
            cpu.Execute(2, Instructions::LDA_IM);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x42);
                REQUIRE(cpu.Status == 0);
            }
            cpu.Reset();
        }
        WHEN("value of a is ZERO")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_IM;
            cpu.memory[0xFFFD] = 0x00;
            cpu.Execute(2, Instructions::LDA_IM);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
            }
            cpu.Reset();
        }
    }
    GIVEN("reading instruction LDA_ABSOLUTE")
    {
        WHEN("value of a is not zero")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_ABS;
            cpu.memory[0xFFFD] = 0x01;
            cpu.memory[0xFFFE] = 0x01;
            cpu.Execute(4, Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x02);
                REQUIRE(cpu.Status == 0);
            }
            cpu.Reset();
        }
        WHEN("value of a is ZERO")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_ABS;
            cpu.memory[0xFFFD] = 0x00;
            cpu.memory[0xFFFE] = 0x00;
            cpu.Execute(4, Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x00);
                REQUIRE(cpu.Status == 2);
            }
            cpu.Reset();
        }
        WHEN("value of a is NEGATIVE")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_ABS;
            cpu.memory[0xFFFD] = 0x64;
            cpu.memory[0xFFFE] = 0x64;
            cpu.Execute(4, Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0xC8);
                REQUIRE(cpu.Status == 128);
            }
            cpu.Reset();
        }
        WHEN("value of a is POSTIVE (7th bit is 0)")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_ABS;
            cpu.memory[0xFFFD] = 0x79;
            cpu.memory[0xFFFE] = 0x06;
            cpu.Execute(4, Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x7F);
                REQUIRE(cpu.Status == 0);
            }
            cpu.Reset();
        }
    }
    GIVEN("reading instruction LDA_ZERO_PAGE")
    {
        WHEN("giving zero page addr")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_ZP;
            cpu.memory[0xFFFD] = 0x10;
            cpu.memory[0x10] = 0x05;
            cpu.Execute(3, Instructions::LDA_ZP);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 5);
                REQUIRE(cpu.Status == 0);
            }
            cpu.Reset();
        }        
        WHEN("giving zero page addr with zero")
        {
            cpu.memory[InstructionAddr] = Instructions::LDA_ZP;
            cpu.memory[0xFFFD] = 0x01;
            cpu.memory[0x01] = 0x00;
            cpu.Execute(3, Instructions::LDA_ZP);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
            }
            cpu.Reset();
        }
    }
}


