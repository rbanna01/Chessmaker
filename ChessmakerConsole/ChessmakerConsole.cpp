#if _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new DEBUG_NEW
#endif

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <limits.h>
#include <tchar.h>
#include <strsafe.h>
#include <windows.h>


#if _DEBUG
#pragma comment(lib, "..\\Debug\\ChessmakerCore.lib")
#else
#pragma comment(lib, "..\\Release\\ChessmakerCore.lib")
#endif

extern "C" __declspec(dllimport)
bool Parse(char* definition);

extern "C" __declspec(dllimport)
int Start();
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

extern "C" __declspec(dllimport)
int PerformAIMove();

#define MOVE_RESULT_DONE 0
#define MOVE_RESULT_NEXT_LOCAL 1
#define MOVE_RESULT_NEXT_AI 2
#define MOVE_RESULT_ERROR -1

int RunUnitTests();
int RunPerformanceTests();
int SetupGame();

int _tmain(int argc, _TCHAR* argv[])
{
	int retVal;
	do
	{
		printf("Operation? 1 = unit tests, 2 = performance tests, 3 = play\n");
		std::string input;
		std::getline(std::cin, input);

		if (input.compare("1") == 0)
			retVal = RunUnitTests();
		else if (input.compare("2") == 0)
			retVal = RunPerformanceTests();
		else if (input.compare("3") == 0)
			retVal = SetupGame();
		else
			continue;
		
		break;
	} while (true);

	_CrtDumpMemoryLeaks();
	getchar();
	return retVal;
}


int IterateVariants(void (*f)(WCHAR *))
{
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	StringCchCopy(szDir, MAX_PATH, TEXT("Variants\\*"));
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	hFind = FindFirstFile(szDir, &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf("No files found\n");
		return dwError;
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(ffd.cFileName, TEXT("..")) != 0 && wcscmp(ffd.cFileName, TEXT(".")) != 0)
				f(ffd.cFileName);
				//_tprintf(TEXT("  %s\n"), ffd.cFileName);
		}
		/*else
		{
		filesize.LowPart = ffd.nFileSizeLow;
		filesize.HighPart = ffd.nFileSizeHigh;
		_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
		}*/
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("An error occurred looking for subdirectories\n");
	}

	FindClose(hFind);
	return dwError;
}


bool ParseDefinition(std::wstring variantName)
{
	std::wstring variantFile = TEXT("Variants\\");
	variantFile += variantName;
	variantFile += TEXT("\\Definition.xml");

	// open definition file
	std::ifstream def(variantFile, std::ifstream::in | std::ifstream::binary);
	def.seekg(0, std::ios::end);
	int length = def.tellg();
	if (length == -1)
	{
		printf("File not found: %s\n", variantFile);
		def.close();
		return false;
	}
	def.seekg(0, std::ios::beg);
	char *definition = new char[length + 1];
	def.read(definition, length);
	definition[length] = 0;
	def.close();

	bool retVal = Parse(definition);
	delete definition;

	if (!retVal)
		_tprintf(TEXT("Error parsing variant definition: %s\n"), variantFile);
	return retVal;
}


// unit testing

void RunTestsForVariant(WCHAR *name)
{
	_tprintf(TEXT("Testing variant: %s\n"), name);

	if (ParseDefinition(name))
	{
		Start(); // err... so we really want to load every "test game" in this directory and run it. Re-parse the game definition itself each time, in turn. Spit out an error if a move can't be made, or if the # of available moves doesn't match the stored number.
	}
	
	Shutdown();
}


int RunUnitTests()
{
	IterateVariants(RunTestsForVariant);
	printf("Done\n");
	return 0;
}


// performance testing

#define NUM_REPEATS 3
void PlayAIGameForVariant(WCHAR *name)
{
	_tprintf(TEXT("Testing variant: %s\n"), name);

	double totalDuration = 0;

	for (int i = 0; i < NUM_REPEATS; i++)
	{
		srand(0);

		//if (ParseDefinition(name) && SetPlayerAI(1, "alpha beta 2") && SetPlayerAI(2, "alpha beta 2"))
		if (ParseDefinition(name) && SetPlayerAI(1, "random capture") && SetPlayerAI(2, "random capture"))
		{
			clock_t startTime = clock();
			int retVal = Start();

			do
			{
				if (retVal == MOVE_RESULT_NEXT_AI)
				{
					retVal = PerformAIMove();
				}
				else
				{
					printf("Invalid AI move return code, aborting\n");
					Shutdown();
					return;
				}
			} while (retVal != MOVE_RESULT_DONE);

			double duration = (clock() - startTime) / (double)CLOCKS_PER_SEC;
			totalDuration += duration;
			_tprintf(TEXT("Completed %s game in %f seconds\n"), name, duration);
		}

		Shutdown();
	}

	_tprintf(TEXT("Mean %s game duration is %f seconds\n"), name, totalDuration / (double)NUM_REPEATS);
}

int RunPerformanceTests()
{
	IterateVariants(PlayAIGameForVariant);

	printf("Done\n");
	return 0;
}


// game playing

void RunGameLoop();


void Print(WCHAR *name)
{
	_tprintf(TEXT("  %s\n"), name);
}


int SetupGame()
{
	printf("Select a variant:\n");
	IterateVariants(Print);

	std::wstring variantName;
	std::getline(std::wcin, variantName);

	if (!ParseDefinition(variantName))
		return 1;

	bool setupFailed = false;
	
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
			break;
		}
		else if (input.compare("2") == 0)
		{
			if (!SetPlayerLocal(1))
			{
				setupFailed = true;
				printf("Error setting player to LOCAL\n");
			}
			else if (!SetPlayerAI(2, "alpha beta 2"))
			{
				setupFailed = true;
				printf("Error setting player to AI\n");
			}
			break;
		}
		else if (input.compare("3") == 0)
		{
			if (!SetPlayerAI(1, "alpha beta 2"))
			{
				setupFailed = true;
				printf("Error setting player to AI\n");
			}
			else if (!SetPlayerAI(2, "alpha beta 2"))
			{
				setupFailed = true;
				printf("Error setting player to AI\n");
			}
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
		RunGameLoop();

	Shutdown();

	return 0;
}


void RunGameLoop()
{
	srand(time(NULL));
	int retVal = Start();
	std::string input;
	do
	{
		if (retVal == MOVE_RESULT_NEXT_AI)
		{
			retVal = PerformAIMove();
			continue;
		}

		if (retVal == MOVE_RESULT_ERROR)
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
	} while (retVal != MOVE_RESULT_DONE);

	printf("Game over\n");
}