#include "stdafx.h"
#include "Manager/DatabaseManager.h"
#include "Manager/SPManager.h"
#include "Manager/DbConnection.h"
#include "SqlCmd/QueryCommon.h"
#include "Config/Config.h"
#include "SqlDef/XTable.h"

struct UserInfo
{
	long long userId;
	std::string userName;
	long long cash;

	UserInfo(long long _userId, const std::string& _userName)
		: userId(_userId), userName(_userName), cash(0)
	{}

	UserInfo(long long _userId, const std::string& _userName, long long _cash)
		: userId(_userId), userName(_userName), cash(_cash)
	{}

	void AddCash(long long amount)
	{
		cash += amount;
	}
};

DbConnection* g_Conn;
std::vector<UserInfo> g_Users;

const char* storedProcedureFileName = "StoredProcedure.sql";
const char* configFileName = "MyConfig.xml";

static void TestConnection()
{
	auto statement = g_Conn->CreateStatement();
	statement->Execute("USE StockDb;");
}

static void CreateTables()
{
	using namespace sqldef;

	auto db = DatabaseManager::Get()->RegisterDatabase("StockDb");

	auto userTbl = Table("UserTbl");
	userTbl->Append(Column<EDataType::BIGINT>("userId", Const::NotNull, Const::Auto, Const::Pk));
	userTbl->Append(Column<EDataType::VARCHAR, 24>("userName", Const::NotNull, Const::Unique));
	userTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
	if (!db->CreateTable(userTbl.get()))
		return;

	auto accountTbl = Table("AccountTbl");
	accountTbl->Append(Column<EDataType::BIGINT>("accountId", Const::NotNull, Const::Auto, Const::Pk));
	accountTbl->Append(Column<EDataType::BIGINT>("userId", Const::NotNull));
	accountTbl->Append(Column<EDataType::BIGINT>("cashAmount", Const::NotNull, Const::Zero));
	accountTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
	accountTbl->Append(Column<EDataType::DATETIME>("updated", Const::NotNull));
	accountTbl->PushFk("userId", "userTbl", "userId");
	if (!db->CreateTable(accountTbl.get()))
		return;

	auto transactionTbl = Table("TransactionTbl");
	transactionTbl->Append(Column<EDataType::BIGINT>("transactionId", Const::Const::NotNull, Const::Auto, Const::Pk));
	transactionTbl->Append(Column<EDataType::BIGINT>("cashAmount", Const::NotNull));
	transactionTbl->Append(Column<EDataType::BIGINT>("srcAccountId", Const::NotNull));
	transactionTbl->Append(Column<EDataType::BIGINT>("dstAccountId", Const::NotNull));
	transactionTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
	if (!db->CreateTable(transactionTbl.get()))
		return;

	// auto statement = g_Conn->CreateStatement();
	//statement->Execute("CREATE DATABASE IF NOT EXISTS StockDb");
	//statement->Execute("SET foreign_key_checks=0;");

	//statement->Execute("DROP TABLE IF EXISTS UserTbl;");
	//statement->Execute("CREATE TABLE `StockDb`.`UserTbl` ("
	//	"`userId` BIGINT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
	//	"`userName` VARCHAR(24) NOT NULL UNIQUE,"
	//	"`created` DATETIME NOT NULL"
	//	");");


	//statement->Execute("DROP TABLE IF EXISTS AccountTbl;");
	//statement->Execute("CREATE TABLE `StockDb`.`AccountTbl` ("
	//	"`accountId` BIGINT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
	//	"`userId` BIGINT NOT NULL UNIQUE,"
	//	"`cashAmount` BIGINT NOT NULL DEFAULT 0,"
	//	"`created` DATETIME NOT NULL,"
	//	"`updated` DATETIME NOT NULL,"
	//	"CONSTRAINT fk_accounttbl_usertbl FOREIGN KEY(userId) REFERENCES userTbl(userId)"
	//	");");

	//statement->Execute("DROP TABLE IF EXISTS TransactionTbl;");
	//statement->Execute("CREATE TABLE `StockDb`.`TransactionTbl` ("
	//	"`transactionId` BIGINT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
	//	"`cashAmount` BIGINT NOT NULL,"
	//	"`srcAccountId` BIGINT NOT NULL,"
	//	"`dstAccountId` BIGINT NOT NULL,"
	//	"`created` DATETIME NOT NULL"
	//	");");

	//statement->Execute("SET foreign_key_checks=1;");
	Logger::DebugLog("Create Tables");
}

static void InitializeDatas()
{
	auto statement = g_Conn->CreateStatement();
	statement->Execute("CREATE DATABASE IF NOT EXISTS StockDb");
	statement->Execute("SET foreign_key_checks=0;");
	statement->Execute("USE StockDb;");
	statement->Execute("DELETE FROM UserTbl;");
	statement->Execute("DELETE FROM AccountTbl;");
	statement->Execute("DELETE FROM TransactionTbl;");

	statement->Execute("SET foreign_key_checks=1;");
	Logger::DebugLog("Initialize Datas");
}

