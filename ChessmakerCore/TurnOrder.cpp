#include "TurnOrder.h"
#include "Player.h"

TurnRepeat::TurnRepeat(int count)
{
	maxRepeats = count;
	currentIteration = 0;
	state = AtStart;
}


TurnRepeat::~TurnRepeat()
{
	while (!steps.empty())
		delete steps.front(), steps.pop_front();
}


TurnStep* TurnRepeat::GetNext(bool forwards)
{
	if (state == AtStart)
	{
		if (!forwards)
			return 0;

		it = steps.begin();
	}
	else if (state == AtEnd)
	{
		if (forwards)
			return 0;

		it = steps.end();
		it--;
	}

	if (state != AtEnd && !(*it)->IsStep())
	{
		TurnRepeat *repeat = (TurnRepeat*)*it;
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

	// arriving into a repeat for the first time, so reset it
	TurnRepeat *repeat = (TurnRepeat*)*it;
	if (forwards)
	{
		repeat->currentIteration = 0;
		repeat->state = AtStart;
	}
	else
	{
		repeat->currentIteration = repeat->maxRepeats - 1;
		repeat->state = AtEnd;
	}
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


void TurnOrder::StepBackward()
{
	currentStep = GetNext(false);
}
