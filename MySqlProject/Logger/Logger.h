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

		mExitFlag = true;
		mCV.notify_one();
		if (mWorker.joinable())
		{
			mWorker.join();
		}

		if (mInst->mOutFile.is_open())
		{
			mInst->mOutFile.close();
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

	template<typename ...T>
	static void Log(ELogLevel level, const char* format, T&&... args)
	{
		Log(level, Format::format(format, std::forward<T>(args)...));
	}

	static void DebugLog(const char* message)
	{
		mInst->LogFunc(ELogLevel::Debug, message);
	}

	static void DebugLog(const std::string& message)
	{
		Log(ELogLevel::Debug, message.c_str());
	}

	template<typename ...T>
	static void DebugLog(const char* format, T&&... args)
	{
		Log(ELogLevel::Debug, Format::format(format, std::forward<T>(args)...));
	}

	static void ErrorLog(const char* message)
	{
		mInst->LogFunc(ELogLevel::Error, message);
	}

	static void ErrorLog(const std::string& message)
	{
		Log(ELogLevel::Error, message.c_str());
	}

	template<typename ...T>
	static void ErrorLog(const char* format, T&&... args)
	{
		Log(ELogLevel::Error, Format::format(format, std::forward<T>(args)...));
	}
private:
	void LogFunc(ELogLevel level, const char* message)
	{
		auto now = DateTime::Now().ToString();
		auto logMessage = Format::format("[%s] [%s] : %s", now.c_str(), SLogLevel::GetTag(level), message);
		{
			std::lock_guard<std::mutex> _(mSync);
			if (mExitFlag == false)
			{
				mBuffer << logMessage << "\n";
			}
		}

		mCV.notify_one();
	}

	void Write()
	{
		if (mOutFile.is_open())
		{
			mOutFile << mBufferedString << std::flush;
		}

		if (mConsoleLog)
		{
			std::cout << mBufferedString << std::flush;
		}

		mBufferedString.clear();
	}

	void Flush()
	{
		using namespace std::literals::chrono_literals;

		while (!mExitFlag)
		{
			std::unique_lock<std::mutex> lk(mSync);

			mCV.wait(lk);

			mBufferedString = mBuffer.str();
			mBuffer.str("");

			lk.unlock();

			if (mBufferedString.size() > 0)
			{
				Write();
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(mFlushDurationMilliSec));
		}
	}
};

#endif