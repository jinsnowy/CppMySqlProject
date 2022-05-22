#pragma once

template<typename Derived>
class SqlCmd : public std::enable_shared_from_this<Derived>
{
private:
	long long mElapsedMilliSec = 0;

public:
	bool Execute(const std::string& exeCmd)
	{
		bool result = false;

		try 
		{
			auto stopWatch = Stopwatch::StartNow();
			
			result = OnExecute(exeCmd);
			
			stopWatch.Stop();

			mElapsedMilliSec = stopWatch.ElapsedMilliSec();
		}
		catch (sql::SQLException e)
		{
			Logger::ErrorLog("Execution Failed : %s, Error Message : %s, Error Code : %d, Error State : %s", Where(), e.what(), e.getErrorCode(), e.getSQLStateCStr());
			
			OnError();
		}
		catch (std::exception e)
		{
			Logger::ErrorLog("Execution Failed : %s, Error Message : %s By StdException", Where(), e.what());

			OnError();
		}

		return result;
	}

	virtual ~SqlCmd()
	{
	}

	long long GetElapsedMilliSec() const
	{
		return mElapsedMilliSec;
	}

protected:
	virtual bool OnExecute(const std::string& exeCmd) = 0;
	virtual void OnError() {};
	virtual const char* Where() = 0;
};

template<typename Derived>
class SqlNoCmd : public std::enable_shared_from_this<Derived>
{
private:
	long long mElapsedMilliSec = 0;

public:
	bool Execute()
	{
		bool result = false;

		try
		{
			auto stopWatch = Stopwatch::StartNow();

			result = OnExecute();

			stopWatch.Stop();

			mElapsedMilliSec = stopWatch.ElapsedMilliSec();
		}
		catch (sql::SQLException e)
		{
			Logger::ErrorLog("Execution Failed : %s, Error Message : %s, Error Code : %d, Error State : %s", Where(), e.what(), e.getErrorCode(), e.getSQLStateCStr());
			
			OnError();
		}
		catch (std::exception e)
		{
			Logger::ErrorLog("Execution Failed : %s, Error Message : %s By StdException", Where(), e.what());

			OnError();
		}

		return result;
	}

	virtual ~SqlNoCmd()
	{
	}

	long long GetElapsedMilliSec() const
	{
		return mElapsedMilliSec;
	}

protected:
	virtual bool OnExecute() = 0;
	virtual void OnError() {};
	virtual const char* Where() = 0;
};
