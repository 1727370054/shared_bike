#pragma once

#include "global_def.h"

#include <iostream>
#include <string>

class iEvent
{
public:
	iEvent(u32 eid, u32 sn);

	virtual ~iEvent();
	virtual std::ostream& dump(std::ostream& out) const { return out; }
	virtual i32 ByteSize() { return 0; }
	virtual bool SerializeToArray(char* buffer, i32 len) { return true; }

	u32 generateSeqNo();
	u32 get_eid() const { return eid_; }
	u32 get_sn() const { return sn_; }
	void set_args(void* args) { args_ = args; }
	void* get_args() const { return args_; }

	void set_eid(u32 eid) { eid_ = eid; }

private:
	u32 eid_;
	u32 sn_;
	void* args_;
};