#include "TurnOrder.h"
#include "Player.h"

int nextID = 1;
TurnRepeat::TurnRepeat(int count)
{
	maxRepeats = count;
	currentIteration = 0;
	state = AtStart;
	id = nextID++; neverUsed = true;
}


TurnRepeat::~TurnRepeat()
{
	while (!steps.empty())
		delete steps.front(), steps.pop_front();
}


TurnStep* TurnRepeat::GetNext(bool forwards)
{
	/*if (neverUsed)
		printf("repeat %i is in state %i\n", id, (int)state);
	else
		printf("repeat %i is at position %i, in state %i\n", id, std::distance(steps.begin(), it), (int)state);*/
	if (state == AtStart)
	{
		if (!forwards)
			return 0;

		it = steps.begin();
	}
	else if (state == AtEnd)
	{
		if (forwards)
			return 0; // if already used up ALL my loops, 

		it = steps.end();
		it--;
	}

	neverUsed = false;
	if (state != AtEnd && !(*it)->IsStep())
	{
		TurnRepeat *repeat = (TurnRepeat*)*it;
		//printf("path 1   ");
		TurnStep *step = repeat->GetNext(forwards);
		if (step != 0)
		{
			state = InMiddle;
			return step;
		}
	}

	if (state != InMiddle) // on the first use, don't increment this iterator
		state = InMiddle;
	else if (forwards)
	{
		it++;
		if (it == steps.end())
		{
			if (maxRepeats != 0 && currentIteration >= maxRepeats - 1)
			{
				state = AtEnd;
				return 0; // gone past the end on last iteration
			}

			currentIteration++;
			it = steps.begin();
		}
	}
	else
	{
		if (it == steps.begin())
		{
			if (maxRepeats != 0 && currentIteration <= 0)
			{
				state = AtStart;
				return 0; // gone past the beginning on first iteration
			}

			currentIteration--;
			it = steps.end();
			it--;
		}
		else
			it--;
	}

	if ((*it)->IsStep())
	{
		TurnStep *step = (TurnStep*)*it;
		return step;
	}

	//printf("path 2   ");
	TurnRepeat *repeat = (TurnRepeat*)*it;
	return repeat->GetNext(forwards);
}


TurnOrder::TurnOrder() : TurnRepeat(1)
{

}


TurnOrder::~TurnOrder()
{

}


TurnOrder *TurnOrder::CreateDefault(std::list<Player*> players)
{
	TurnOrder *order = new TurnOrder();

	TurnRepeat *repeat = new TurnRepeat(0);
	order->steps.push_back(repeat);

	for (auto it = players.begin(); it != players.end(); it++)
		repeat->steps.push_back(new TurnStep(*it));

	return order;
}


Player *TurnOrder::GetNextPlayer()
{
	currentStep = GetNext(true);
	if (currentStep == 0)
		return 0; 
	return currentStep->GetPlayer();
}


Player *TurnOrder::StepBackward()
{
	currentStep = GetNext(false);
	if (currentStep == 0)
		return 0;
	return currentStep->GetPlayer();
}
