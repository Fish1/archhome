#include <iostream>
#include <fstream>

#include <array>
#include <string>
#include <cstdint>

// sleep
#include <unistd.h>

#include "defines.hpp"

bool HALT = false;
bool FAIL = false;
bool CMP_RESULT = false;

uint16_t RAM [RAM_SIZE];
size_t RAM_LOCATION = 0;

size_t MOVES = 0;
size_t INSTRUCTIONS = 0;

void dumpRam()
{
	std::cout << " --- RAM DUMP --- " << std::endl;
	for(size_t i = 0; i < RAM_SIZE; ++i) {
		std::cout << std::hex << (char)(RAM[i] >> 8) << std::hex << (char)(RAM[i] >> 0);
	}
	std::cout << std::endl;
	std::cout << " --- END RAM DUMP --- " << std::endl;
}

int TAPE_HEAD = 0;
std::array<char, TAPE_SIZE> TAPE;
std::array<bool, 255> ALPHABET;

void dumpTape()
{
#ifdef VERBOSE
	std::cout << " --- DUMP TAPE --- " << std::endl;
#endif
	for(unsigned int i = 0; i < TAPE_SIZE; ++i)
	{
		if(TAPE_HEAD == i) 
		{
			std::cout << "[" << TAPE[i] << "]";
		} 
		else 
		{
			std::cout << TAPE[i];
		}
	}

	std::cout << std::endl;
#ifdef VERBOSE
	std::cout << " --- END DUMP TAPE --- " << std::endl;
#endif
}

struct DecodeData 
{
	uint16_t op;
	uint16_t address;
	char letter;
	bool blank;
	bool on;
};

void message(std::string msg)
{
	std::cout << "Message: " + msg + "\t| TAPE: " + std::to_string(TAPE_HEAD) + "\t| RAM: 0x" + std::to_string(RAM_LOCATION) + "\t| CMP: " + std::to_string(CMP_RESULT) << std::endl;
}

void error(std::string msg, int exitCode = 1)
{
	std::cout << "Error: " + msg << std::endl;
	exit(exitCode);
}

void run();

uint16_t fetch();
DecodeData decode(const uint16_t raw);
void execute(const DecodeData & data);

/* TAPE CONTROL */
void opLeft(const DecodeData & data);
void opRight(const DecodeData & data);
void opDraw(const DecodeData & data);
void opAlpha(const DecodeData & data);
void opBrae(const DecodeData & data);
void opBrane(const DecodeData & data);
void opBra(const DecodeData & data);
void opCmp(const DecodeData & data);
void opFail();
void opHalt();
/* END TAPE CONTROL */

void loadRam(std::string filename)
{
	std::ifstream file(filename, std::ios::binary);

	if(file.is_open())
	{
		size_t ramIndex = 0;
		unsigned char buff[sizeof(uint16_t)];
		while(file.read((char*)buff, sizeof(uint16_t))) 
		{
			uint16_t d = buff[0];
			d <<= 8;
			d |= buff[1];
			RAM[ramIndex++] = d;
		}

#ifdef DUMP_RAM
		dumpRam();
#endif

	}
	else
	{
		error(std::string("failed to open file \"") + filename + "\"");
	}

	file.close();
}

void loadTape(std::string tape)
{
	TAPE.fill(0);
	for(size_t index = 0; index < tape.size(); ++index)
	{
		TAPE[index] = tape[index];
	}
}

int main(int argc, char ** argv) 
{
	if(argc == 3)
	{
		std::ifstream tapeFile(argv[2]);

		if(tapeFile.is_open())
		{
			std::string tape;

			while(std::getline(tapeFile, tape))
			{
				INSTRUCTIONS = 0;
				MOVES = 0;
				HALT = false;
				FAIL = false;
				CMP_RESULT = false;
				RAM_LOCATION = 0;
				TAPE_HEAD = 0;
				loadRam(argv[1]);
				loadTape(tape);
				ALPHABET.fill(false);
				ALPHABET[0] = true;
				run();
			}
		}
		else
		{
			error(std::string("failed to open file \"") + argv[2] + "\"");
		}
	}
	else if(argc == 2)
	{
		loadRam(argv[1]);
		loadTape("");
		ALPHABET.fill(false);
		ALPHABET[0] = true;
		run();
	}
	else
	{
		error("invalid usage");
	}

	return 0;
}

