/* 
Class Project: The logical conclusion (v1.1)
CSCI-2500 Fall 2021
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1

// useful constants
BIT ONE[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT REG_THIRTY_ONE[5] = {TRUE, TRUE, TRUE, TRUE, TRUE};
BIT THIRTY_TWO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};


/******************************************************************************/
/* Function prototypes */
/******************************************************************************/
BIT not_gate(BIT A);
BIT or_gate(BIT A, BIT B);
BIT or_gate3(BIT A, BIT B, BIT C);
BIT and_gate(BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT xor_gate(BIT A, BIT B);
BIT nor_gate(BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3);
BIT multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output);
BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);

void copy_bits(BIT* A, BIT* B);
void print_binary(BIT* A);
void convert_to_binary(int a, BIT* A, int length);
void convert_to_binary_char(int a, char* A, int length);
int binary_to_integer(BIT* A);

int get_instructions(BIT Instructions[][32]);

void Instruction_Memory(BIT* ReadAddress, BIT* Instruction);
void Control(BIT* OpCode,
  BIT* RegDst, BIT* Jump, BIT* Branch, BIT* MemRead, BIT* MemToReg,
  BIT* ALUOp, BIT* MemWrite, BIT* ALUSrc, BIT* RegWrite, BIT* Link);
void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2);
void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData);
void ALU_Control(BIT* ALUOp, BIT* funct, BIT* ALUControl, BIT* Return);
void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result);
void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData);
void Extend_Sign16(BIT* Input, BIT* Output);
void updateState();
  
/******************************************************************************/
/* Functions provided for your convenience */
/******************************************************************************/
BIT not_gate(BIT A)
{
  if (A)
    return FALSE;
  else
    return TRUE;
}

BIT or_gate(BIT A, BIT B)
{
  if (A || B)
    return TRUE;
  else
    return FALSE;
}

BIT or_gate3(BIT A, BIT B, BIT C)
{
  return or_gate(A, or_gate(B, C));
}

BIT and_gate(BIT A, BIT B)
{
  if (A && B)
    return TRUE;
  else
    return FALSE;
}

BIT and_gate3(BIT A, BIT B, BIT C)
{
  return and_gate(A, and_gate(B, C));
}

BIT xor_gate(BIT A, BIT B)
{
  if (A ^ B)
    return TRUE;
  else
    return FALSE;
}

BIT nor_gate(BIT A, BIT B)
{
  return not_gate(or_gate(A, B));
}

BIT nand_gate(BIT A, BIT B)
{
  return not_gate(and_gate(A, B));
}

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3)
{
  // Note: The input -> output mapping is slightly modified from before
  *O0 = and_gate(not_gate(I1), not_gate(I0));
  *O1 = and_gate(not_gate(I1), I0);
  *O2 = and_gate(I1, not_gate(I0));
  *O3 = and_gate(I1, I0);
  
  return;
}

BIT multiplexor2(BIT S, BIT I0, BIT I1)
{
  BIT x0 = and_gate(not_gate(S), I0);
  BIT x1 = and_gate(S, I1);
  return or_gate(x0, x1);  
}

void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output)
{
  for (int i = 0; i < 32; ++i) {
    BIT x0 = and_gate(not_gate(S), I0[i]);
    BIT x1 = and_gate(S, I1[i]);
    Output[i] = or_gate(x0, x1);
  }
}

void decoder3(BIT* I, BIT EN, BIT* O)
{
  decoder2(I[0], I[1], &O[0], &O[1], &O[2], &O[3]);
  decoder2(I[0], I[1], &O[4], &O[5], &O[6], &O[7]);
  O[0] = and_gate(and_gate(not_gate(I[2]), O[0]), EN);
  O[1] = and_gate(and_gate(not_gate(I[2]), O[1]), EN);
  O[2] = and_gate(and_gate(not_gate(I[2]), O[2]), EN);
  O[3] = and_gate(and_gate(not_gate(I[2]), O[3]), EN);
  O[4] = and_gate(and_gate(I[2], O[4]), EN);
  O[5] = and_gate(and_gate(I[2], O[5]), EN);
  O[6] = and_gate(and_gate(I[2], O[6]), EN);
  O[7] = and_gate(and_gate(I[2], O[7]), EN);
  return;
}

