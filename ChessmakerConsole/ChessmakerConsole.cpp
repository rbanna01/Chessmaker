#if _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW
#endif

#include <tchar.h>
#include <string>
#include <iostream>
#include <fstream>

#pragma comment(lib, "..\\Debug\\ChessmakerCore.lib")

extern "C" __declspec(dllimport)
bool Initialize(char* definition);

extern "C" __declspec(dllimport)
std::string *GetBoardSVG();

extern "C" __declspec(dllimport)
const char *GetCurrentPlayer();

extern "C" __declspec(dllimport)
int PerformMove(const char *notation);

extern "C" __declspec(dllimport)
void Shutdown();

int _tmain(int argc, _TCHAR* argv[])
{
	// open definition file
	std::ifstream def("Definition.xml", std::ifstream::in | std::ifstream::binary);
	def.seekg(0, std::ios::end);
	int length = def.tellg();
	if (length == -1)
	{
		def.close();
		return 1;
	}
	def.seekg(0, std::ios::beg);
	char *definition = new char[length + 1];
	def.read(definition, length);
	definition[length] = 0;
	def.close();
	
	// initialize game engine
	if (!Initialize(definition))
	{
		printf("Error parsing definition\n");
		delete definition;
		return 1;
	}
	else
		printf("Definition parsed successfully\n");

	delete definition;

	// retrieve and then save the board SVG file
	std::string *svg = GetBoardSVG();
	printf("received %i bytes:\n%s\n", svg->size(), svg->c_str());

	std::ofstream ofs("Render.svg", std::ofstream::out);
	ofs << svg->c_str();
	ofs.close();

	delete svg;

	int retVal = 0;
	std::string input;
	do
	{
		if (retVal == -1)
			printf("Invalid input, please retry\n");
		else
			printf("%s to move\n", GetCurrentPlayer());

		std::getline(std::cin, input);
		if (input.length() == 0)
		{
			printf("Aborting...\n");
			break;
		}

		retVal = PerformMove(input.c_str());
	} while (retVal != 0);

	printf("Game over\n");

	Shutdown();

	_CrtDumpMemoryLeaks();
	getchar();

	return 0;
}