void run() 
{
	while(!HALT && !FAIL)
	{
		/* FETCH -> DECODE -> EXECUTE */
		execute(decode(fetch()));
#ifdef SLOW
		usleep(10000);
#endif
	}

	if(HALT)
	{	
		std::cout << "Halted after " << std::endl;
	}
	else if(FAIL)
	{
		std::cout << "Failed after " << std::endl;
	}

	std::cout << MOVES << " moves and " << INSTRUCTIONS << " instructions executed" << std::endl;	
	dumpTape();
	std::cout << std::endl; 
}

uint16_t fetch()
{
	return RAM[RAM_LOCATION++];
}

DecodeData decode(const uint16_t raw)
{
	DecodeData data;
	data.op = (raw & 0b0000000011110000) >> 4;
	uint16_t addressTop = (raw & 0b0000000000001111) << 12;
	uint16_t addressBottom = (raw & 0b1111111100000000) >> 8;
	data.address = addressTop | addressBottom;
	data.letter = (raw & 0b1111111100000000) >> 8;
	data.blank = raw & 0b0000000000001000;
	data.on = data.blank;
	return data;
}

void execute(const DecodeData & data)
{
	INSTRUCTIONS += 1;

	switch(data.op)
	{
		case OP_LEFT:
			opLeft(data);
			break;
		case OP_RIGHT:
			opRight(data);
			break;
		case OP_HALT:
			opHalt();
			break;
		case OP_FAIL:
			opFail();
			break;
		case OP_DRAW:
			opDraw(data);
			break;
		case OP_ALPHA:
			opAlpha(data);
			break;
		case OP_BRAE:
			opBrae(data);
			break;
		case OP_BRANE:
			opBrane(data);
			break;
		case OP_BRA:
			opBra(data);
			break;
		case OP_CMP:
			opCmp(data);
			break;
	}
}

void opLeft(const DecodeData & data)
{
	TAPE_HEAD -= 1;
	MOVES += 1;
#ifdef VERBOSE
	message("OP_LEFT");	
#endif
}

void opRight(const DecodeData & data)
{
	TAPE_HEAD += 1;
	MOVES += 1;
#ifdef VERBOSE
	message("OP_RIGHT");
#endif
}

void opDraw(const DecodeData & data)
{
	if(data.blank)
	{
		TAPE[TAPE_HEAD] = 0;
	}
	else
	{
		TAPE[TAPE_HEAD] = data.letter;
	}
#ifdef VERBOSE
	message("OP_DRAW");
#endif
}

void opAlpha(const DecodeData & data)
{
	ALPHABET[data.letter] = data.on;
#ifdef VERBOSE
	message("OP_ALPHA");
#endif
}

void opBrae(const DecodeData & data)
{
	if(CMP_RESULT == true)
	{
		RAM_LOCATION = data.address;
	}
#ifdef VERBOSE
	message("OP_BRAE");
#endif
}

void opBrane(const DecodeData & data)
{
	if(CMP_RESULT == false)
	{
		RAM_LOCATION = data.address;
	}
#ifdef VERBOSE
	message("OP_BRANE");
#endif
}

void opBra(const DecodeData & data)
{
	RAM_LOCATION = data.address;	
#ifdef VERBOSE
	message("OP_BRA");
#endif
}

void opCmp(const DecodeData & data)
{
	char dataLetter = data.letter;

	if(data.blank)
	{
		dataLetter = 0;
	}

	if(ALPHABET[dataLetter] == false)
	{
		opFail();		
	}

	if(TAPE_HEAD >= TAPE_SIZE || TAPE_HEAD < 0)
	{
		CMP_RESULT = dataLetter == 0;
#ifdef VERBOSE
	message(std::string("") + dataLetter + " = .");
	message("OP_CMP");
#endif
	}
	else
	{
		CMP_RESULT = dataLetter == TAPE[TAPE_HEAD];
#ifdef VERBOSE
	message(std::string("") + dataLetter + " = " + TAPE[TAPE_HEAD]);
	message("OP_CMP");
#endif
	}
}

void opFail()
{
	FAIL = true;
#ifdef VERBOSE
	message("OP_FAIL");
#endif
}

void opHalt()
{
	HALT = true;
#ifdef VERBOSE
	message("OP_HALT");
#endif
}
