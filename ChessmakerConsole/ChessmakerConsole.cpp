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
#include <ctime>
#include <limits.h>

#if _DEBUG
#pragma comment(lib, "..\\Debug\\ChessmakerCore.lib")
#else
#pragma comment(lib, "..\\Release\\ChessmakerCore.lib")
#endif

extern "C" __declspec(dllimport)
bool Parse(char* definition);

extern "C" __declspec(dllimport)
void Start();
/*
extern "C" __declspec(dllimport)
const char *GetBoardSVG();
*/
extern "C" __declspec(dllimport)
bool SetPlayerLocal(int number);

extern "C" __declspec(dllimport)
bool SetPlayerRemote(int number);

extern "C" __declspec(dllimport)
bool SetPlayerAI(int number, const char *aiName);

extern "C" __declspec(dllimport)
void Shutdown();

extern "C" __declspec(dllimport)
int PerformMove(const char *notation);

void RunGameLoop(bool anyLocalPlayer);

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
	if (!Parse(definition))
	{
		printf("Error parsing definition\n");
		delete definition;
		return 1;
	}

	delete definition;

	bool setupFailed = false, anyLocalPlayer;
	
	do
	{
		std::string input;
		printf("Game type? 1 = local, 2 = vs AI, 3 = AI vs AI\n");

		std::getline(std::cin, input);
		if (input.compare("1") == 0)
		{
			if (!SetPlayerLocal(1))
			{
				setupFailed = true;
				printf("Error setting player to LOCAL\n");
			}
			else if (!SetPlayerLocal(2))
			{
				setupFailed = true;
				printf("Error setting player to LOCAL\n");
			}
			anyLocalPlayer = true;
			break;
		}
		else if (input.compare("2") == 0)
		{
			if (!SetPlayerLocal(1))
			{
				setupFailed = true;
				printf("Error setting player to LOCAL\n");
			}
			else if (!SetPlayerAI(2, "random capture"))
			{
				setupFailed = true;
				printf("Error setting player to AI\n");
			}
			anyLocalPlayer = true;
			break;
		}
		else if (input.compare("3") == 0)
		{
			if (!SetPlayerAI(1, "random capture"))
			{
				setupFailed = true;
				printf("Error setting player to AI\n");
			}
			else if (!SetPlayerAI(2, "random capture"))
			{
				setupFailed = true;
				printf("Error setting player to AI\n");
			}
			anyLocalPlayer = false;
			break;
		}
	} while (true);

/*
	// retrieve and then save the board SVG file
	std::string *svg = GetBoardSVG();
	printf("received %i bytes:\n%s\n", svg->size(), svg->c_str());

	std::ofstream ofs("Render.svg", std::ofstream::out);
	ofs << svg->c_str();
	ofs.close();

	delete svg;
*/
	
	if (!setupFailed)
		RunGameLoop(anyLocalPlayer);

	Shutdown();

	_CrtDumpMemoryLeaks();
	getchar();

	return 0;
}

void RunGameLoop(bool anyLocalPlayer)
{
	srand(time(NULL));
	Start();

	if (!anyLocalPlayer)
		return;

	int retVal = 0;
	std::string input;
	do
	{
		if (retVal == -1)
			printf("Invalid input, please retry\n");
		else
			printf("Enter move: ");

		std::getline(std::cin, input);
		if (input.length() == 0)
		{
			printf("Aborting...\n");
			break;
		}

		retVal = PerformMove(input.c_str());
	} while (retVal != 0);

	printf("Game over\n");
}