#pragma once
#include <string>
#include <vector>
#include <sstream>

class Format
{
public:
	template<typename ...Args>
	static std::string format(const char* format, Args&&... args)
	{
		static constexpr size_t BufferSize = 512;
		thread_local char buffer[BufferSize];
		memset(buffer, 0, BufferSize);

		int sz = std::snprintf(buffer, BufferSize, format, std::forward<Args>(args)...);
		return std::string(buffer, buffer + sz);
	}

	static std::string join(const std::vector<std::string>& strs, const char* delimiter)
	{
		std::stringstream ss;
		int sz = (int)strs.size();
		for (int i = 0; i < sz; ++i)
		{
			ss << strs[i];
			if (i != sz - 1)
			{
				ss << delimiter;
			}
		}

		return ss.str();
	}
};
