#pragma once

#include <memory>
#include "Util/Gloabl.h"

MAKE_SMART_PTR(EObject);

class EObject : public std::enable_shared_from_this<EObject>
{
public:

	virtual ~EObject() = 0;
};

