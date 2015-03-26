#pragma once

#include "Definitions.h"

class Player;


class BaseTurnStep
{
public:
	virtual bool IsStep() = 0;
	virtual ~BaseTurnStep() { };
protected:
	BaseTurnStep() { };
};


class TurnStep : public BaseTurnStep
{
public:
	TurnStep(Player *player) { this->player = player; }
	Player *GetPlayer() { return player; }
	virtual bool IsStep() { return true; }
private:
	Player *player;
};


class TurnRepeat : public BaseTurnStep
{
public:
	TurnRepeat(int count);
	virtual ~TurnRepeat();

	virtual bool IsStep() { return false; }

protected:
	TurnStep* GetNext(bool forwards);
	std::list<BaseTurnStep*> steps;

private:
	typedef enum { AtStart, InMiddle, AtEnd } State_t;

	int currentIteration, maxRepeats;
	State_t state;
	std::list<BaseTurnStep*>::iterator it;

	friend class GameParser;
	friend class TurnOrder;
};


class TurnOrder : public TurnRepeat
{
public:
	TurnOrder();
	virtual ~TurnOrder();

	Player *GetNextPlayer();
	void StepBackward();

	static TurnOrder *CreateDefault(std::list<Player*> players);

private:
	TurnStep *currentStep;

	friend class GameParser;
};