void decoder5(BIT* I, BIT* O)
{
  BIT EN[4];
  decoder2(I[3], I[4], &EN[0], &EN[1], &EN[2], &EN[3]);
  decoder3(I, EN[0], O);
  decoder3(I, EN[1], &O[8]);
  decoder3(I, EN[2], &O[16]);
  decoder3(I, EN[3], &O[24]);
  return;
}

BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3)
{
  BIT x0, x1, x2, x3 = FALSE;
  decoder2(S0, S1, &x0, &x1, &x2, &x3);
  
  BIT y0 = and_gate(x0, I0);
  BIT y1 = and_gate(x1, I1);
  BIT y2 = and_gate(x2, I2);
  BIT y3 = and_gate(x3, I3);
  
  BIT z0 = or_gate(y0, y1);
  BIT z1 = or_gate(y2, y3);
  
  return or_gate(z0, z1);  
}

BIT and_gate4(BIT A, BIT B, BIT C, BIT D){
  
  return and_gate(and_gate(A,B),and_gate(C,D));
}

BIT or_gate4(BIT A, BIT B, BIT C, BIT D){
  return or_gate(or_gate(A, B), or_gate(C, D));
}

/******************************************************************************/
/* Helper functions */
/******************************************************************************/
void copy_bits(BIT* A, BIT* B)
{
  for (int i = 0; i < 32; ++i)
    B[i] = A[i]; 
}

void print_binary(BIT* A)
{
  for (int i = 31; i >= 0; --i)
    printf("%d", A[i]); 
  printf("\n");
}

void convert_to_binary(int a, BIT* A, int length){
  convert_to_binary_char(a, A, length);
}

void convert_to_binary_char(int a, char* A, int length)
{
  if (a >= 0) {
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 1 ? '1' : '0');
      //printf("A[%d] = %c\n", i, A[i]);
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 0 ? '1' : '0');
      a /= 2;
    }
  }
}
  
int binary_to_integer(BIT* A)
{
  unsigned a = 0;
  unsigned mult = 1;
  
  for (int i = 0; i < 32; ++i) {
    a += A[i]*mult;
    mult *= 2;
  }
  
  return (int)a;
}


/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

// TODO: Implement any helper functions to assist with parsing

void set_op(char instruction[5], BIT Instructions[][32], int i){

  if(strcmp(instruction, "addi") == 0){
    Instructions[i][29] = TRUE;

  }else if(strcmp(instruction, "lw") == 0){
    Instructions[i][31] = TRUE;
    Instructions[i][27] = TRUE;
    Instructions[i][26] = TRUE;
  }else if(strcmp(instruction, "beq") == 0){
    Instructions[i][28] = TRUE;
  }else if(strcmp(instruction, "sw") == 0){
    Instructions[i][31] = TRUE;
    Instructions[i][29] = TRUE;
    Instructions[i][27] = TRUE;
    Instructions[i][26] = TRUE;
  }else if(strcmp(instruction, "j") == 0){
    Instructions[i][27] = TRUE;
  }else if(strcmp(instruction, "jal") == 0){
    Instructions[i][27] = TRUE;
    Instructions[i][26] = TRUE;
  }
}

void set_funct(char instruction[5], BIT Instructions[][32], int i){

  if(strcmp(instruction, "add") == 0){
    Instructions[i][5] = TRUE;
  } else if(strcmp(instruction, "or") == 0){
    Instructions[i][5] = TRUE;
    Instructions[i][2] = TRUE;
    Instructions[i][0] = TRUE;
  } else if(strcmp(instruction, "and") == 0){
    Instructions[i][5] = TRUE;
    Instructions[i][2] = TRUE;
  } else if(strcmp(instruction, "sub") == 0){
    Instructions[i][5] = TRUE;
    Instructions[i][1] = TRUE;
  } else if(strcmp(instruction, "slt") == 0){
    Instructions[i][5] = TRUE;
    Instructions[i][3] = TRUE;
    Instructions[i][1] = TRUE;
  } else if(strcmp(instruction, "jr") == 0){
    Instructions[i][3] = TRUE;
  }

}

int regNumber(char registr[128]){
  if(strcmp(registr, "zero") == 0){
    return 0;
  } else if(strcmp(registr, "v0") == 0){
    return 2;
  } else if(strcmp(registr, "a0") == 0){
    return 4;
  } else if(strcmp(registr, "t0") == 0){
    //printf("RANNNNNN");
    return 8;
  } else if(strcmp(registr, "t1") == 0){
    return 9;
  } else if(strcmp(registr, "s0") == 0){
    //printf("RANNNNNN");
    return 16;
  } else if(strcmp(registr, "s1") == 0){
    return 17;
  } else if(strcmp(registr, "sp") == 0){
    return 29;
  } else if(strcmp(registr, "ra") == 0){
    return 31;
  }
  return -1;
}

