#include <stdint.h>
#include "includes/cpu.h"
#include "includes/bus.h"

const uint16_t stack_offset = 0x100; // 0x100 default

uint16_t currVal = 0;
// Set separate val for REL addressing as that requires signed values
int8_t relVal = 0;

Cpu::Cpu() {
	A = 0;
	X = 0;
	Y = 0;
	S = 0x20;
	sp = 0xff;
	bus = Bus();
	pc = 0x8000;
}

Cpu::Cpu(char * filename) {
	A = 0;
	X = 0;
	Y = 0;
	S = 0x20;
	sp = 0xff;
	bus = Bus(filename);
	pc = bus.readRam16(0xfffc);
}

Cpu::~Cpu() {

}

Cpu::instruction currIns;

void Cpu::executeIns() {
	//printf("pc: %.4x", pc);
	currVal = bus.readRam8(pc++);
	currIns = Cpu::instructionarr[currVal];
	/*if (currIns.name == &Cpu::STA && currIns.mode == &Cpu::ZP0) {
		printf("FAFA 0x%.2x", pc);
	}*/
	// Use below to call addressing mode then operation respectively
	((*this).*currIns.mode)();
	((*this).*currIns.name)();
	cyclecount += currIns.cycles;
}

void Cpu::setFlag(uint8_t flag, uint8_t set) {
	if (set)
		S |= flag;
	// To unset the flag, we need to inverse flag so that its given byte is 0
	else
		S &= ~flag;
}

uint8_t Cpu::isFlagSet(uint8_t flag) {
  	return (S & flag) > 0;
}
// Interrupts

// Initialize Cpu to known state
// https://www.pagetable.com/?p=410 for more info.
void Cpu::RESET() {
	A = 0;
	X = 0;
	Y = 0;
	S = 0x20; // Unused bit always 1
	sp = 0xfd;
	pc = bus.readRam16(0xfffc);
	cyclecount = 8;
	//printf("RESET");
}
// Like BRK except set B to 0
void Cpu::IRQ() {
	setFlag(I, 1);
	bus.writeRam8(stack_offset + sp--, (uint8_t)((pc & 0xff00) >> 8));
	bus.writeRam8(stack_offset + sp--, (uint8_t)(pc & 0xff));
	setFlag(B, 0);
	bus.writeRam8(stack_offset + sp--, S);
	pc = bus.readRam16(0xfffe);
	// Same cycle count as BRK
	cyclecount = 7;
	//printf("IRQ");
}
// Like IRQ except get pc from 0xfffa
void Cpu::NMI() {
	setFlag(I, 1);
	bus.writeRam8(stack_offset + sp--, (uint8_t)((pc & 0xff00) >> 8));
	bus.writeRam8(stack_offset + sp--, (uint8_t)(pc & 0xff));
	setFlag(B, 0);
	bus.writeRam8(stack_offset + sp--, S);
	pc = bus.readRam16(0xfffa);
	cyclecount = 7;
	//printf("NMI");
}
//Implied Doubles as ACC insctruction for accumulator
// All functions using IMP and not ACC don't call value from memory
// Will be written as <ins> A for ACC and simply <ins> for IMP
void Cpu::IMP() {
	currVal = (uint16_t) (A) & 0xff;
	//printf("IMP: 0x%.2x\n", currVal);
	//pc++;
}

// Immediate: Grabs next byte from memory to use
void Cpu::IMM() {
	currVal = (uint16_t) bus.readRam8(pc++) & 0xff;
	//printf("IMM: 0x%.2x\n", currVal);
}
// Zero Page: Sets val to memory address stored in next byte
void Cpu::ZP0() {
	uint8_t zp = bus.readRam8(pc++);
	//currVal = (uint16_t) (bus.readRam8(zp));
	currVal = (uint16_t) zp;
	//printf("ZP0: 0x%.2x 0x%.2x\n", zp, pc);
}
// Zero page with X: Sets val to memory address stored in next byte + X
void Cpu::ZPX() {
	uint8_t zp = bus.readRam8(pc++);
	//uint16_t index = (uint16_t)((zp + X) & 0xff);
	//currVal = (uint16_t)(bus.readRam8(index)) & 0xff;
	currVal = (uint16_t)((zp + X) & 0xff);
	//printf("ZPX: 0x%.2x, 0x%.2x, 0x%.2x, 0x%.2x\n", zp, X, index, currVal);
	//printf("ZPX: 0x%.2x\n", currVal);
}
// Zero page with Y: Sets val to memory address stored in next byte + Y
void Cpu::ZPY() {
	uint8_t zp = bus.readRam8(pc++);
	//uint16_t index = (uint16_t)((zp + Y) & 0xff);
	//currVal = (uint16_t)(bus.readRam8(index)) & 0xff;
	currVal = (uint16_t)((zp + Y) & 0xff);
	//printf("ZPX: 0x%.2x\n", currVal);
}

