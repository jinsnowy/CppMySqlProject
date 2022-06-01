#pragma once
#include <string>
#include <vector>
#include <sstream>

class Format
{
private:
	template<typename T, typename = std::enable_if_t<!std::is_same_v<std::string, std::decay_t<T>> && !std::is_same_v<std::wstring, std::decay_t<T>>>>
	using FormatArg = T;

public:
	template<typename ...Ts>
	static std::string format(const char* format, FormatArg<Ts>&&... args)
	{
		static constexpr size_t BufferSize = 512;
		thread_local char buffer[BufferSize];
		memset(buffer, 0, BufferSize);

		int sz = std::snprintf(buffer, BufferSize, format, std::forward<FormatArg<Ts>>(args)...);
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
