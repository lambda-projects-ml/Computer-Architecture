#include "cpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define DATA_LEN 6

// PC: Program Counter, address of the currently executing instruction
// IR: Instruction Register, contains a copy of the currently executing instruction
// MAR: Memory Address Register, holds the memory address we're reading or writing
// MDR: Memory Data Register, holds the value to write or the value just read
void cpu_ram_read(struct cpu *cpu, int index)
{
  if (index > 256 || index < 0)
  {
    printf("Index out of range");
  }
  // printf("%d\n", cpu->ram[index]);
  printf("Index out of range");
}
void cpu_ram_write(struct cpu *cpu, unsigned char element, int index)
{
  if (index > 256 || index < 0)
  {
    printf("Index out of range");
  }
  cpu->ram[index] = element;
}
/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
// void cpu_load(struct cpu *cpu)
void cpu_load(struct cpu *cpu, char *filename)
{
  FILE *fp;
  char line[1024];
  int address = 0;

  fp = fopen(filename, "r");

  if (fp == NULL)
  {
    fprintf(stderr, "file not found\n");
    exit(1);
  }

  while (fgets(line, 1024, fp) != NULL)
  { // not EOF
    char *endptr;

    unsigned char v = strtoul(line, &endptr, 2);

    if (endptr == line)
    {
      continue;
    }

    cpu_ram_write(cpu, v, address);
    address++;
  }

  fclose(fp);
}

/**
 * ALU - Arithmetic Logic Unit
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{

  switch (op)
  {
  case ALU_MUL:
    // TODO
    cpu->registers[0] = cpu->registers[regA] * cpu->registers[regB];
    break;
  case ALU_ADD:
    // TODO
    cpu->registers[0] = cpu->registers[regA] + cpu->registers[regB];
    break;
  case ALU_MOD:
    // TODO
    cpu->registers[0] = cpu->registers[regA] % cpu->registers[regB];
    break;
  case ALU_CMP:
    // TODO
    // `FL` bits: `00000LGE`
    if (cpu->registers[regA] < cpu->registers[regB])
    {
      cpu->fl = 00000100;
    }
    else if (cpu->registers[regA] > cpu->registers[regB])
    {
      cpu->fl = 00000010;
    }
    else if (cpu->registers[regA] == cpu->registers[regB])
    {
      cpu->fl = 00000001;
    }
    break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  int instruction; // Instruction Register, contains a copy of the currently executing instruction
  int operandA, operandB, reg, v, retaddr;

  while (running)
  {
    instruction = cpu->ram[cpu->pc];
    // printf(" PC: %02X  INST: %02X \n", cpu->pc, instruction);
    if (instruction >> 6 == 2)
    {
      operandA = cpu->ram[cpu->pc + 1];
      operandB = cpu->ram[cpu->pc + 2];
    }
    else if (instruction >> 6 == 1)
    {
      operandA = cpu->ram[cpu->pc + 1];
    }

    switch (instruction)
    {
    case LDI:
      cpu->registers[operandA] = operandB;
      cpu->pc += 3;
      break;
    case JMP:
      cpu->pc = cpu->registers[operandA];
      break;
    case JNE:
      if (cpu->fl != 1)
      {
        cpu->pc = cpu->registers[operandA];
      }
      else
      {
        cpu->pc += 2;
      }
      break;
    case JEQ:
      if (cpu->fl == 1)
      {
        cpu->pc = cpu->registers[operandA];
      }
      else
      {
        cpu->pc += 2;
      }

      break;
    case CMP:
      alu(cpu, instruction, operandA, operandB);
      cpu->pc += 3;
      break;
    case PRN:
      printf("%d\n", cpu->registers[operandA]);
      cpu->pc += 2;
      break;
    case MUL:
      alu(cpu, instruction, operandA, operandB);
      cpu->pc += 3;
      break;
    case ADD:
      alu(cpu, instruction, operandA, operandB);
      cpu->pc += 3;
      break;
    case MOD:
      alu(cpu, instruction, operandA, operandB);
      cpu->pc += 3;
      break;
    case PUSH:
      cpu->registers[SP]--;
      reg = cpu->ram[cpu->pc + 1];
      v = cpu->registers[reg];
      cpu->ram[cpu->registers[SP]] = v;
      cpu->pc += 2;
      break;
    case POP:
      reg = cpu->ram[cpu->pc + 1];
      v = cpu->ram[cpu->registers[SP]];
      cpu->registers[reg] = v;
      cpu->registers[SP]++;
      cpu->pc += 2;
      break;
    case CALL:
      // push return addr on stack
      retaddr = cpu->pc + 2;
      cpu->registers[SP]--; // decrement SP
      cpu->ram[cpu->registers[SP]] = retaddr;

      // set the cpu->PC to the subroutine address
      reg = cpu->ram[cpu->pc + 1];
      cpu->pc = cpu->registers[reg];
      break;
    case RET:
      // pop return addr from stack
      retaddr = cpu->ram[cpu->registers[SP]];
      cpu->registers[SP]++;

      // Set cpu->pc to return addr
      cpu->pc = retaddr;
      break;
    case HLT:
      running = 0;
      break;
    default:
      printf("Unknown instruction %02x at address %02x\n", instruction, cpu->pc);
      exit(1);
    }
  }
  // TODO
  // 1. Get the value of the current instruction (in address PC).
  // 2. Figure out how many operands this next instruction requires
  // 3. Get the appropriate value(s) of the operands following this instruction
  // 4. switch() over it to decide on a course of action.
  // 5. Do whatever the instruction should do according to the spec.
  // 6. Move the PC to the next instruction.
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  memset(cpu->registers, 0, 8);
  memset(cpu->ram, 0, 256);
}
