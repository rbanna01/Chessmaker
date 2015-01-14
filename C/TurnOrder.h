#pragma once

class Player;

class TurnOrder
{
public:
	TurnOrder();
	~TurnOrder();

	Player *GetNextPlayer();
	void StepBackward();
};

