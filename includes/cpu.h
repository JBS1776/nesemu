#ifndef CPU
#define CPU

#include <stdint.h>
#include <map>
#include  <string>
#include "bus.h"

class Cpu {
	public:
		Cpu();
		Cpu(char * filename);
		~Cpu();
		uint8_t A; // Accumulator
		uint8_t X; // X Register
		uint8_t Y; // Y Register
		uint8_t S; // Status Flags
		// Below flags are to be displayed in the FLAG var
		// Displayed here in binary for readability, will be in hex for futre code
		enum FLAGS {
			N = 0b10000000, // Negative
			V = 0b01000000, // Signed Overflow
			U = 0b00100000, // Unused
			B = 0b00010000, // Break
			D = 0b00001000, // Decimal
			I = 0b00000100, // Interrupt
			Z = 0b00000010, // Zero
			C = 0b00000001  // Carry
		};
		uint8_t sp = 0;
		// stack is size 0xff and is stored in ram's range 0x100-0x1ff
		//uint8_t stack[0xff];
		uint16_t pc;
		// Interrupts
		void RESET();
		void IRQ();
		void NMI();

		Bus bus;
		void setFlag(uint8_t flag, uint8_t set);
		uint8_t isFlagSet(uint8_t flag);
		typedef void (Cpu::*opname)();
		typedef void (Cpu::*opmode)();
		typedef struct instruction {
				opname name;
				opmode mode;
				uint8_t opcode;
				uint8_t size;
				uint8_t cycles;
		};
		// For illegal opcodes, move to next instruction and make with NOP instruction
		instruction nop = {&Cpu::NOP, &Cpu::IMP, 0xea, 1, 2};
		instruction instructionarr[0x100] = {{&Cpu::BRK, &Cpu::IMP, 0, 1, 7}, {&Cpu::ORA, &Cpu::IZX, 0x1, 2, 6},
																			nop, nop, nop, {&Cpu::ORA, &Cpu::ZP0, 0x5, 2, 3}, {&Cpu::ASL, &Cpu::ZP0, 0x6, 2, 5},
																		nop, {&Cpu::PHP, &Cpu::IMP, 0x8, 1, 3}, {&Cpu::ORA, &Cpu::IMM, 0x9, 2, 2},
																	{&Cpu::ASL, &Cpu::IMP, 0xa, 1, 2}, nop, nop, {&Cpu::ORA, &Cpu::ABS, 0xd, 3, 4},
																{&Cpu::ASL, &Cpu::ABS, 0xe, 3, 6}, nop, {&Cpu::BPL, &Cpu::REL, 0x10, 2, 2},
															{&Cpu::ORA, &Cpu::IZY, 0x11, 2, 5}, nop, nop, nop, {&Cpu::ORA, &Cpu::IZY, 0x15, 2, 4},
														{&Cpu::ASL, &Cpu::ZPX, 0x16, 2, 6}, nop, {&Cpu::CLC, &Cpu::IMP, 0x18, 1, 2}, {&Cpu::ORA, &Cpu::ABY, 0x19, 3, 4},
													nop, nop, nop, {&Cpu::ORA, &Cpu::ABX, 0x1d, 3, 4}, {&Cpu::ASL, &Cpu::ABX, 0x1e, 3, 7},
												nop, {&Cpu::JSR, &Cpu::ABS, 0x20, 3, 6}, {&Cpu::AND, &Cpu::IZX, 0x21, 2, 6}, nop, nop, {&Cpu::BIT, &Cpu::ZP0, 0x24, 2, 3},
											{&Cpu::AND, &Cpu::ZP0, 0x25, 2, 3}, {&Cpu::ROL, &Cpu::ZP0, 0x26, 2, 5}, nop, {&Cpu::PLP, &Cpu::IMP, 0x28, 1, 4},
										{&Cpu::AND, &Cpu::IMM, 0x29, 2, 2}, {&Cpu::ROL, &Cpu::IMP, 0x2a, 1, 2}, nop, {&Cpu::BIT, &Cpu::ABS, 0x2c, 3, 4}, {&Cpu::AND, &Cpu::ABS, 0x2d, 3, 4},
									{&Cpu::ROL, &Cpu::ABS, 0x2e, 3, 6}, nop, {&Cpu::BMI, &Cpu::REL, 0x30, 2, 2}, {&Cpu::AND, &Cpu::IZY, 0x31, 2, 5}, nop, nop, nop, {&Cpu::AND, &Cpu::ZP0, 0x35, 2, 4},
								{&Cpu::ROL, &Cpu::ZPX, 0x36, 2, 6}, nop, {&Cpu::SEC, &Cpu::IMP, 0x38, 1, 2}, {&Cpu::AND, &Cpu::ABY, 0x39, 3, 4}, nop, nop, nop, {&Cpu::AND, &Cpu::ABX, 0x3d, 3, 4},
							{&Cpu::ROL, &Cpu::ABX, 0x3e, 3, 7}, nop, {&Cpu::RTI, &Cpu::IMP, 0x40, 1, 6}, {&Cpu::EOR, &Cpu::IZX, 0x41, 2, 6}, nop, nop, nop, {&Cpu::EOR, &Cpu::ZP0, 0x45, 2, 3},
						{&Cpu::LSR, &Cpu::ZP0, 0x46, 2, 5}, nop, {&Cpu::PHA, &Cpu::IMP, 0x48, 1, 3}, {&Cpu::EOR, &Cpu::IMM, 0x49, 2, 2}, {&Cpu::LSR, &Cpu::IMP, 0x4a, 1, 2}, nop,
						{&Cpu::JMP, &Cpu::ABS, 0x4c, 3, 3}, {&Cpu::EOR, &Cpu::ABS, 0x4d, 3, 4}, {&Cpu::LSR, &Cpu::ABS, 0x4e, 3, 6}, nop, {&Cpu::BVC, &Cpu::REL, 0x50, 2, 2},
						{&Cpu::EOR, &Cpu::IZY, 0x51, 2, 5}, nop, nop, nop, {&Cpu::EOR, &Cpu::ZPX, 0x55, 2, 4}, {&Cpu::LSR, &Cpu::ZPX, 0x56, 2, 6}, nop, {&Cpu::CLI, &Cpu::IMP, 0x58, 1, 2},
					{&Cpu::EOR, &Cpu::ABY, 0x59, 3, 4}, nop, nop, nop, {&Cpu::EOR, &Cpu::ABX, 0x5d, 3, 4}, {&Cpu::LSR, &Cpu::ABX, 0x5e, 3, 7}, nop, {&Cpu::RTS, &Cpu::IMP, 0x60, 1, 6},
				{&Cpu::ADC, &Cpu::IZX, 0x61, 2, 6}, nop, nop, nop, {&Cpu::ADC, &Cpu::ZP0, 0x65, 2, 3}, {&Cpu::ROR, &Cpu::ZP0, 0x66, 2, 5}, nop, {&Cpu::PLA, &Cpu::IMP, 0x68, 1, 4},
			{&Cpu::ADC, &Cpu::IMM, 0x69, 2, 2}, {&Cpu::ROR, &Cpu::IMP, 0x6a, 1, 2}, nop, {&Cpu::JMP, &Cpu::IND, 0x6c, 3, 5}, {&Cpu::ADC, &Cpu::ABS, 0x6d, 3, 4},
			{&Cpu::ROR, &Cpu::ABS, 0x6e, 3, 6}, nop, {&Cpu::BVS, &Cpu::REL, 0x70, 2, 2}, {&Cpu::ADC, &Cpu::IZY, 0x71, 2, 5}, nop, nop, nop, {&Cpu::ADC, &Cpu::ZPX, 0x75, 2, 4},
			{&Cpu::ROR, &Cpu::ZPX, 0x76, 2, 6}, nop, {&Cpu::SEI, &Cpu::IMP, 0x78, 1, 2}, {&Cpu::ADC, &Cpu::ABY, 0x79, 3, 4}, nop, nop, nop, {&Cpu::ADC, &Cpu::ABX, 0x7d, 3, 4},
			{&Cpu::ROR, &Cpu::ABX, 0x7e, 3, 7}, nop, nop, {&Cpu::STA, &Cpu::IZX, 0x81, 2, 6}, nop, nop, {&Cpu::STY, &Cpu::ZP0, 0x84, 2, 3}, {&Cpu::STA, &Cpu::ZP0, 0x85, 2, 3},
			{&Cpu::STX, &Cpu::ZP0, 0x86, 2, 3}, nop, {&Cpu::DEY, &Cpu::IMP, 0x88, 1, 2}, nop, {&Cpu::TXA, &Cpu::IMP, 0x8a, 1, 2}, nop, {&Cpu::STY, &Cpu::ABS, 0x8c, 3, 4},
			{&Cpu::STA, &Cpu::ABS, 0x8d, 3, 4}, {&Cpu::STX, &Cpu::ABS, 0x8e, 3, 4}, nop, {&Cpu::BCC, &Cpu::REL, 0x90, 2, 2}, {&Cpu::STA, &Cpu::IZY, 0x91, 2, 6}, nop, nop,
			{&Cpu::STY, &Cpu::ZPX, 0x94, 2, 4}, {&Cpu::STA, &Cpu::ZPX, 0x95, 2, 4}, {&Cpu::STX, &Cpu::ZPY, 0x96, 2, 4}, nop, {&Cpu::TYA, &Cpu::IMP, 0x98, 1, 2},
			{&Cpu::STA, &Cpu::ABY, 0x99, 3, 5}, {&Cpu::TXS, &Cpu::IMP, 0x9a, 1, 2}, nop, nop, {&Cpu::STA, &Cpu::ABX, 0x9d, 3, 5}, nop, nop, {&Cpu::LDY, &Cpu::IMM, 0xa0, 2, 2},
			{&Cpu::LDA, &Cpu::IZX, 0xa1, 2, 6}, {&Cpu::LDX, &Cpu::IMM, 0xa2, 2, 2}, nop, {&Cpu::LDY, &Cpu::ZP0, 0xa4, 2, 3}, {&Cpu::LDA, &Cpu::ZP0, 0xa5, 2, 3},
			{&Cpu::LDX, &Cpu::ZP0, 0xa6, 2, 3}, nop, {&Cpu::TAY, &Cpu::IMP, 0xa8, 1, 2}, {&Cpu::LDA, &Cpu::IMM, 0xa9, 2, 2}, {&Cpu::TAX, &Cpu::IMP, 0xaa, 1, 2}, nop,
			{&Cpu::LDY, &Cpu::ABS, 0xac, 3, 4}, {&Cpu::LDA, &Cpu::ABS, 0xad, 3, 4}, {&Cpu::LDX, &Cpu::ABS, 0xae, 3, 4}, nop, {&Cpu::BCS, &Cpu::REL, 0xb0, 2, 2},
			{&Cpu::LDA, &Cpu::IZY, 0xb1, 2, 5}, nop, nop, {&Cpu::LDY, &Cpu::ZPX, 0xb4, 2, 4}, {&Cpu::LDA, &Cpu::ZPX, 0xb5, 2, 4}, {&Cpu::LDX, &Cpu::ZPY, 0xb6, 2, 4}, nop,
			{&Cpu::CLV, &Cpu::IMP, 0xb8, 1, 2}, {&Cpu::LDA, &Cpu::ABY, 0xb9, 3, 4}, {&Cpu::TSX, &Cpu::IMP, 0xba, 1, 2}, nop, {&Cpu::LDY, &Cpu::ABX, 0xbc, 3, 4},
			{&Cpu::LDA, &Cpu::ABX, 0xbd, 3, 4}, {&Cpu::LDX, &Cpu::ABY, 0xbe, 3, 4}, nop, {&Cpu::CPY, &Cpu::IMM, 0xc0, 2, 2}, {&Cpu::CMP, &Cpu::IZX, 0xc1, 2, 6}, nop, nop,
			{&Cpu::CPY, &Cpu::ZP0, 0xc4, 2, 3}, {&Cpu::CMP, &Cpu::ZP0, 0xc5, 2, 3}, {&Cpu::DEC, &Cpu::ZP0, 0xc6, 2, 5}, nop, {&Cpu::INY, &Cpu::IMP, 0xc8, 1, 2},
			{&Cpu::CMP, &Cpu::IMM, 0xc9, 2, 2}, {&Cpu::DEX, &Cpu::IMP, 0xca, 1, 2}, nop, {&Cpu::CPY, &Cpu::ABS, 0xcc, 3, 4}, {&Cpu::CMP, &Cpu::ABS, 0xcd, 3, 4},
			{&Cpu::DEC, &Cpu::ABS, 0xce, 3, 6}, nop, {&Cpu::BNE, &Cpu::REL, 0xd0, 2, 2}, {&Cpu::CMP, &Cpu::IZY, 0xd1, 2, 5}, nop, nop, nop, {&Cpu::CMP, &Cpu::ZPX, 0xd5, 2, 4},
			{&Cpu::DEC, &Cpu::ZPX, 0xd6, 2, 6}, nop, {&Cpu::CLD, &Cpu::IMP, 0xd8, 1, 2}, {&Cpu::CMP, &Cpu::ABY, 0xd9, 3, 4}, nop, nop, nop, {&Cpu::CMP, &Cpu::ABX, 0xdd, 3, 4},
			{&Cpu::DEC, &Cpu::ABX, 0xde, 3, 7}, nop, {&Cpu::CPX, &Cpu::IMM, 0xe0, 2, 2}, {&Cpu::SBC, &Cpu::IZX, 0xe1, 2, 6}, nop, nop, {&Cpu::CPX, &Cpu::ZP0, 0xe4, 2, 3},
			{&Cpu::SBC, &Cpu::ZP0, 0xe5, 2, 3}, {&Cpu::INC, &Cpu::ZP0, 0xe6, 2, 5}, nop, {&Cpu::INX, &Cpu::IMP, 0xe8, 1, 2}, {&Cpu::SBC, &Cpu::IMM, 0xe9, 2, 2}, nop, nop,
			{&Cpu::CPX, &Cpu::ABS, 0xec, 3, 4}, {&Cpu::SBC, &Cpu::ABS, 0xed, 3, 4}, {&Cpu::INC, &Cpu::ABS, 0xee, 3, 6}, nop, {&Cpu::BEQ, &Cpu::REL, 0xf0, 2, 2},
			{&Cpu::SBC, &Cpu::IZY, 0xf1, 2, 5}, nop, nop, nop, {&Cpu::SBC, &Cpu::ZPX, 0xf5, 2, 4}, {&Cpu::INC, &Cpu::ZPX, 0xf6, 2, 6}, nop, {&Cpu::SED, &Cpu::IMP, 0xf8, 1, 2},
			{&Cpu::SBC, &Cpu::ABY, 0xf9, 3, 4}, nop, nop, nop, {&Cpu::SBC, &Cpu::ABX, 0xfd, 3, 4}, {&Cpu::INC, &Cpu::ABX, 0xfe, 3, 7}, nop};
			uint64_t cyclecount = 0;
		void executeIns();
	private:

