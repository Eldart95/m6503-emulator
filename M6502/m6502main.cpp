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
        REQUIRE(cpu.PC == StartupAddr);
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
            cpu.memory[StartupAddr] = Instructions::LDA_IM;
            cpu.memory[StartupAddr + 1] = 0x42;
            cpu.Execute(Instructions::LDA_IM);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x42);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 2);
            }
            cpu.Reset();
        }
        WHEN("value of a is ZERO")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_IM;
            cpu.memory[StartupAddr + 1] = 0x00;
            cpu.Execute(Instructions::LDA_IM);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 2);
            }
            cpu.Reset();
        }
    }
    GIVEN("reading instruction LDA_ABSOLUTE")
    {
        WHEN("value of a is not zero")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABS;
            cpu.memory[StartupAddr + 1] = 0xA1;
            cpu.memory[StartupAddr + 2] = 0xB;
            cpu.memory[0xA1 + 0xB] = 0xA;
            cpu.Execute(Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 10);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        
        WHEN("value of a is ZERO")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABS;
            cpu.memory[StartupAddr + 1] = 0xBB;
            cpu.memory[StartupAddr + 2] = 0x00;
            cpu.memory[0xBB + 0x00] = 0;
            cpu.Execute(Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        
        WHEN("value of a is NEGATIVE")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABS;
            cpu.memory[StartupAddr + 1] = 0x12;
            cpu.memory[StartupAddr + 2] = 0x21;
            cpu.memory[0x12 + 0x21] = 0xFD;
            cpu.Execute(Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0xFD);
                REQUIRE(cpu.Status == 128);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("value of a is POSTIVE")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABS;
            cpu.memory[StartupAddr + 1] = 0x19;
            cpu.memory[StartupAddr + 2] = 0x06;
            cpu.memory[0x19 + 0x06] = 0x1F;
            cpu.Execute(Instructions::LDA_ABS);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x1F);
                REQUIRE(cpu.GetStatus() == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        
    }
    GIVEN("reading instruction LDA_ABSOLUT_X")
    {
        WHEN("no page cross")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSX;
            cpu.X = 0x5f;
            cpu.memory[StartupAddr + 1] = 0x0;
            cpu.memory[StartupAddr + 2] = 0x0;
            cpu.memory[cpu.X + 0x0 + 0x0] = 0x3d;
            cpu.Execute(Instructions::LDA_ABSX);
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0x5f);
                REQUIRE(cpu.A == 0x3d);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, val is 0")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSX;
            cpu.X = 0x5f;
            cpu.memory[StartupAddr + 1] = 0x0;
            cpu.memory[StartupAddr + 2] = 0x0;
            cpu.memory[cpu.X + 0x0 + 0x0] = 0x0;
            cpu.Execute(Instructions::LDA_ABSX);
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0x5f);
                REQUIRE(cpu.A == 0x0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, val is positive")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSX;
            cpu.X = 0xa2;
            cpu.memory[StartupAddr + 1] = 0x02;
            cpu.memory[StartupAddr + 2] = 0x0;
            cpu.memory[cpu.X + 0x02 + 0x0] = 0xa4;
            cpu.Execute(Instructions::LDA_ABSX);
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0xa2);
                REQUIRE(cpu.A == 0xa4);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }

    }
    GIVEN("reading instruction LDA_ZERO_PAGE")
    {
        WHEN("giving zero page addr")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ZP;
            cpu.memory[StartupAddr + 1] = 0x10;
            cpu.memory[0x10] = 0x05;
            cpu.Execute(Instructions::LDA_ZP);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 5);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 3);
            }
            cpu.Reset();
        }        
        WHEN("giving zero page addr with zero")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ZP;
            cpu.memory[StartupAddr + 1] = 0x01;
            cpu.memory[0x01] = 0x00;
            cpu.Execute(Instructions::LDA_ZP);
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 3);
            }
            cpu.Reset();
        }
    }
}


