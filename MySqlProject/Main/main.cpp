#include "stdafx.h"
#include "Manager/DatabaseManager.h"
#include "Manager/SPManager.h"
#include "Manager/DbConnection.h"
#include "SqlCmd/QueryCommon.h"
#include "Config/Config.h"
#include "SqlDef/XTable.h"

#include "ProjectCommon.h"
#include "ProjectScenario.h"

DbConnection* g_Conn = nullptr;
const char* storedProcedureFileName = "StoredProcedure.sql";
const char* configFileName = "MyConfig.xml";

int main(int argc, char** argv)
{
	DatabaseManager::Initialize();
	PathManager::Initialize();
	Logger::Initialize();
	SPManager::Initialize(storedProcedureFileName);
	Config::Initialize(configFileName);

	g_Conn = DatabaseManager::Get()->CreateConnection("Default", Config::GetHostname(), Config::GetUsername(), Config::GetPassword());
	g_Conn->SetAutoCommit(true);
	g_Conn->SetIsolationLevel(sql::enum_transaction_isolation::TRANSACTION_REPEATABLE_READ);
	DatabaseManager::Get()->SetDefaultConnection(g_Conn);

	ProjectScenario::Initialize();
	UserFactory::Initialize();

	auto db = DatabaseManager::Get()->RegisterDatabase("StockDb");
	DatabaseManager::Get()->UseDatabase(db);
	SPManager::Install();

	try 
	{
		ProjectScenario::CreateTables();
		ProjectScenario::CreateUsers();
		ProjectScenario::CreateAccountDatas();
		
		ProjectScenario::AddSomeCash();

		ProjectScenario::QueryDatas();
		ProjectScenario::SendSomeCash();
	}
	catch (sql::SQLException e)
	{
		Logger::ErrorLog("Sql Exception, Error Message : %s, Error Code : %d, Error State : %s", e.what(), e.getErrorCode(), e.getSQLStateCStr());
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Exception, Error Message : %s", e.what());
	}

	SPManager::Release();
	DatabaseManager::Release();

	return 0;
}


