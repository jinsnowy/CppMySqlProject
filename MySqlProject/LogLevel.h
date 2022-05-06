#pragma once

enum class ELogLevel
{
	Debug,
	Warning,
	Error,
	Fatal,
};

struct SLogLevel
{
	static const char* GetTag(ELogLevel level)
	{
		switch (level)
		{
		case ELogLevel::Debug:
			return "Debug";
		case ELogLevel::Warning:
			return "Warning";
		case ELogLevel::Error:
			return "Error";
		case ELogLevel::Fatal:
			return "Fatal";
		default:
			return "Unknown";
		}
	}
};