## CppMySqlProject
 
## Overview
### Scenario Based Study C++ MySQL Connector
+ Config for DbConnection
	- MySQL C++ ODBC Connector
	- TinyXml2

+ Database Object : User and Account (User deposits some cash in Account...)
	- Tables and StoredProcedures (SendCash, AddCash)

+ Programmable Table Define 

```cpp
auto userTbl = Table("UserTbl");
userTbl->Append(Column<EDataType::BIGINT>("userId", Const::NotNull, Const::Auto, Const::Pk));
userTbl->Append(Column<EDataType::VARCHAR, 24>("userName", Const::NotNull, Const::Unique));
userTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
userTbl->PushIndex("userName");
if (db->CreateTable(std::move(userTbl)) == nullptr)
	return;
```

+ Statement Wrappers(Query, PrepareStatement, StoreProcedure)
	- statement create from connection 
	- XStatement, XPrepareStatement, XQueryResult, XStoredProcedure 

+ Scenario Test
	- No Select Using By Cache Object (Object In Server Side)
	- DbWorker (Single Thread) For SP

---
__main.cpp__
```cpp
	DatabaseManager::Initialize();
	PathManager::Initialize();
	Logger::Initialize();
	SPManager::Initialize(storedProcedureFileName);
	Config::Initialize(configFileName);

	g_Conn = DatabaseManager::Get()->CreateConnection("Default", Config::GetHostname(), Config::GetUsername(), Config::GetPassword());
	g_Conn->SetAutoCommit(true);
	g_Conn->SetIsolationLevel(sql::enum_transaction_isolation::TRANSACTION_READ_COMMITTED);
	DatabaseManager::Get()->SetDefaultConnection(g_Conn);

	ProjectScenario::Initialize();
	UserFactory::Initialize();

	auto db = DatabaseManager::Get()->RegisterDatabase("StockDb");
	DatabaseManager::Get()->GetDefaultConnection()->UseDatabase(db);
	SPManager::Install();

	try 
	{
		ProjectScenario::CreateTables();
		ProjectScenario::CreateUsers();
		ProjectScenario::CreateAccountDatas();

		//
		// ProjectScenario::AddSomeCash();
		// ProjectScenario::QueryDatas();
		// ProjectScenario::SendSomeCash();

		ProjectScenario::ConcurrentSendCashSingleWorker();
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
```
## Dependencies
### MySQL C++ ODBC Connector
![MySQL Connector](./mysql.png)
- Guide : https://dev.mysql.com/doc/connector-odbc/en/
- Download : https://dev.mysql.com/downloads/connector/odbc/