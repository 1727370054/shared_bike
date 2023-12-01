#include "events_def.h"

#include <iostream>
#include <string>

std::ostream& MobileCodeReqEv::dump(std::ostream& out) const
{
	out << "MobileCodeReq sn: " << get_sn() << " ";
	out << "mobile: " << msg_.mobile() << std::endl;
	return out;
}

std::ostream& MobileCodeRspEv::dump(std::ostream& out) const
{
	out << "MobileCodeReq sn: " << get_sn() << " ";
	out << "code: " << msg_.code() << " ";
	out << "icode: " << msg_.icode() << " ";
	out << "descibe: " << msg_.desc() << std::endl;
	return out;
}

std::ostream& LoginReqEv::dump(std::ostream& out) const
{
	out << "MobileCodeReq sn: " << get_sn() << " ";
	out << "mobile: " << msg_.mobile() << " ";
	out << "icode: " << msg_.icode() << std::endl;
	return out;
}

std::ostream& LoginRspEv::dump(std::ostream& out) const
{
	out << "MobileCodeReq sn: " << get_sn() << " ";
	out << "code: " << msg_.code() << " ";
	out << "descibe: " << msg_.desc() << std::endl;
	return out;
}

std::ostream& RechargeReqEv::dump(std::ostream& out) const
{
	out << "MobileCodeReq sn: " << get_sn() << " ";
	out << "mobile: " << msg_.mobile() << " ";
	out << "amount: " << msg_.amount() << std::endl;
	return out;
}

std::ostream& RechargeRspEv::dump(std::ostream& out) const
{
	out << "MobileCodeReq sn: " << get_sn() << " ";
	out << "code: " << msg_.code() << " ";
	out << "balance: " << msg_.balance() <<" ";
	out << "descibe: " << msg_.desc() << std::endl;
	return out;
}
