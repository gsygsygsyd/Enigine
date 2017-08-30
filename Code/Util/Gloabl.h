#pragma once

#include <memory>

#define MAKE_SMART_PTR(cls)\
class cls;\
typedef std::shared_ptr<cls> cls##Ptr;\
typedef std::weak_ptr<cls> cls##WPtr;\
typedef std::unique_ptr<cls> cls##UPtr;