#ifdef CATCH2
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.Execute();
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
            cpu.X = 0x3e;
            cpu.memory[StartupAddr + 1] = 0x02; //low
            cpu.memory[StartupAddr + 2] = 0x0; //high
            cpu.memory[cpu.X + 0x02 + 0x0] = 0x4f;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0x3e);
                REQUIRE(cpu.A == 0x4f);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, 8th bit is 1")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSX;
            cpu.X = 0x3e;
            cpu.memory[StartupAddr + 1] = 0x02; //low
            cpu.memory[StartupAddr + 2] = 0x0; //high
            cpu.memory[cpu.X + 0x02 + 0x0] = 0xa4;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0x3e);
                REQUIRE(cpu.A == 0xa4);
                REQUIRE(cpu.Status == 128);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, not trivial")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSX;
            cpu.X = 0x0;
            cpu.memory[StartupAddr + 1] = 0x81; //low
            cpu.memory[StartupAddr + 2] = 0x3d; //high
            cpu.memory[cpu.X + 0x3d + 0x81] = 0x7a;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0x00);
                REQUIRE(cpu.A == 0x7a);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("page cross")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSX;
            cpu.X = 0xff;
            cpu.memory[StartupAddr + 1] = 0xff; //low
            cpu.memory[StartupAddr + 2] = 0x3d; //high
            cpu.memory[cpu.X + 0x3d + 0xff] = 0x5b;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.X == 0xff);
                REQUIRE(cpu.A == 0x5b);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 5);
            }
            cpu.Reset();
        }

    }
    GIVEN("reading instruction LDA_ABSOLUT_Y")
    {
        WHEN("no page cross")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSY;
            cpu.Y = 0x5f;
            cpu.memory[StartupAddr + 1] = 0x0;
            cpu.memory[StartupAddr + 2] = 0x0;
            cpu.memory[cpu.Y + 0x0 + 0x0] = 0x3d;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.Y == 0x5f);
                REQUIRE(cpu.A == 0x3d);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, val is 0")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSY;
            cpu.Y = 0x5f;
            cpu.memory[StartupAddr + 1] = 0x0;
            cpu.memory[StartupAddr + 2] = 0x0;
            cpu.memory[cpu.Y + 0x0 + 0x0] = 0x0;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.Y == 0x5f);
                REQUIRE(cpu.A == 0x0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, val is positive")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSY;
            cpu.Y = 0x3e;
            cpu.memory[StartupAddr + 1] = 0x02; //low
            cpu.memory[StartupAddr + 2] = 0x0; //high
            cpu.memory[cpu.Y + 0x02 + 0x0] = 0x4f;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.Y == 0x3e);
                REQUIRE(cpu.A == 0x4f);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, 8th bit is 1")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSY;
            cpu.Y = 0x3e;
            cpu.memory[StartupAddr + 1] = 0x02; //low
            cpu.memory[StartupAddr + 2] = 0x0; //high
            cpu.memory[cpu.Y + 0x02 + 0x0] = 0xa4;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.Y == 0x3e);
                REQUIRE(cpu.A == 0xa4);
                REQUIRE(cpu.Status == 128);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("no page cross, not trivial")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSY;
            cpu.Y = 0x0;
            cpu.memory[StartupAddr + 1] = 0x81; //low
            cpu.memory[StartupAddr + 2] = 0x3d; //high
            cpu.memory[cpu.Y + 0x3d + 0x81] = 0x7a;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.Y == 0x00);
                REQUIRE(cpu.A == 0x7a);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("page cross")
        {
            cpu.memory[StartupAddr] = Instructions::LDA_ABSY;
            cpu.Y = 0xff;
            cpu.memory[StartupAddr + 1] = 0xff; //low
            cpu.memory[StartupAddr + 2] = 0x3d; //high
            cpu.memory[cpu.Y + 0x3d + 0xff] = 0x5b;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.Y == 0xff);
                REQUIRE(cpu.A == 0x5b);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 5);
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
            cpu.Execute();
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
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 3);
            }
            cpu.Reset();
        }
    }
    GIVEN("reading instruction LDA_ZERO_PAGE_X")
    {
        WHEN("giving zero page addr")
        {
            cpu.X = 0x2;
            cpu.memory[StartupAddr] = Instructions::LDA_ZPX;
            cpu.memory[StartupAddr + 1] = 0x10; //addr
            cpu.memory[0x10] = 0x05; //effective addr
            cpu.memory[0x05 + cpu.X] = 0x05;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0x05);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
        WHEN("giving zero page addr with zero")
        {
            cpu.X = 0x1;
            cpu.memory[StartupAddr] = Instructions::LDA_ZPX;
            cpu.memory[StartupAddr + 1] = 0x01;
            cpu.memory[0x01] = 0x01;
            cpu.memory[0x01 + cpu.X] = 0x01;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 1);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 4);
            }
            cpu.Reset();
        }
    }
    GIVEN("reading instruction LDA_ABSOLUT_X_INDIRECT")
    {
        WHEN("simple case")
        {
            cpu.X = 0x01;
            cpu.memory[StartupAddr] = Instructions::LDA_ZPX_IND;
            cpu.memory[StartupAddr + 1] = 0x10; //ptr to addr
            cpu.memory[0x10 + cpu.X] = 0x05; //addr to read
            cpu.memory[0x05] = 0x05;
            cpu.memory[0x06] = 0x05;
            cpu.memory[0xA] = 5;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 5);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 6);
            }
            cpu.Reset();
        }
        WHEN("another simple case")
        {
            cpu.X = 0x2A;
            cpu.memory[StartupAddr] = Instructions::LDA_ZPX_IND;
            cpu.memory[StartupAddr + 1] = 0xAA; //ptr to addr
            cpu.memory[0xAA + cpu.X] = 0x4B; //addr to read
            cpu.memory[0x4B] = 0x1C;
            cpu.memory[0x4C] = 0xC;
            cpu.memory[0x28] = 0;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 6);
            }
            cpu.Reset();
        }
    }
    GIVEN("reading instruction LDA_ABSOLUT_Y_INDIRECT")
    {
        WHEN("simple case")
        {
            cpu.Y = 0x01;
            cpu.memory[StartupAddr] = Instructions::LDA_ZPY_IND;
            cpu.memory[StartupAddr + 1] = 0x10; //ptr to addr
            cpu.memory[0x10 + cpu.Y] = 0x05; //addr to read
            cpu.memory[0x05] = 0x05;
            cpu.memory[0x06] = 0x05;
            cpu.memory[0xA] = 5;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 5);
                REQUIRE(cpu.Status == 0);
                REQUIRE(cpu.cycles == 6);
            }
            cpu.Reset();
        }
        WHEN("another simple case")
        {
            cpu.Y = 0x2A;
            cpu.memory[StartupAddr] = Instructions::LDA_ZPY_IND;
            cpu.memory[StartupAddr + 1] = 0xAA; //ptr to addr
            cpu.memory[0xAA + cpu.Y] = 0x4B; //addr to read
            cpu.memory[0x4B] = 0x1C;
            cpu.memory[0x4C] = 0xC;
            cpu.memory[0x28] = 0;
            cpu.Execute();
            THEN("state of cpu")
            {
                REQUIRE(cpu.A == 0);
                REQUIRE(cpu.Status == 2);
                REQUIRE(cpu.cycles == 6);
            }
            cpu.Reset();
        }
    }
}


#else
#include "m6502main.h"
#include "parser.h"

int main(int argc, char* argv[])
{
    std::cout << "Welcome to 6502 emulator.\n";
    //std::cout << "use -jit for jit parsing of input\n";

    if (argc == 1)
    {
        std::cerr << "please use -h for help.\n";
        return -1;
    }
    else if (argc == 2 && std::string(argv[1]) == std::string("-jit"))
    {
        std::cout << "\
            welcome to jit.. type:\n \
            -q to quit\n  \
            -p to print cpu status\n";
        std::string input;
        CPU cpu;
        while ((std::getline(std::cin, input)) && input != "-q")
        {
            //std::cout << input << "\n";
            if(input == "-p")
            {
                cpu.printStatus();
            }
            else
            {
                parser::parse(input);
            }
        }
        std::cout << "std::out \n";
    }
    return 0;
}

#endif




