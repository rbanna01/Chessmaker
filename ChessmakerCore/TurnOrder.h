#pragma once

#include <list>

class Player;


class BaseTurnStep
{
public:
	virtual bool IsStep() = 0;
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
	~TurnRepeat();

	virtual bool IsStep() { return false; }
	int id; bool neverUsed;
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
	~TurnOrder();

	Player *GetNextPlayer();
	Player *StepBackward();

	static TurnOrder *CreateDefault(std::list<Player*> players);

private:
	TurnStep *currentStep;

	friend class GameParser;
};