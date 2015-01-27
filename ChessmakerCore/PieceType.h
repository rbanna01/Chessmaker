#pragma once
#include <list>

class PieceType
{
public:
	PieceType();
	~PieceType();
private:
	int value = 1;
	char notation[3];
	char name[32];
	PieceType *capturedAs;

	friend class GameParser;
};

