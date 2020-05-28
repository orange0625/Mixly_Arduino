// MultiStepper_MCP23017.cpp
//
// Copyright (C) 2015 Mike McCauley
// $Id: MultiStepper_MCP23017.cpp,v 1.2 2015/10/04 05:16:38 mikem Exp $

#include "MultiStepper_MCP23017.h"
#include "AccelStepper_MCP23017.h"

MultiStepper_MCP23017::MultiStepper_MCP23017()
    : _num_steppers(0)
{
}

boolean MultiStepper_MCP23017::addStepper(AccelStepper_MCP23017& stepper)
{
    if (_num_steppers >= MULTISTEPPER_MAX_STEPPERS)
	return false; // No room for more
    _steppers[_num_steppers++] = &stepper;
    return true;
}

void MultiStepper_MCP23017::moveTo(long absolute[])
{
    // First find the stepper that will take the longest time to move
    float longestTime = 0.0;

    uint8_t i;
    for (i = 0; i < _num_steppers; i++)
    {
	long thisDistance = absolute[i] - _steppers[i]->currentPosition();
	float thisTime = abs(thisDistance) / _steppers[i]->maxSpeed();

	if (thisTime > longestTime)
	    longestTime = thisTime;
    }

    if (longestTime > 0.0)
    {
	// Now work out a new max speed for each stepper so they will all 
	// arrived at the same time of longestTime
	for (i = 0; i < _num_steppers; i++)
	{
	    long thisDistance = absolute[i] - _steppers[i]->currentPosition();
	    float thisSpeed = thisDistance / longestTime;
	    _steppers[i]->moveTo(absolute[i]); // New target position (resets speed)
	    _steppers[i]->setSpeed(thisSpeed); // New speed
	}
    }
}

// Returns true if any motor is still running to the target position.
boolean MultiStepper_MCP23017::run()
{
    uint8_t i;
    boolean ret = false;
    for (i = 0; i < _num_steppers; i++)
    {
	if ( _steppers[i]->distanceToGo() != 0)
	{
	    _steppers[i]->runSpeed();
	    ret = true;
	}
    }
    return ret;
}

// Blocks until all steppers reach their target position and are stopped
void    MultiStepper_MCP23017::runSpeedToPosition()
{ 
    while (run())
	;
}
