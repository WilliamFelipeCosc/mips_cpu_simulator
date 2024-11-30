#include <stdio.h>
#include <stdlib.h>

#define NUM_REGISTERS 32
#define MEMORY_LIMIT 32

typedef struct
{
  int registers[NUM_REGISTERS]; // Registradores
  int Hi;                       // Registrador Hi
  int Lo;                       // Registrador Lo
  int Pc;                       // Registrador PC (Program Counter)
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

typedef struct
{
  int location;      // Endereço na memória
  unsigned int code; // Código binário
} InstructionLocation;

typedef struct
{
  int nextAvailableAddress;              // Próximo endereço disponível
  int size;                              // Tamanho da memória ocupada
  InstructionLocation ram[MEMORY_LIMIT]; // Lista de instruções
} Memory;

void initMemory(Memory *memory);
void addInstruction(Memory *memory, unsigned int binary);
void executeAllInstructions(Memory *memory, Processor *cpu);
void decodeInstruction(unsigned int binary, Instruction *instr);
int executeInstruction(Instruction instr, Processor *cpu);
int executeRType(Instruction instr, Processor *cpu);
int executeIType(Instruction instr, Processor *cpu);
int executeJType(Instruction instr, Processor *cpu);
void updateRegisters(int value, Instruction instr, Processor *cpu, Memory *memory);
void printRegisters(Processor cpu);

int main()
{
  Processor cpu = {{0}}; // Inicializa todos os registradores com 0
  Memory memory;

  initMemory(&memory);

  unsigned int binaryInstr = 0x21280096; // Exemplo de instrução I (addi $t0, $t1, 150)
  addInstruction(&memory, binaryInstr);

  unsigned int binaryInstr9 = 0x0c00001c; // Exemplo de instrução J (jal 28)
  addInstruction(&memory, binaryInstr9);

  unsigned int binaryInstr2 = 0x310a004b; // Exemplo de instrução I (andi $t2, $t0, 75)
  addInstruction(&memory, binaryInstr2);

  unsigned int binaryInstr3 = 0x010a5821; // Exemplo de instrução R (addu $t3, $t0, $t2)
  addInstruction(&memory, binaryInstr3);

  unsigned int binaryInstr4 = 0x010a6023; // Exemplo de instrução R (subu $t4, $t0, $t2)
  addInstruction(&memory, binaryInstr4);

  unsigned int binaryInstr5 = 0x014b4819; // Exemplo de instrução R (multu $t2, $t3)
  addInstruction(&memory, binaryInstr5);

  unsigned int binaryInstr6 = 0x016a001b; // Exemplo de instrução R (divu $t3, $t2)
  addInstruction(&memory, binaryInstr6);

  unsigned int binaryInstr7 = 0x00007012; // Exemplo de instrução R (mflo $t6)
  addInstruction(&memory, binaryInstr7);

  unsigned int binaryInstr8 = 0x00006810; // Exemplo de instrução R (mfhi $13, $0, $0)
  addInstruction(&memory, binaryInstr8);

  executeAllInstructions(&memory, &cpu);

  printRegisters(cpu);

  return 0;
}

void initMemory(Memory *memory)
{
  memory->nextAvailableAddress = 0;
  memory->size = 0;
}

void addInstruction(Memory *memory, unsigned int binary)
{
  InstructionLocation newInstruction;
  newInstruction.code = binary;
  newInstruction.location = memory->nextAvailableAddress;

  memory->ram[memory->size] = newInstruction;

  memory->nextAvailableAddress += 4;
  memory->size += 1;
}

void executeAllInstructions(Memory *memory, Processor *cpu)
{

  while (cpu->Pc < memory->nextAvailableAddress)
  {
    int i = 0;
    for (i = 0; i < memory->size; i++)
    {
      if (cpu->Pc == memory->ram[i].location)
      {
        Instruction instr;
        decodeInstruction(memory->ram[i].code, &instr);
        int ret_value = executeInstruction(instr, cpu);
        updateRegisters(ret_value, instr, cpu, memory);
      }
    }
  }
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
}

int executeInstruction(Instruction instr, Processor *cpu)
{
  switch (instr.opcode)
  {
  case 0: // Tipo R
    return executeRType(instr, cpu);
    break;
  case 2: // Tipo J
  case 3:
    return executeJType(instr, cpu);
    break;
  default: // Tipo I
    return executeIType(instr, cpu);
    break;
  }
}

int executeRType(Instruction instr, Processor *cpu)
{
  if (instr.funct == 33)
  {
    return cpu->registers[instr.rs] + cpu->registers[instr.rt];
  }
  if (instr.funct == 35)
  {
    return cpu->registers[instr.rs] - cpu->registers[instr.rt];
  }
  if (instr.funct == 25)
  {
    return cpu->registers[instr.rs] * cpu->registers[instr.rt];
  }
  if (instr.funct == 27)
  {
    if (cpu->registers[instr.rs] && cpu->registers[instr.rt])
    {
      return cpu->registers[instr.rs] / cpu->registers[instr.rt];
    }
  }
  if (instr.funct == 16)
  {
    return cpu->Hi;
  }
  if (instr.funct == 18)
  {
    return cpu->Lo;
  }
}

int executeIType(Instruction instr, Processor *cpu)
{
  if (instr.opcode == 8)
  {
    return cpu->registers[instr.rs] + instr.immediate;
  }
  if (instr.opcode == 12)
  {
    return cpu->registers[instr.rs] & instr.immediate;
  }
}

int executeJType(Instruction instr, Processor *cpu)
{

  return instr.address;
}

void updateRegisters(int value, Instruction instr, Processor *cpu, Memory *memory)
{
  // Atualizar Registros
  if (instr.opcode == 0)
  {
    if (instr.funct == 25 || instr.funct == 27)
    {
      cpu->Lo = value;
    }
    else
    {
      cpu->registers[instr.rd] = value;
    }
    cpu->Pc += 4;
    return;
  }

  if (instr.opcode == 2)
  {
    cpu->Pc = instr.address;
    return;
  }
  if (instr.opcode == 3)
  {
    cpu->registers[31] = cpu->Pc;
    cpu->Pc = instr.address;
    return;
  }

  cpu->Pc += 4;
  cpu->registers[instr.rt] = value;
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

  printf("Hi: %d\n", cpu.Hi);
  printf("Lo: %d\n", cpu.Lo);

  int i = 0;
  for (i = 0; i < NUM_REGISTERS; i++)
  {
    printf("$%d - %s: %d\n", i, registers[i], cpu.registers[i]);
  }
}
