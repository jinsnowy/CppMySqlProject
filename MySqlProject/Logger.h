#pragma once
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "LogLevel.h"

class Logger
{
private:
	static std::unique_ptr<Logger> mInst;

private:
	bool mExitFlag;
	bool mConsoleLog;
	int mFlushDurationMilliSec;

	std::ofstream mOutFile;
	std::string mBufferedString;
	std::stringstream mBuffer;
	std::thread mWorker;
	std::condition_variable mCV;
	std::mutex  mSync;

private:
	Logger()
		:
		mExitFlag(false),
		mConsoleLog(true),
		mFlushDurationMilliSec(100),
		mWorker([this]() { this->Flush(); })
	{
	}

public:
	~Logger()
	{
		using namespace std::literals::chrono_literals;

		while (true)
		{
			{
				std::lock_guard<std::mutex> _(mSync);
				auto left = mBuffer.str();
				if (left.size() == 0)
				{
					break;
				}
			}

			mCV.notify_one();
			std::this_thread::sleep_for(100s);
		}

		mExitFlag = true;
		mCV.notify_one();
		if (mWorker.joinable())
		{
			mWorker.join();
		}
	}

	static void Initialize()
	{
		mInst.reset(new Logger());

		try 
		{
			auto curDate = DateTime::Now();
			auto dateStr = curDate.ToString("%Y_%m_%d");
			std::string fileName = PathManager::GetPath(Format::format("Log\\[%s]_Log.txt", dateStr.c_str()));
			mInst->mOutFile.open(fileName.c_str(), std::ios::app);
		}
		catch (std::exception e)
		{
			std::cout << "Logger Initialize Failed : " << e.what() << std::endl;
		}
	}

	static Logger* Get() { return mInst.get(); }

	static Logger* GetCurrentLogger() { return mInst.get(); }

	void SetConsoleLog(bool bConsoleLog) { mConsoleLog = bConsoleLog; }

	static void Log(ELogLevel level, const char* message)
	{
		mInst->LogFunc(level, message);
	}

	static void Log(ELogLevel level, const std::string& message)
	{
		Log(level, message.c_str());
	}

	template<typename ...Args>
	static void Log(ELogLevel level, const char* format, Args&&... args)
	{
		Log(level, Format::format(format, std::forward<Args>(args)...));
	}

	static void DebugLog(const char* message)
	{
		mInst->LogFunc(ELogLevel::Debug, message);
	}

	static void DebugLog(const std::string& message)
	{
		Log(ELogLevel::Debug, message.c_str());
	}

	template<typename ...Args>
	static void DebugLog(const char* format, Args&&... args)
	{
		Log(ELogLevel::Debug, Format::format(format, std::forward<Args>(args)...));
	}

	static void ErrorLog(const char* message)
	{
		mInst->LogFunc(ELogLevel::Error, message);
	}

	static void ErrorLog(const std::string& message)
	{
		Log(ELogLevel::Error, message.c_str());
	}

	template<typename ...Args>
	static void ErrorLog(const char* format, Args&&... args)
	{
		Log(ELogLevel::Error, Format::format(format, std::forward<Args>(args)...));
	}


	//static void Log(const char* format, const std::string& s)
	//{
	//	Get()->LogFunc(Format::format(format, s.c_str()));
	//}


private:
	//template<typename ...FArgs, typename Arg, typename ...BArgs>
	//static void LogSplit(const char* const& format, FArgs&&... fArgs, Arg&& arg, BArgs&&... bArgs)
	//{
	//	if constexpr (is_first_of<std::string, BArgs>::value)
	//	{
	//		LogSplit(format, std::forward<Args>(args)...);
	//	}
	//	else
	//	{
	//		Get()->LogFunc(Format::format(format, std::forward<Args>(args)...));
	//	}
	//}

	//template<typename ...FArgs, typename Arg, typename ...BArgs>
	//static void LogSplit(const char* const& format, FArgs&&... fArgs, std::string&& arg, BArgs&&... bArgs)
	//{
	//	if constexpr (is_one_of<std::string, Args>::value)
	//	{
	//		LogSplit(format, std::forward<Args>(args)...);
	//	}
	//	else
	//	{
	//		Get()->LogFunc(Format::format(format, std::forward<Args>(args)...));
	//	}
	//}

	//template<typename ...FArgs, typename Arg>
	//static void LogSplit(const char* const& format, FArgs&&... fArgs, Arg&& arg)
	//{
	//	if constexpr (is_one_of<std::string, Args>::value)
	//	{
	//		LogSplit(format, std::forward<Args>(args)...);
	//	}
	//	else
	//	{
	//		Get()->LogFunc(Format::format(format, std::forward<Args>(args)...));
	//	}
	//}

	//template<typename ...Args>
	//static void LogComplete(const char* const& format, Args&&... args)
	//{
	//	Get()->LogFunc(Format::format(format, std::forward<Args>(args)...));
	//}

private:
	void LogFunc(ELogLevel level, const char* message)
	{
		auto now = DateTime::Now().ToString();
		auto logMessage = Format::format("[%s] [%s] : %s", now.c_str(), SLogLevel::GetTag(level), message);
		{
			std::lock_guard<std::mutex> _(mSync);
			mBuffer << logMessage << "\n";
		}
		mCV.notify_one();
	}

	void Write()
	{
		if (mInst->mOutFile.is_open())
		{
			mInst->mOutFile << mBufferedString;
		}

		if (mConsoleLog)
		{
			std::cout << mBufferedString;
		}

		mBufferedString.clear();
	}

	void Flush()
	{
		using namespace std::literals::chrono_literals;

		while (!mExitFlag)
		{
			{
				std::unique_lock<std::mutex> lk(mSync);
				
				mCV.wait(lk, [this]()
				{
					mBufferedString = mBuffer.str();
					return mExitFlag || mBufferedString.size() > 0;
				});

				if (mBufferedString.size() == 0)
				{
					mBufferedString = mBuffer.str();
				}

				mBuffer.str("");
			}

			if (mBufferedString.size() > 0)
			{
				Write();
			}

			if (mExitFlag)
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(mFlushDurationMilliSec));
		}

		if (mInst->mOutFile.is_open())
		{
			mInst->mOutFile.close();
		}
	}
};

#endif