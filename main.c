#include <stdio.h>
#include <stdlib.h>

#define NUM_REGISTERS 32

typedef struct
{
  int name;
  int code;
} Label;

typedef struct
{
  Label label;
  int registers[NUM_REGISTERS]; // Registradores
} Processor;

typedef struct
{
  unsigned int opcode;  // Código da operação
  unsigned int rs;      // Registrador fonte
  unsigned int rt;      // Registrador destino 1
  unsigned int rd;      // Registrador destino 2
  unsigned int shamt;   // Shift amount (para instruções R)
  unsigned int funct;   // Função (para instruções R)
  int immediate;        // Valor imediato (para instruções I)
  unsigned int address; // Endereço (para instruções J)
} Instruction;

void decodeInstruction(unsigned int binary, Instruction *instr);
void executeInstruction(Instruction instr, Processor *cpu);
void executeRType(Instruction instr, Processor *cpu);
void executeIType(Instruction instr, Processor *cpu);
void executeJType(Instruction instr, Processor *cpu);
void updateRegisters(Instruction instr, Processor *cpu);
void printRegisters(Processor cpu);

int main()
{
  Processor cpu = {{0}}; // Inicializa todos os registradores com 0
  Instruction instr;

  unsigned int binaryInstr = 0x012A4020; // Exemplo de instrução R (add $8, $9, $10)
  decodeInstruction(binaryInstr, &instr);

  printf("Decoding instruction...\n");
  executeInstruction(instr, &cpu);
  printRegisters(cpu);

  return 0;
}

void decodeInstruction(unsigned int binary, Instruction *instr)
{
  instr->opcode = (binary >> 26) & 0x3F; // 6 bits mais significativos
  instr->rs = (binary >> 21) & 0x1F;     // Próximos 5 bits
  instr->rt = (binary >> 16) & 0x1F;     // Próximos 5 bits
  instr->rd = (binary >> 11) & 0x1F;     // Próximos 5 bits
  instr->shamt = (binary >> 6) & 0x1F;   // Próximos 5 bits
  instr->funct = binary & 0x3F;          // Últimos 6 bits
  instr->immediate = binary & 0xFFFF;    // Últimos 16 bits (se I-type)
  instr->address = binary & 0x3FFFFFF;   // Últimos 26 bits (se J-type)

  printf("opcode: %d\n", instr->opcode);
  printf("rs: %d\n", instr->rs);
  printf("rt: %d\n", instr->rt);
  printf("rd: %d\n", instr->rd);
  printf("shamt: %d\n", instr->shamt);
  printf("funct: %d\n", instr->funct);
  printf("immediate: %d\n", instr->immediate);
  printf("address: %d\n", instr->address);
}

void executeInstruction(Instruction instr, Processor *cpu)
{
  switch (instr.opcode)
  {
  case 0: // Tipo R
    executeRType(instr, cpu);
    break;
  case 2: // Tipo J
  case 3:
    executeJType(instr, cpu);
    break;
  default: // Tipo I
    executeIType(instr, cpu);
    break;
  }
}

void executeRType(Instruction instr, Processor *cpu)
{
  printf("executeRType\n");
}

void executeIType(Instruction instr, Processor *cpu)
{
  printf("executeIType\n");
}

void executeJType(Instruction instr, Processor *cpu)
{
  printf("executeJType\n");
}

void updateRegisters(Instruction instr, Processor *cpu)
{
  printf("updateRegisters\n");
}

void printRegisters(Processor cpu)
{
  char *registers[] = {
      "$zero", "$at", "$v0", "$v1",
      "$a0", "$a1", "$a2", "$a3",
      "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
      "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
      "$k0", "$k1",
      "$gp", "$sp", "$fp", "$ra"};

  int i = 0;
  for (i = 0; i < NUM_REGISTERS; i++)
  {
    printf("$%d - %s: %d\n", i, registers[i], cpu.registers[i]);
  }
}
