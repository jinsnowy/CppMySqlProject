#include "stdafx.h"
#include "DatabaseManager.h"
#include "DbConnection.h"

std::unique_ptr<DatabaseManager> DatabaseManager::mInst = nullptr;

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