			void IMP();

			void IMM();

			void ZP0();

			void ZPX();

			void ZPY();

			void REL();

			void ABS();

			void ABX();

			void ABY();

			void IND();

			void IZX();

			void IZY();

			// OPCODES
			// Add with Carry
			void ADC();
			// Logical AND
			void AND();
			// Arithmetic Bit shift Left
			void ASL();
			// Branch if Carry Clear
			void BCC();
			// Brach if Carry Set
			void BCS();
			// Branch if Equal
			void BEQ();
			// Bit Test
			void BIT();
			// Branch if Minus
			void BMI();
			// Branch if Not Equal
			void BNE();
			// Branch if Positive
			void BPL();
			// Force Interrupt
			void BRK();
			// Branch if Overflow Clear
			void BVC();
			// Branch if Overflow Set
			void BVS();
			// Clear Carry Flag
			void CLC();
			// Clear Decimal Mode
			void CLD();
			// Clear Interrupt Disable
			void CLI();
			// Clear Overflow Flag
			void CLV();
			// Compare
			void CMP();
			// Compare X Regiser
			void CPX();
			// Compare Y Register
			void CPY();
			// Decrement Memory
			void DEC();
			// Decrement X Register
			void DEX();
			// Decrement Y Register
			void DEY();
			// Exclusive OR
			void EOR();
			// Increment Memory
			void INC();
			// Increment X Reg.
			void INX();
			// Increment Y Reg.
			void INY();
			// Jump
			void JMP();
			// Jump to Subroutine
			void JSR();
			// Load Accumulator
			void LDA();
			// Load X Reg.
			void LDX();
			// Load Y Reg.
			void LDY();
			// Logical Shift Right
			void LSR();
			// No Operation
			void NOP();
			// Logical Inclusive OR
			void ORA();
			// Push Accumulator
			void PHA();
			// Push Processor Status
			void PHP();
			// Pull Accumulator
			void PLA();
			// Pull Processor Status
			void PLP();
			// Rotate Left
			void ROL();
			// Rotate Right
			void ROR();
			// Return from Interrupt
			void RTI();
			// Return from Subroutine
			void RTS();
			// Subtract with Carry
			void SBC();
			// Set Carry Flag
			void SEC();
			// Set Decimal Flag
			void SED();
			// Set Interrupt Disable
			void SEI();
			// Store Accumulator
			void STA();
			// Store X Reg.
			void STX();
			// Store Y Reg.
			void STY();
			// Transfer Accumulator to X
			void TAX();
			// Transfer Accumulator to Y
			void TAY();
			// Transfer Stack Pointer to X
			void TSX();
			// Transfer X to Accumulator
			void TXA();
			// Transfer X to Stack Pointer
			void TXS();
			// Transfer Y to Accumulator
			void TYA();
};

#endif
