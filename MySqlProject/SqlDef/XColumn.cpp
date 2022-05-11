#include "stdafx.h"
#include "XColumn.h"

using namespace sqldef;

static std::string wrap(const std::string& s, char c = '`')
{
	return Format::format("%c%s%c", c, s.c_str(), c);
}

std::string sqldef::XColumn::Build() const
{
	std::stringstream ss;
	ss << wrap(mName) << " ";	
	for (const auto& attr : mAttributes)
	{
		ss << attr.second->Tag() << " ";
	}

	return ss.str();
}
