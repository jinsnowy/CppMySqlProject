#pragma once

class DbConnection;
class DatabaseManager
{
private:
	sql::Driver* mDriver;
	std::unordered_map<std::string, std::unique_ptr<DbConnection>> mConnMap;
	static std::unique_ptr<DatabaseManager> mInst;

public:
	DatabaseManager()
		:
		mDriver(get_driver_instance())
	{
	}

	static void Release()
	{
		mInst = nullptr;
	}

	static DatabaseManager* Get()
	{
		return mInst.get();
	}

	static bool Initialize();

	DbConnection* GetConnection(const char* connectionName);

	static DbConnection* CreateConnection(const char* connectionName, const std::string& server, const std::string& username, const std::string& password);
};