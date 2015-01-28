#include <tchar.h>
#include <string.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "..\\Debug\\ChessmakerCore.lib")

extern "C" __declspec(dllimport)
bool Initialize(char* definition);

extern "C" __declspec(dllimport)
std::string *GetBoardSVG();

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
		printf("error");
		return 1;
	}
	
	// retrieve and then save the board SVG file
	std::string *svg = GetBoardSVG();
	printf("received %i bytes:\n%s", svg->size(), svg->c_str());

	std::ofstream ofs("Render.svg", std::ofstream::out);
	ofs << svg->c_str();
	ofs.close();

	delete svg;
	delete definition;
	
	getchar();
	return 0;
}