void Rset_bits(char regOne[128], char regTwo[128], char regThree[128], 
                                BIT Instructions[][32], int i){

  char temp[5] = {0};
  convert_to_binary_char(regNumber(regTwo), temp, 5);

  int k = 0;
  for (int j = 21; j <= 25 ; j++){
    if(temp[k] == '1'){
      Instructions[i][j] = TRUE;
    }else{
      Instructions[i][j] = FALSE;
    }
    k++;
  }

  k = 0;
  convert_to_binary_char(regNumber(regThree), temp, 5);
  for (int j = 16; j <= 20; j++){
    if(temp[k] == '1'){
      Instructions[i][j] = TRUE;
    }else{
      Instructions[i][j] = FALSE;
    }
    k++;
  }

  

  k=0;
  convert_to_binary_char(regNumber(regOne), temp, 5);
  for (int j = 11; j <= 15; j++){
    if(temp[k] == '1'){
     Instructions[i][j] = TRUE;
    }else{
     Instructions[i][j] = FALSE;
    }
     k++;
  }
}

void Iset_bits(char destination[128], char regOne[128], char immediate[128], BIT Instructions[][32], int i){

  char temp[5] = {0};

  convert_to_binary_char(regNumber(regOne), temp, 5);
  int k = 0;
  for (int j = 21; j <= 25 ; j++){
    if(temp[k] == '1'){
      Instructions[i][j] = TRUE;
    }else{
      Instructions[i][j] = FALSE;
    }

    k++;
  }

  convert_to_binary_char(regNumber(destination), temp, 5);
  k = 0;
  for (int j = 16; j <= 20 ; j++){
    if(temp[k] == '1'){
      Instructions[i][j] = TRUE;
    }else{
      Instructions[i][j] = FALSE;
    }

    k++;
  }

  char iTemp[16];
  convert_to_binary_char(atoi(immediate), iTemp, 16);
  k = 0;
  for (int j = 0; j <= 15 ; j++){
    if(iTemp[k] == '1'){
      Instructions[i][j] = TRUE;
    }else{
      Instructions[i][j] = FALSE;
    }

    k++;  
  }
}

void Jset_bits(char address[128], BIT Instructions[][32], int i){

  char temp[26] = {0};

  convert_to_binary_char(atoi(address), temp, 26);
  int k = 0;
  for (int j = 0; j <= 25 ; j++){
    if(temp[k] == '1'){
      Instructions[i][j] = TRUE;
    }else{
      Instructions[i][j] = FALSE;
    }

    k++;
  }


}

int get_instructions(BIT Instructions[][32])
{
  char line[256] = {0};
  int instruction_count = 0;
  while (fgets(line, 256, stdin) != NULL) {        
    // TODO: perform conversion of instructions to binary
    // Input: coming from stdin via: ./a.out < input.txt
    // Output: Convert instructions to binary in the instruction memory
    // Return: Total number of instructions
    // Note: you are free to use if-else and external libraries here
    // Note: you don't need to implement circuits for saving to inst. mem.
    // My approach:
    // - Use sscanf on line to get strings for instruction and registers
    // - Use instructions to determine op code, funct, and shamt fields
    // - Convert immediate field and jump address field to binary
    // - Use registers to get rt, rd, rs fields
    // Note: I parse everything as strings, then convert to BIT array at end

    //max length of an instruction (length 4 + 1 for null char)
    char instruction[5];

    //char *registers[4];
    char registerOne[128];
    char registerTwo[128];
    char registerThree[128];

    //registers[0] = registerOne;
    //registers[1] = registerTwo;
    //registers[2] = registerThree;

    sscanf(line, "%s %s %s %s\n", instruction, registerOne, registerTwo, registerThree);
    set_op(instruction, Instructions, instruction_count);

    //BIT RTYPE = FALSE;

    if(strcmp(instruction, "add") == 0 || strcmp(instruction, "and") == 0 || strcmp(instruction, "or") == 0 
                                        || strcmp(instruction, "sub") == 0 || strcmp(instruction, "slt") == 0){
      Rset_bits(registerOne, registerTwo, registerThree, Instructions, instruction_count);
      set_funct(instruction, Instructions, instruction_count);

    }else if(strcmp(instruction, "jr") == 0){

      char jtemp[5] = {0};
      convert_to_binary_char(regNumber(registerOne), jtemp, 5);
      int k = 0;
      for (int j = 21; j <= 25 ; j++){
        if(jtemp[k] == '1'){
          Instructions[instruction_count][j] = TRUE;
        }else{
          Instructions[instruction_count][j] = FALSE;
        }
        //Instructions[i][j] = temp[k];
        k++;
      }

      set_funct(instruction, Instructions, instruction_count);

    }

    if(strcmp(instruction, "lw") == 0 || strcmp(instruction, "sw") == 0 || strcmp(instruction, "addi") == 0){
      Iset_bits(registerOne, registerTwo, registerThree, Instructions, instruction_count);
    }

    if(strcmp(instruction, "beq") == 0) {
      Iset_bits(registerTwo, registerOne, registerThree, Instructions, instruction_count);
    }

    if(strcmp(instruction, "j") == 0 || strcmp(instruction, "jal") == 0){
      Jset_bits(registerOne, Instructions, instruction_count);
    }

    instruction_count++;
  }

  return instruction_count;
}


