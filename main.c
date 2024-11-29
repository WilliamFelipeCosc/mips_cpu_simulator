#include <stdio.h>
#include <stdlib.h>

#define NUM_REGISTERS 32
#define MEMORY_LIMIT 32

typedef struct
{
  int registers[NUM_REGISTERS]; // Registradores
  int Hi;
  int Lo;
  int Pc;
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
  int location;
  unsigned int code;
} InstructionLocation;

typedef struct
{
  int nextAvailableAddress;
  int size;
  InstructionLocation ram[MEMORY_LIMIT];
} Memory;

void initMemory(Memory *memory);
void addInstruction(Memory *memory, unsigned int binary);
void executeAllInstructions(Memory *memory, Processor *cpu);
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
  Memory memory;

  initMemory(&memory);

  unsigned int binaryInstr = 0x21280096; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr);

  unsigned int binaryInstr2 = 0x310a004b; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr2);

  unsigned int binaryInstr3 = 0x010a5821; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr3);

  unsigned int binaryInstr4 = 0x010a6023; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr4);

  unsigned int binaryInstr5 = 0x014b4819; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr5);

  unsigned int binaryInstr6 = 0x016a001b; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr6);

  unsigned int binaryInstr7 = 0x00007012; // Exemplo de instrução R (add $8, $9, $10)
  addInstruction(&memory, binaryInstr7);

  unsigned int binaryInstr8 = 0x00006810; // Exemplo de instrução R (mfhi $13, $0, $0)
  addInstruction(&memory, binaryInstr8);

  // printf("sizeee: %d\n", memory.size);
  // int i;
  // for (i = 0; i < memory.size; i++)
  // {
  //   printf("mem %d: %d - %d\n", i, memory.ram[i].location, memory.ram[i].code);
  // }

  executeAllInstructions(&memory, &cpu);

  // executeInstruction(instr, &cpu);
  // executeInstruction(instr2, &cpu);
  // executeInstruction(instr3, &cpu);
  // executeInstruction(instr4, &cpu);
  // executeInstruction(instr5, &cpu);
  // executeInstruction(instr6, &cpu);
  // executeInstruction(instr7, &cpu);
  // executeInstruction(instr8, &cpu);

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

  printf("pogss: %d - %d\n\n", cpu->Pc, memory->nextAvailableAddress);
  if (cpu->Pc >= memory->nextAvailableAddress)
    return;

  // printf("pingas");

  int i = 0;
  for (i = 0; i < memory->size; i++)
  {
    if (cpu->Pc == memory->ram[i].location)
    {
      printf("uéeeeee: %d %d\n", cpu->Pc, memory->ram[i].location);
      Instruction instr;
      decodeInstruction(memory->ram[i].code, &instr);
      executeInstruction(instr, cpu);
      break;
    }
  }

  executeAllInstructions(memory, cpu);
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

  // printf("addr: %d\n", instr);
  // printf("opcode: %d\n", instr->opcode);
  // printf("rs: %d\n", instr->rs);
  // printf("rt: %d\n", instr->rt);
  // printf("rd: %d\n", instr->rd);
  // printf("shamt: %d\n", instr->shamt);
  // printf("funct: %d\n", instr->funct);
  // printf("immediate: %d\n", instr->immediate);
  // printf("address: %d\n", instr->address);
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

  // printf("hmmmm: %d\n", instr.funct);
  if (instr.funct == 33)
  {
    cpu->registers[instr.rd] = cpu->registers[instr.rs] + cpu->registers[instr.rt];
  }
  if (instr.funct == 35)
  {
    cpu->registers[instr.rd] = cpu->registers[instr.rs] - cpu->registers[instr.rt];
  }
  if (instr.funct == 25)
  {
    int total = cpu->registers[instr.rs] * cpu->registers[instr.rt];
    cpu->Lo = total;
  }
  if (instr.funct == 27)
  {
    int division = cpu->registers[instr.rs] / cpu->registers[instr.rt];
    int remainder = cpu->registers[instr.rs] % cpu->registers[instr.rt];
    cpu->Lo = division;
    cpu->Hi = remainder;
  }
  if (instr.funct == 16)
  {
    cpu->registers[instr.rd] = cpu->Hi;
  }
  if (instr.funct == 18)
  {
    cpu->registers[instr.rd] = cpu->Lo;
  }

  cpu->Pc += 4;
}

void executeIType(Instruction instr, Processor *cpu)
{
  printf("foii?: %d\n", instr.opcode);
  if (instr.opcode == 8)
  {
    cpu->registers[instr.rt] = cpu->registers[instr.rs] + instr.immediate;
  }
  if (instr.opcode == 12)
  {
    cpu->registers[instr.rt] = cpu->registers[instr.rs] & instr.immediate;
  }
  cpu->Pc += 4;
}

void executeJType(Instruction instr, Processor *cpu)
{
  printf("executeJType\n");
  cpu->Pc += 4;
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

  printf("Hi: %d\n", cpu.Hi);
  printf("Lo: %d\n", cpu.Lo);

  int i = 0;
  for (i = 0; i < NUM_REGISTERS; i++)
  {
    // if (cpu.registers[i] )
    printf("$%d - %s: %d\n", i, registers[i], cpu.registers[i]);
  }
}