void Cpu::REL() {
	relVal = (int8_t) (bus.readRam8(pc) & 0xff);
	//printf("REL: 0x%.2x\n", relVal);
}

void Cpu::ABS() {
	/*uint16_t abs = (uint16_t) bus.readRam8(pc++);
	uint16_t abs2 = (uint16_t) bus.readRam8(pc++);*/
	uint16_t index = bus.readRam16(pc++);
	pc++;
	//currVal = bus.readRam16(index);
	currVal = index;
	//printf("ABS: 0x%.4x\n", currVal);
}

void Cpu::ABX() {
	/*uint16_t abs = (uint16_t) bus.readRam8(pc++);
	uint16_t abs2 = (uint16_t) bus.readRam8(pc++) << 8;*/
	uint16_t index = bus.readRam16(pc++);
	pc++;
	uint16_t ix = index + X;
	if ((ix & 0xff00) != (index & 0xff00)) {
		cyclecount++;
	}
	//currVal = bus.readRam16((uint16_t) (ix) & 0xffff);
	currVal = ix & 0xffff;
	//printf("ABX: 0x%.4x\n", currVal);
}

void Cpu::ABY() {
	/*uint16_t abs = (uint16_t) bus.readRam8(pc++);
	uint16_t abs2 = (uint16_t) bus.readRam8(pc++) << 8;*/
	uint16_t index = bus.readRam16(pc++);
	pc++;
	uint16_t iy = index + Y;
	if ((iy & 0xff00) != (index & 0xff00)) {
		cyclecount++;
	}
	//currVal = bus.readRam16((uint16_t) (iy) & 0xffff);
	currVal = iy & 0xffff;
	//printf("ABY: 0x%.4x\n", currVal);
}

void Cpu::IND() {
	uint16_t index = bus.readRam16(pc++);
	currVal = bus.readRam16(index);
	//printf("IND: 0x%.4x\n", currVal);
}

void Cpu::IZX() {
	uint16_t index = (bus.readRam8(pc++) + X) & 0xff;
	//Use manual 16 bit read to account for page turning
	//currVal = bus.readRam16(index);
	currVal = index;
	//printf("IZX: 0x%.4x\n", currVal);
}

void Cpu::IZY() {
	uint8_t index = bus.readRam8(pc++);
	//Use manual 16 bit read to account for page turning
	uint8_t addr = bus.readRam8(index);
	//uint8_t addr = index;
	uint16_t iy = addr + Y;
	if (iy & 0xff00) {
		cyclecount++;
	}
  currVal = (uint16_t)(iy) & 0xffff;
	//currVal = bus.readRam16((uint16_t) (iy & 0xffff));
	//printf("IZY: 0x%.4x\n", currVal);
}

