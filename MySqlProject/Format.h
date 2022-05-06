#pragma once
#include <string>

class Format
{
public:
	template<typename ...Args>
	static std::string format(const char* format, Args&&... args)
	{
		static constexpr size_t BufferSize = 512;

		char buffer[BufferSize] = {};
		int sz = std::snprintf(buffer, BufferSize, format, std::forward<Args>(args)...);
		return std::string(buffer, buffer + sz);
	}
};