/******************************************************************************/
/* Program state - memory spaces, PC, and control */
/******************************************************************************/
BIT PC[32]                  = {FALSE};
BIT MEM_Instruction[32][32] = {FALSE};
BIT MEM_Data[32][32]        = {FALSE};
BIT MEM_Register[32][32]    = {FALSE};

BIT RegDst    = FALSE;
BIT Jump      = FALSE;
BIT Branch    = FALSE;
BIT MemRead   = FALSE;
BIT MemToReg  = FALSE;
BIT ALUOp[2]  = {FALSE};
BIT MemWrite  = FALSE;
BIT ALUSrc    = FALSE;
BIT RegWrite  = FALSE;
BIT Zero      = FALSE;
BIT ALUControl[4] = {FALSE};
BIT Link      = FALSE;
BIT Return    = FALSE;

void print_instruction()
{
  unsigned pc = binary_to_integer(PC);
  printf("PC: %d\n", pc);
  printf("Instruction: ");
  print_binary(MEM_Instruction[pc]);
}

void print_state()
{
  printf("Data: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Data[i]));
  } 
  printf("\n");  
  
  printf("Register: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Register[i]));
  } 
  printf("\n");
}


/******************************************************************************/
/* Functions that you will implement */
/******************************************************************************/
void Instruction_Memory(BIT* ReadAddress, BIT* Instruction)
{
  // TODO: Implement instruction memory
  // Input: 32-bit instruction address
  // Output: 32-bit binary instruction
  // Note: Useful to use a 5-to-32 decoder here
  BIT Select[32];
  decoder5(ReadAddress, Select);
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      Instruction[j] = multiplexor2(Select[i], Instruction[j], MEM_Instruction[i][j]);
    }
  }
}

void Control(BIT* OpCode,
  BIT* RegDst, BIT* Jump, BIT* Branch, BIT* MemRead, BIT* MemToReg,
  BIT* ALUOp, BIT* MemWrite, BIT* ALUSrc, BIT* RegWrite, BIT* Link)
{
  // TODO: Set control bits for everything
  // Input: opcode field from the instruction
  // OUtput: all control lines get set 
  // Note: Can use SOP or similar approaches to determine bits
  *ALUSrc = or_gate(OpCode[5], OpCode[3]);
  *RegWrite = or_gate(not_gate(or_gate(OpCode[2], or_gate(and_gate(OpCode[1], not_gate(OpCode[5])), and_gate(OpCode[5], OpCode[3])))),
                      and_gate(OpCode[0], and_gate(OpCode[1], not_gate(OpCode[5]))));
  *Jump = and_gate(OpCode[1], not_gate(OpCode[5]));
  *MemWrite = and_gate(OpCode[5], OpCode[3]);
  ALUOp[0] = OpCode[2];
  ALUOp[1] = not_gate(or_gate(OpCode[3], or_gate(OpCode[2], OpCode[5])));
  *MemToReg = and_gate(OpCode[5], not_gate(OpCode[3]));
  *MemRead = and_gate(OpCode[5], not_gate(OpCode[3]));
  *Branch = OpCode[2];
  *Link = and_gate(not_gate(OpCode[5]), and_gate(OpCode[0], OpCode[1]));
  *RegDst = not_gate(or_gate(and_gate(OpCode[5], not_gate(OpCode[3])), and_gate(OpCode[3], not_gate(OpCode[5]))));
}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2)
{
  // TODO: Implement register read functionality
  // Input: two 5-bit register addresses
  // Output: the values of the specified registers in ReadData1 and ReadData2
  // Note: Implementation will be very similar to instruction memory circuit
  BIT Select1[32];
  BIT Select2[32];
  decoder5(ReadRegister1, Select1);
  decoder5(ReadRegister2, Select2);
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      ReadData1[j] = multiplexor2(Select1[i], ReadData1[j], MEM_Register[i][j]);
      ReadData2[j] = multiplexor2(Select2[i], ReadData2[j], MEM_Register[i][j]);
    }
  }
}

