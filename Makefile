CC = g++

all: include/onegin.cpp include/stack.cpp Assembler/assembler.cpp Assembler/main.cpp SoftCPU/cpu.cpp SoftCPU/main.cpp \
	 Disassembler/disassembler.cpp Disassembler/main.cpp
	$(CC) include/onegin.cpp Assembler/assembler.cpp Assembler/main.cpp -o asm
	$(CC) include/stack.cpp SoftCPU/cpu.cpp SoftCPU/main.cpp -o cpu
	$(CC) Disassembler/disassembler.cpp Disassembler/main.cpp -o disasm

asm: include/onegin.cpp Assembler/assembler.cpp Assembler/main.cpp
	$(CC) include/onegin.cpp Assembler/assembler.cpp Assembler/main.cpp -o asm

cpu: include/stack.cpp SoftCPU/cpu.cpp SoftCPU/main.cpp
	$(CC) include/stack.cpp SoftCPU/cpu.cpp SoftCPU/main.cpp -o cpu

disasm: Disassembler/disassembler.cpp Disassembler/main.cpp
	$(CC) Disassembler/disassembler.cpp Disassembler/main.cpp -o disasm