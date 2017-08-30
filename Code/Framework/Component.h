#pragma once

#include "Object.h"

class EComponent : public EObject
{
public:
	virtual void Tick(float del) = 0;
};