void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData)
{
  // TODO: Implement register write functionality
  // Input: one 5-bit register address, data to write, and control bit
  // Output: None, but will modify register file
  // Note: Implementation will again be similar to those above
  BIT Select[32];
  decoder5(WriteRegister, Select);
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      MEM_Register[i][j] = multiplexor2(and_gate(RegWrite, Select[i]), MEM_Register[i][j], WriteData[j]);
    }
  }
}

void ALU_Control(BIT* ALUOp, BIT* funct, BIT* ALUControl, BIT* Return)
{
  // TODO: Implement ALU Control circuit
  // Input: 2-bit ALUOp from main control circuit, 6-bit funct field from the
  //        binary instruction
  // Output:4-bit ALUControl for input into the ALU
  // Note: Can use SOP or similar approaches to determine bits
  ALUControl[3] = FALSE;
  ALUControl[1] = not_gate(and_gate(ALUOp[1], funct[2]));
  ALUControl[2] = or_gate(ALUOp[0], and_gate(ALUOp[1], funct[1]));
  ALUControl[0] = and_gate(ALUOp[1], or_gate(funct[0], and_gate(funct[3], funct[5])));
  *Return = and_gate(ALUOp[1], and_gate(funct[3], not_gate(funct[5])));
  RegWrite = multiplexor2(*Return, RegWrite, FALSE);
}

void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum)
{
  // TODO: implement a 1-bit adder
  // Note: you can probably copy+paste this from your (or my) Lab 5 solution
  *Sum = xor_gate(xor_gate(A,B), CarryIn);
  *CarryOut = or_gate(and_gate(xor_gate(A,B), CarryIn), and_gate(A,B));
}

void ALU1_bit(BIT A, BIT B, BIT* ALUControl, BIT CarryIn, BIT* CarryOut, BIT* Result, BIT* Set) {
  *Result = FALSE;
  B = multiplexor2(ALUControl[2], B, not_gate(B));
  adder1(A, B, CarryIn, CarryOut, Set);
  *Result = multiplexor2(ALUControl[1], *Result, *Set);
  *Result = multiplexor2(not_gate(or_gate(ALUControl[0], ALUControl[1])), *Result, and_gate(A, B));
  *Result = multiplexor2(and_gate(not_gate(ALUControl[1]), ALUControl[0]), *Result, or_gate(A, B));
  *Result = multiplexor2(and_gate(ALUControl[0], ALUControl[1]), *Result, FALSE);
}

void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result)
{   
  // TODO: Implement 32-bit ALU
  // Input: 4-bit ALUControl, two 32-bit inputs
  // Output: 32-bit result, and zero flag big
  // Note: Can re-use prior implementations (but need new circuitry for zero)
  BIT CarryOut;
  BIT Set;
  *Zero = TRUE;
  BIT CarryIn = ALUControl[2];
  ALU1_bit(Input1[0], Input2[0], ALUControl, CarryIn, &CarryOut, &Result[0], &Set);
  for (int i = 1; i < 32; i++) {
    ALU1_bit(Input1[i], Input2[i], ALUControl, CarryOut, &CarryOut, &Result[i], &Set);
  }
  for (int i = 0; i < 32; i++) {
    *Zero = multiplexor2(Result[i], *Zero,  FALSE);
  }
  Result[0] = multiplexor2(and_gate(ALUControl[0], ALUControl[1]), Result[0], Set);
}

