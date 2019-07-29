#pragma once
class MoveArea
{
public:
	MoveArea();
	virtual ~MoveArea();
	bool checkMoveArea(float distX, float distY, int area);
};

