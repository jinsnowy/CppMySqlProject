#include "stdafx.h"
#include "DatabaseManager.h"
#include "DbConnection.h"
#include "SqlDef/Xtable.h"
#include "Database.h"
#include "SqlCmd/XStatement.h"

std::unique_ptr<DatabaseManager> DatabaseManager::mInst = nullptr;

DatabaseManager::~DatabaseManager()
{
}

void DatabaseManager::Release()
{
	mInst = nullptr;
}

bool DatabaseManager::Initialize()
{
	try
	{
		mInst.reset(new DatabaseManager());
	}
	catch (sql::SQLException e)
	{
		Logger::ErrorLog("Cannot Initialize Database Manger, Error Message : %s, Error Code : %d, Error State : %s", e.what(), e.getErrorCode(), e.getSQLStateCStr());
		return false;
	}

	return true;
}

void DatabaseManager::SetDefaultConnection(DbConnection* connection)
{
	mDefaultConnection = connection;
}

Database* DatabaseManager::RegisterDatabase(const char* name)
{
	std::string key = name;
	if (mDatabaseMap.find(key) != mDatabaseMap.end())
	{
		return mDatabaseMap[key].get();
	}

	mDatabaseMap.emplace(key, Database::Initialize(name));

	return mDatabaseMap[key].get();
}

Database* DatabaseManager::FindDatabase(const char* name)
{
	std::string key = name;
	if (mDatabaseMap.find(key) != mDatabaseMap.end())
	{
		return mDatabaseMap[key].get();
	}

	return nullptr;
}

void DatabaseManager::UseDatabase(Database* db)
{
	auto statement = mDefaultConnection->CreateStatement();
	statement->Execute(db->GetUseString());
}

DbConnection* DatabaseManager::GetConnection(const char* connectionName)
{
	auto found = mConnMap.find(connectionName);
	if (found == mConnMap.end())
		return nullptr;

	return found->second.get();
}

DbConnection* DatabaseManager::CreateConnection(const char* connectionName, const std::string& server, const std::string& username, const std::string& password)
{
	DbConnection* conn = nullptr;
	try
	{
		sql::Connection* rawConn = Get()->mDriver->connect(server, username, password);
		conn = new DbConnection(server, username, password, rawConn);

		Get()->mConnMap.emplace(connectionName, conn);
	}
	catch (sql::SQLException e)
	{
		Logger::ErrorLog("Could not connect to server, Error Message : %s, Error Code : %d, Error State : %s", e.what(), e.getErrorCode(), e.getSQLStateCStr());
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Could not connect to server, Error Message : %s", e.what());
	}

	return conn;
}
