#pragma once

#include "ievent.h"
#include "event_type.h"

#include <string>

class iEventHandler
{
public:
	virtual iEvent* handle(const iEvent* ev) { return nullptr; }
	virtual ~iEventHandler() {}

	iEventHandler(const std::string& name) : name_(name) {}

	const std::string& getName() { return name_; }
private:
	std::string name_;
};