// OPCODES
// Add with Carry
void Cpu::ADC() {
	// Carry bit is alway accounted for so we must clear C before calling this
	uint16_t M = currVal;
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		M = bus.readRam16(currVal);
	}
	uint16_t res = A + M + isFlagSet(C);
	setFlag(Z, (res & 0xff) == 0);
	setFlag(N, res & 0x80);
	// & res by 0xff00 in case we are adding > 2 values in addressing
	setFlag(C, res & 0xff00);
	/*
		Signed overflow flag (V) explanation:
		8 bit Signed vals go from [0 -> 0x7f] for + and [0x80 -> 0xff] for -
		for the negative side, 0x80 = 128 for unsigned and 0x80 = -128 for Signed
		We take the two's complement of sums >= 0x80
		Since the sign changed (went from + to -), this is an Overflow
		These are the following scenarios for addition:
		A > 0 and M > 0: Overflow occurs if A + M >= 0x80
		A < 0 and M > 0 or A > 0 and M < 0: No Overflow
		A < 0 and M < 0: Overflow occurs if A + M >= 0 (since only 8 bits, we must "drop" carry from 9th bit therefore making last bit zero)

		^^ We can't just xand A and M and xor that with res as that will set the flag when sign of A, M and sum are all the same
		Since ~(A ^ M) will fail if A != M at overflow, we can take A and compare it w/ sign of Regiser for difference
		We can pull 0x80 out of each Term so we aren't doing & 0x80 or each val as well,
		(~(A & 0x80 ^ M & 0x80) & (A & 0x80 ^ res & 0x80)) = (~(A ^ M) & (A ^ res)) & 0x80
		Since res is also 16 bit, we need to case A and M to 16 bit as well for this operation
	*/
	setFlag(V, (~((uint16_t) A ^ (uint16_t) M) & ((uint16_t) A ^ res)) & 0x80);
	A = res & 0xff;
}
// Logical AND
void Cpu::AND() {
	uint8_t M = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = A & M;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	A = res;
}
// Arithmetic Bit shift Left
void Cpu::ASL() {
	uint8_t M = currVal;
	if (currIns.mode != &Cpu::IMP) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = M << 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, M & 0x80);
	if (currIns.mode == &Cpu::IMP) {
		A = res;
	}
	else {
		bus.writeRam8(currVal, res);
	}
}
// Branch if Carry Clear
void Cpu::BCC() {
	if (!isFlagSet(C)) {
		// Do stuff
		cyclecount++;
		// Is page crossed?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Brach if Carry Set
void Cpu::BCS() {
	if (isFlagSet(C)) {
		// Do stuff
		cyclecount++;
		// Is Page crossed?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Branch if Equal
void Cpu::BEQ() {
	if (isFlagSet(Z)) {
		// Do stuff
		cyclecount++;
		// Is Page crossed?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Bit Test
void Cpu::BIT() {
	uint8_t M = bus.readRam8(currVal);
	uint8_t res = A & M;
	setFlag(Z, res == 0);
	setFlag(V, M & 0x40);
	setFlag(N, M & 0x80);
}
// Branch if Minus
void Cpu::BMI() {
	if (isFlagSet(N)) {
		// Do stuff
		cyclecount++;
		// Is Page crossed?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Branch if Not Equal
void Cpu::BNE() {
	if (!isFlagSet(Z)) {
		// Do stuff
		cyclecount++;
		// Is Page crossed?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Branch if Positive
void Cpu::BPL() {
	if (!isFlagSet(N)) {
		// Do stuff
		cyclecount++;
		// Is Page crossed?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Force Interrupt
void Cpu::BRK() {
	// Set Interrupt and Break flags in S
	setFlag(I, 1);
	setFlag(B, 1);
	bus.writeRam8(stack_offset + sp--, (uint8_t)((pc & 0xff00) >> 8));
	bus.writeRam8(stack_offset + sp--, (uint8_t)(pc & 0xff));
	bus.writeRam8(stack_offset + sp--, S);
	// Now we return to normal since S has been pushed to stack, set Break to 0
	// WILL NEED TO DISABLE THIS EVENTUALLY TO ALLOW PROGRAM TO STAY ALIVE, KEEPING ENABLED FOR TESTING
	//setFlag(B, 0);
	pc = bus.readRam16(0xfffe);
}
// Branch if Overflow Clear
void Cpu::BVC() {
	if (!isFlagSet(V)) {
		// Do stuff
		cyclecount++;
		// is there a page crossing?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Branch if Overflow Set
void Cpu::BVS() {
	if (isFlagSet(V)) {
		cyclecount++;
		// Is there a page crossing?
		if ((pc & 0xff00) != ((pc + relVal) & 0xff00)) {
			cyclecount++;
		}
		pc += relVal;
	}
	else {
		// Continue to next instruction
		pc++;
	}
}
// Clear Carry Flag
void Cpu::CLC() {
	setFlag(C, 0);
}
// Clear Decimal Mode
void Cpu::CLD() {
	setFlag(D, 0);
}
// Clear Interrupt Disable
void Cpu::CLI() {
	setFlag(I, 0);
}
// Clear Overflow Flag
void Cpu::CLV() {
	setFlag(V, 0);
}
// Compare
void Cpu::CMP() {
	uint8_t M = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = A - M;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, A >= M);
}
// Compare X Regiser
void Cpu::CPX() {
	uint8_t M = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = X - M;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, X >= M);
}
// Compare Y Register
void Cpu::CPY() {
	uint8_t M = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = Y - M;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, Y >= M);
}
// Decrement Memory
void Cpu::DEC() {
	uint8_t M = bus.readRam8(currVal);
	uint8_t res = M - 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	bus.writeRam8(pc, res);
}
// Decrement X Register
void Cpu::DEX() {
	//uint8_t M = (uint8_t) (currVal & 0xff);
	uint8_t res = X - 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	X = res;
}
// Decrement Y Register
void Cpu::DEY() {
	//uint8_t M = (uint8_t) (currVal & 0xff);
	uint8_t res = Y - 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	Y = res;
}
// Exclusive OR
void Cpu::EOR() {
	uint8_t M = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = A ^ M;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	A = res;
}
// Increment Memory
void Cpu::INC() {
	uint8_t M = bus.readRam8(currVal);
	// M is only 8 bits so no need to carry
	uint8_t res = M + 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	bus.writeRam8(pc, res);
}
// Increment X Reg.
void Cpu::INX() {
	uint8_t res = X + 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	X = res;
}
// Increment Y Reg.
void Cpu::INY() {
	uint8_t res = Y + 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	Y = res;
}
// Jump
void Cpu::JMP() {
	// Jumps pc to the memory address specified
	/*if (currIns.mode == &Cpu::ABS) {
		pc = bus.readRam16(pc - 2);
	}
	else {
		pc = currVal;
}*/
	pc = currVal;
}
// Jump to Subroutine
void Cpu::JSR() {
	//bus.readRam(stack[pc]);
	// The JSR instruction pushes pc (minus one)
	// to the stack and then sets the program counter to the target memory address.
	// We are decrementing pc because when we pull it from the stack, we need to increment as pull instruction is implied
	//printf("JSRpc: %.4x\n", pc);
	bus.writeRam8(stack_offset + sp--, ((pc - 1) & 0xff00) >> 8);
	//printf("b1: %.2x\n", bus.readRam(stack_offset + (sp + 1)));
	bus.writeRam8(stack_offset + sp--, (pc - 1) & 0xff);
	//printf("b2: %.2x\n", bus.readRam(stack_offset + (sp + 1)));
	pc = bus.readRam16(pc - 2);
}
// Load Accumulator
void Cpu::LDA() {
	uint8_t res = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		res = bus.readRam8(currVal);
	}
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	A = res;
	// TODO continue to next instruction
}
// Load X Reg.
void Cpu::LDX() {
	uint8_t res = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM) {
		res = bus.readRam8(currVal);
	}
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	X = res;
	// TODO continue to next instruction
}
// Load Y Reg.
void Cpu::LDY() {
	uint8_t res = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM) {
		res = bus.readRam8(currVal);
	}
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	Y = res;
	// TODO continue to next instruction
}
// Logical Shift Right
void Cpu::LSR() {
	uint8_t M = bus.readRam8(currVal);
	uint8_t res = M >> 1;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, M & 0x1);
	if (currIns.mode == &Cpu::IMP) {
		A = res;
	}
	else {
		bus.writeRam8(currVal, res);
	}
}
// No Operation
void Cpu::NOP() {
	pc++;
}
// Logical Inclusive OR
void Cpu::ORA() {
	uint8_t M = (uint8_t) (currVal & 0xff);
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		M = bus.readRam8(currVal);
	}
	uint8_t res = A | M;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	A = res;
}
// Push Accumulator
void Cpu::PHA() {
	//Stack works LiFo so we push element to last unused index in stack
	bus.writeRam8(stack_offset + sp--, A);
}
// Push Processor Status
void Cpu::PHP() {
	bus.writeRam8(stack_offset + sp--, S);
}
// Pull Accumulator
void Cpu::PLA() {
	A = bus.readRam8(stack_offset + ++sp);
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
}
// Pull Processor Status
void Cpu::PLP() {
	S = bus.readRam8(stack_offset + ++sp);
}
// Logical Rotate Left
void Cpu::ROL() {
	uint8_t M = bus.readRam8(currVal);
	uint16_t res = (uint16_t) M << 1 | isFlagSet(C);
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, M & 0x80);
	if (currIns.mode == &Cpu::IMP) {
		A = (uint8_t) (res & 0xff);
	}
	else {
		bus.writeRam8(currVal, (uint8_t) (res & 0xff));
	}
	//bus.writeRam8(pc, res & 0xff);
}
// Logical Rotate Right
void Cpu::ROR() {
	uint8_t M = bus.readRam8(currVal);
	uint16_t res = (uint16_t) M >> 1 | isFlagSet(C) << 7;
	setFlag(Z, res == 0);
	setFlag(N, res & 0x80);
	setFlag(C, M & 0x1);
	if (currIns.mode == &Cpu::IMP) {
		A = (uint8_t) (res & 0xff);
	}
	else {
		bus.writeRam8(currVal, (uint8_t) (res & 0xff));
	}
	//bus.writeRam8(pc, res & 0xff);
}
// Return from Interrupt
void Cpu::RTI() {
		S = bus.readRam8(stack_offset + ++sp);
		pc = (uint16_t) (bus.readRam8((stack_offset + ++sp) & 0xffff)) | (uint16_t)(bus.readRam8((stack_offset + ++sp) & 0xffff)) << 8;
}
// Return from Subroutine
void Cpu::RTS() {
		// pulls two bytes from stack into pc then increments pc
		// since memory vals are 8 bits, we need to convert the vals to 16 bit
		pc = (uint16_t) (bus.readRam8((stack_offset + ++sp) & 0xffff)) | (uint16_t)(bus.readRam8((stack_offset + ++sp) & 0xffff)) << 8;
		pc++;
}
// Subtract with Carry
void Cpu::SBC() {
	// Subtraction is the same as addition with two's complement so C must be set before subtraction
	uint16_t M = (~(currVal) & 0xff) + isFlagSet(C);
	if (currIns.mode != &Cpu::IMM && currIns.mode != &Cpu::IZY) {
		M = ~(bus.readRam8(currVal)) + isFlagSet(C);
	}
	uint16_t res = A + M;
	//printf("%d %d %d\n", A, M, res);
	setFlag(Z, (res & 0xff) == 0);
	setFlag(N, res & 0x80);
	// & res by 0xff00 in case we are adding > 2 values in addressing
	setFlag(C, res & 0xff00);
	// Since we are adding by two's complement, V flag has same rule as ADC,
	// Refer to ADC instruction for further details
	setFlag(V, (~((uint16_t) A ^ (uint16_t) M) & ((uint16_t) A ^ res)) & 0x80);
	A = res & 0xff;
}
// Set Carry Flag
void Cpu::SEC() {
	setFlag(C, 1);
}
// Set Decimal Flag
void Cpu::SED() {
	setFlag(D, 1);
}
// Set Interrupt Disable
void Cpu::SEI() {
	setFlag(I, 1);
}
// Store Accumulator
void Cpu::STA() {
	bus.writeRam8(currVal, A);
}
// Store X Reg.
void Cpu::STX() {
	bus.writeRam8(currVal, X);
}
// Store Y Reg.
void Cpu::STY() {
	bus.writeRam8(currVal, Y);
}
// Transfer Accumulator to X
void Cpu::TAX() {
	X = A;
	setFlag(Z, X == 0);
	setFlag(N, X & 0x80);
}
// Transfer Accumulator to Y
void Cpu::TAY() {
	Y = A;
	setFlag(Z, Y == 0);
	setFlag(N, Y & 0x80);
}
// Transfer Stack Pointer to X
void Cpu::TSX() {
	X = sp;
}
// Transfer X to Accumulator
void Cpu::TXA() {
	A = X;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
}
// Transfer X to Stack Pointer
void Cpu::TXS() {
	sp = X;
}
// Transfer Y to Accumulator
void Cpu::TYA() {
	A = Y;
	setFlag(Z, A == 0);
	setFlag(N, A & 0x80);
}