static void CreateUserDatas()
{
	std::string filePath = PathManager::GetPath("name.txt");
	std::ifstream file(filePath, std::ios::in);
	if (!file.is_open())
		return;

	std::string name;
	std::vector<std::string> userNames;
	while (std::getline(file, name))
	{
		if (userNames.size() > 100)
			break;

		userNames.push_back(name);
	}

	Logger::DebugLog("Total %zd Names Load", userNames.size());

	g_Users.clear();
	auto statement = g_Conn->CreateStatement();
	for (auto& name : userNames)
	{
		auto dateTimeStr = Random::GetRandomDateTime().ToSqlString();
		auto valueString = Format::format("('%s', '%s')", name.c_str(), dateTimeStr.c_str());
		statement->Execute("INSERT INTO UserTbl(userName, created) VALUES" + valueString + ";");
		auto query = statement->ExecuteQuery("SELECT LAST_INSERT_ID() as id;");
		auto queryResult = query->GetResult();
		if (queryResult->next())
		{
			auto userId = queryResult->getInt64("id");
			g_Users.emplace_back(userId, name);
		}
	}

	file.close();
}

static void QueryUserDatas()
{
	auto statement = g_Conn->CreateStatement();
	auto query = statement->ExecuteQuery("SELECT * FROM UserTbl;");
	auto queryResult = query->GetResult();

	g_Users.clear();
	while (queryResult->next())
	{
		auto userId = queryResult->getInt64("userId");
		auto userName = queryResult->getString("userName");

		auto queryStr = Format::format("SELECT cashAmount FROM AccountTbl WHERE userId = %ld;", userId);
		auto queryCash = statement->ExecuteQuery(queryStr.c_str());
		auto queryCashResult = queryCash->GetResult();
		if (queryCashResult->next())
		{
			auto cashAmount = queryCashResult->getInt64(1);
			g_Users.emplace_back(userId, userName, cashAmount);
		}
	}
}

static void CreateAccountDatas()
{
	auto statment = g_Conn->CreateStatement();
	
	Random cashRandom = Random::GetRandom<100, 10000>();
	std::random_shuffle(g_Users.begin(), g_Users.end());
	for (auto& user : g_Users)
	{
		auto cashInit = cashRandom.Next();
		auto dateTimeStr = DateTime::Now().ToSqlString();
		auto valueStr = Format::format("(%ld, '%s', '%s', %ld)", user.userId, dateTimeStr.c_str(), dateTimeStr.c_str(), cashInit);
		statment->Execute("INSERT INTO AccountTbl(userId, created, updated, cashAmount) VALUES" + valueStr + ";");

		user.AddCash(cashInit);
	}
}

static void TransactionByPrepareStatement()
{
	Random userSelector = Random::GetRandom(0, (int)(g_Users.size() - 1 ));

	long long total = 0;
	for (int i = 0; i < 10000; ++i)
	{
		auto pair = userSelector.GetRandomUniqueSet(2);

		const auto& srcAccount = g_Users[pair[0]];
		const auto& dstAccount = g_Users[pair[1]];

		const auto& amount = srcAccount.cash / 2;
		if (amount == 0)
		{
			continue;
		}

		auto statement = g_Conn->CreatePrepareStatement("CALL sendTransaction(?,?,?, @result)");
		statement->SetParam(1, srcAccount.userId);
		statement->SetParam(2, dstAccount.userId);
		statement->SetParam(3, amount);
		statement->Execute();
		total += statement->GetElapsedMilliSec();
	}

	Logger::DebugLog("By PrepareStatement Executes %lld ms", total);
}

static void TransactionByStoredProcedure()
{
	Random userSelector = Random::GetRandom(0, (int)(g_Users.size() - 1));

	auto sp = SPManager::GetProcedure("sendTransaction");
	long long total = 0;
	for (int i = 0; i < 10000; ++i)
	{
		auto pair = userSelector.GetRandomUniqueSet(2);

		const auto& srcAccount = g_Users[pair[0]];
		const auto& dstAccount = g_Users[pair[1]];

		const auto& amount = srcAccount.cash / 2;
		if (amount == 0)
		{
			continue;
		}

		sp->Bind(srcAccount.userId, dstAccount.userId, amount);
		sp->Execute();
		total += sp->GetElapsedMilliSec();
	}

	Logger::DebugLog("By StoredProcedure Executes %lld ms", total);
}

int main(int argc, char** argv)
{
	DatabaseManager::Initialize();
	PathManager::Initialize();
	Logger::Initialize();
	SPManager::Initialize(storedProcedureFileName);
	Config::Initialize(configFileName);

	g_Conn = DatabaseManager::Get()->CreateConnection("MyConnection", Config::GetHostname(), Config::GetUsername(), Config::GetPassword());
	g_Conn->SetAutoCommit(true);
	g_Conn->SetIsolationLevel(sql::enum_transaction_isolation::TRANSACTION_REPEATABLE_READ);
	DatabaseManager::Get()->SetDefaultConnection(g_Conn);

	try 
	{
		TestConnection();
		CreateTables();
		SPManager::Install(g_Conn->CreateStatement());

		CreateUserDatas();
		CreateAccountDatas();

		QueryUserDatas();
		TransactionByPrepareStatement();
		InitializeDatas();
		TransactionByStoredProcedure();
	}
	catch (sql::SQLException e)
	{
		Logger::ErrorLog("Sql Exception, Error Message : %s, Error Code : %d, Error State : %s", e.what(), e.getErrorCode(), e.getSQLStateCStr());
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Exception , Error Message : %s", e.what());
	}

	DatabaseManager::Release();

	return 0;
}


