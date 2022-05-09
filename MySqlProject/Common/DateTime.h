#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

class DateTime : public tm
{
public:
	static DateTime Now()
	{
		auto t = std::time(nullptr);
		return *reinterpret_cast<DateTime*>(std::localtime(&t));
	}

	bool IsValid()
	{
		switch (tm_mon)	
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return tm_mday <= 31;
		case 4:
		case 6:
		case 9:
		case 11:
			return tm_mday <= 30;
		case 2:
			return tm_mday <= 28;

		default:
			break;
		}

		return false;
	}

	std::string ToString(const char* format = "%Y-%m-%dT%H:%M:%S") const
	{
		std::ostringstream oss;
		oss << std::put_time((tm*)this, format);
		return oss.str();
	}

	std::string ToSqlString() const
	{
		return ToString("%Y-%m-%d %H:%M:%S");
	}
};