void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData)
{
  // TODO: Implement data memory
  // Input: 32-bit address, control flags for read/write, and data to write
  // Output: data read if processing a lw instruction
  // Note: Implementation similar as above
  BIT Select[32];
  decoder5(Address, Select);
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      MEM_Data[i][j] = multiplexor2(and_gate(MemWrite, Select[i]), MEM_Data[i][j], WriteData[j]);
      ReadData[j] = multiplexor2(and_gate(MemRead, Select[i]), ReadData[j], MEM_Data[i][j]);
    }
  }
}

void Extend_Sign16(BIT* Input, BIT* Output)
{
  // TODO: Implement 16-bit to 32-bit sign extender
  // Copy Input to Output, then extend 16th Input bit to 17-32 bits in Output
  for (int i = 0; i < 16; i++) {
    Output[i] = Input[i];
  }
  for (int i = 16; i < 32; i++) {
    Output[i] = Input[15];
  }
}

void updateState()
{
  // TODO: Implement the full datapath here
  // Essentially, you'll be figuring out the order in which to process each of 
  // the sub-circuits comprising the entire processor circuit. It makes it 
  // easier to consider the pipelined version of the process, and handle things
  // in order of the pipeline. The stages and operations are:
  // Fetch - load instruction from instruction memory
  // Decode - set control bits and read from the register file
  // Execute - process ALU
  // Memory - read/write data memory
  // Write Back - write to the register file
  // Update PC - determine the final PC value for the next instruction
  BIT Instruction[32];
  BIT Data1[32];
  BIT Data2[32];
  BIT Data3[32];
  BIT ALUIn2[32];
  BIT Result[32];
  BIT Branch_value[32];
  BIT WriteAddress[32];
  BIT WriteData[32];
  BIT AddOp[4] = {FALSE, TRUE, FALSE, FALSE};
  BIT Ignore;

  //Instruction Fetch, increment PC
  Instruction_Memory(PC, Instruction);
  ALU(AddOp, PC, ONE, &Ignore, PC);
  //Compute Control signals
  Control(&Instruction[26], &RegDst, &Jump, &Branch, &MemRead, &MemToReg, ALUOp, &MemWrite, &ALUSrc, &RegWrite, &Link);
  ALU_Control(ALUOp, Instruction, ALUControl, &Return);

  //Read registers and extend the sign of the immediate field
  Read_Register(&Instruction[21], &Instruction[16], Data1, Data2);
  Extend_Sign16(Instruction, Data3);
  //Execute ALU and branch adder
  for (int i = 0; i < 32; i++) {
    ALUIn2[i] = multiplexor2(ALUSrc, Data2[i], Data3[i]);  
  }
  ALU(ALUControl, Data1, ALUIn2, &Zero, Result);
  ALU(AddOp, PC, Data3, &Ignore, Branch_value);
  
  //Fetch and/or write Data memory
  Data_Memory(MemWrite, MemRead, Result, Data2, Data3);

  //Select the data to be written and the address to be written to
  for (int i = 0; i < 5; i++) {
    WriteAddress[i] = multiplexor2(Link, multiplexor2(RegDst, Instruction[i+16],
                                   Instruction[i+11]), REG_THIRTY_ONE[i]);
  }
  for (int i = 0; i < 32; i++) {
    WriteData[i] = multiplexor2(Link, multiplexor2(MemToReg, Result[i], Data3[i]), PC[i]);
  }

  //Finally Write Back data to the register, if there is data to be written
  Write_Register(RegWrite, WriteAddress, WriteData);

  //Set the PC according to the control signals and resulting branch value
  for (int i = 0; i < 32; i++) {
    PC[i] = multiplexor2(and_gate(Branch, Zero), PC[i], Branch_value[i]);  
  }
  for (int i = 0; i < 25; i++) {
    PC[i] = multiplexor2(Jump, PC[i], Instruction[i]);
  }
  for (int i = 0; i < 32; i++) {
    PC[i] = multiplexor2(and_gate(Return, not_gate(Jump)), PC[i], Result[i]);  
  }
}


/******************************************************************************/
/* Main */
/******************************************************************************/

int main()
{

  setbuf(stdout, NULL);
    
  // parse instructions into binary format
  int counter = get_instructions(MEM_Instruction);
  
  // load program and run
  copy_bits(ZERO, PC);
  copy_bits(THIRTY_TWO, MEM_Register[29]);
  
  while (binary_to_integer(PC) < counter) {
    print_instruction();
    updateState();
    print_state();
  }
  
  return 0;
}