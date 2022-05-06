#pragma once

template<typename Derived>
class SqlCmd : public std::enable_shared_from_this<Derived>
{
private:
	long long mElapsedMilliSec = 0;

public:
	bool Execute(const std::string& exeCmd)
	{
		try 
		{
			auto stopWatch = Stopwatch::StartNow();
			
			OnExecute(exeCmd);
			
			stopWatch.Stop();

			mElapsedMilliSec = stopWatch.ElapsedMilliSec();
		}
		catch (sql::SQLException e)
		{
			Logger::ErrorLog("Execution Failed : %s, Error Message : %s, Error Code : %d, Error State : %s", Where(), e.what(), e.getErrorCode(), e.getSQLStateCStr());
			
			OnError();
			
			return false;
		}

		return true;
	}

	virtual ~SqlCmd()
	{
	}

	long GetElapsedMilliSec() const
	{
		return mElapsedMilliSec;
	}

protected:
	virtual void OnExecute(const std::string& exeCmd) = 0;
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
		try
		{
			auto stopWatch = Stopwatch::StartNow();

			OnExecute();

			stopWatch.Stop();

			mElapsedMilliSec = stopWatch.ElapsedMilliSec();
		}
		catch (sql::SQLException e)
		{
			Logger::ErrorLog("Execution Failed : %s, Error Message : %s, Error Code : %d, Error State : %s", Where(), e.what(), e.getErrorCode(), e.getSQLStateCStr());
			
			OnError();

			return false;
		}

		return true;
	}

	virtual ~SqlNoCmd()
	{
	}

	long long GetElapsedMilliSec() const
	{
		return mElapsedMilliSec;
	}

protected:
	virtual void OnExecute() = 0;
	virtual void OnError() {};
	virtual const char* Where() = 0;
};
