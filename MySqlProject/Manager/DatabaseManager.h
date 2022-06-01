#pragma once

class Database;
class DbConnection;
class DatabaseManager
{
private:
	sql::Driver* mDriver;
	DbConnection* mDefaultConnection;

	std::unordered_map<std::string, std::unique_ptr<DbConnection>> mConnMap;
	std::unordered_map<std::string, std::unique_ptr<Database>> mDatabaseMap;

	static std::unique_ptr<DatabaseManager> mInst;

public:
	DatabaseManager();

	~DatabaseManager();

	static void Release();

	static DatabaseManager* Get();

	static bool Initialize();

	static DbConnection* CreateConnection(const char* connectionName, const std::string& server, const std::string& username, const std::string& password);

	void SetDefaultConnection(DbConnection* connection);

	DbConnection* GetDefaultConnection() const;

	// Assume database already exists
	Database* RegisterDatabase(const char* name);

	Database* FindDatabase(const char* name);

	DbConnection* GetConnection(const char* connectionName);
};