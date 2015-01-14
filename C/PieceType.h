#pragma once
#include <list>

class PieceType
{
public:
	PieceType();
	~PieceType();

	static std::list<PieceType> allTypes;
private:
	int value = 1;
	char notation[3];
	PieceType *capturedAs;
};

