#include "stdafx.h"
#include "ProjectScenario.h"

#include "Config/Config.h"
#include "Manager/Database.h"
#include "Manager/SPManager.h"
#include "Manager/DbConnection.h"

#include "SqlCmd/QueryCommon.h"
#include "SqlDef/XTable.h"
#include "StoredProcedure.h"
#include "ProjectCommon.h"
#include "DBWorker.h"

using namespace std;

std::unique_ptr<ProjectScenario> ProjectScenario::mInst = nullptr;

void ProjectScenario::Initialize()
{
	mInst.reset(new ProjectScenario());
}

void ProjectScenario::CreateTables()
{
	using namespace sqldef;

	auto db = DatabaseManager::Get()->FindDatabase("StockDb");

	auto userTbl = Table("UserTbl");
	userTbl->Append(Column<EDataType::BIGINT>("userId", Const::NotNull, Const::Auto, Const::Pk));
	userTbl->Append(Column<EDataType::VARCHAR, 24>("userName", Const::NotNull, Const::Unique));
	userTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
	if (db->CreateTable(std::move(userTbl)) == nullptr)
		return;

	auto accountTbl = Table("AccountTbl");
	accountTbl->Append(Column<EDataType::BIGINT>("accountId", Const::NotNull, Const::Auto, Const::Pk));
	accountTbl->Append(Column<EDataType::BIGINT>("userId", Const::NotNull));
	accountTbl->Append(Column<EDataType::BIGINT>("cashAmount", Const::NotNull, Const::Zero));
	accountTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
	accountTbl->Append(Column<EDataType::DATETIME>("updated", Const::NotNull));
	accountTbl->PushFk("userId", "userTbl", "userId");
	if (db->CreateTable(std::move(accountTbl)) == nullptr)
		return;

	auto accountHistoryTbl = Table("AccountHistoryTbl");
	accountHistoryTbl->Append(Column<EDataType::BIGINT>("historyId", Const::NotNull, Const::Auto, Const::Pk));
	accountHistoryTbl->Append(Column<EDataType::BIGINT>("accountId", Const::NotNull));
	accountHistoryTbl->Append(Column<EDataType::SMALLINT>("infoType", Const::NotNull));
	accountHistoryTbl->Append(Column<EDataType::BIGINT>("infoId", Const::NotNull, Const::MinusOne));
	accountHistoryTbl->Append(Column<EDataType::BIGINT>("cashAmount", Const::NotNull));
	accountHistoryTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
	if (db->CreateTable(std::move(accountHistoryTbl)) == nullptr)
		return;

	auto sendInfoTbl = Table("SendInfoTbl");
	sendInfoTbl->Append(Column<EDataType::BIGINT>("infoId", Const::NotNull, Const::Auto, Const::Pk));
	sendInfoTbl->Append(Column<EDataType::BIGINT>("srcAccountId", Const::NotNull));
	sendInfoTbl->Append(Column<EDataType::BIGINT>("dstAccountId", Const::NotNull));
	sendInfoTbl->Append(Column<EDataType::BIGINT>("amount", Const::NotNull));
	if (db->CreateTable(std::move(sendInfoTbl)) == nullptr)
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

void ProjectScenario::InitializeDatas()
{
	auto db = DatabaseManager::Get()->FindDatabase("StockDb");

	auto allTables = db->GetAllTables();
	for (auto& table : allTables)
	{
		db->ClearTable(table->GetName());
	}

	Logger::DebugLog("Initialize Datas");
}

void ProjectScenario::CreateUsers(int nUsers)
{
	std::string filePath = PathManager::GetPath("name.txt");
	std::ifstream file(filePath, std::ios::in);
	if (!file.is_open())
		return;

	std::string name;
	std::vector<std::string> userNames;
	while (std::getline(file, name))
	{
		if (userNames.size() >= nUsers)
			break;

		userNames.push_back(name);
	}

	Logger::DebugLog("Total %zd Names Load", userNames.size());

	UserFactory::Get()->ClearAll();
	auto statement = DatabaseManager::Get()->GetDefaultConnection()->CreateStatement();
	for (const auto& name : userNames)
	{
		auto dateTimeStr = Random::GetRandomDateTime().ToSqlString();
		auto valueString = Format::format("('%s', '%s')", name.c_str(), dateTimeStr.c_str());
		statement->Execute("INSERT INTO UserTbl(userName, created) VALUES" + valueString + ";");
		auto query = statement->ExecuteQuery("SELECT LAST_INSERT_ID() as id;");
		auto queryResult = query->GetResult();
		if (queryResult->next())
		{
			auto userId = queryResult->getInt64("id");
			UserFactory::Get()->CreateUser(userId, name);
		}
	}

	file.close();
}

void ProjectScenario::CreateAccountDatas()
{
	auto statement = DatabaseManager::Get()->GetDefaultConnection()->CreateStatement();

	auto users = UserFactory::Get()->GetUsers();
	std::random_shuffle(users.begin(), users.end());

	for (auto& user : users)
	{
		auto now = DateTime::Now();
		auto dateTimeStr = now.ToSqlString();
		auto valueStr = Format::format("(%ld, '%s', '%s', %ld)", user->GetUserId(), dateTimeStr.c_str(), dateTimeStr.c_str(), 0);
		if (statement->Execute("INSERT INTO AccountTbl(userId, created, updated, cashAmount) VALUES" + valueStr + ";"))
		{
			auto query = statement->ExecuteQuery("SELECT LAST_INSERT_ID() as id;");
			auto queryResult = query->GetResult();
			if (queryResult->next())
			{
				auto accountId = queryResult->getInt64("id");
				auto accountInfo = AccountInfo::Create(user.get(), accountId, 0, now, now);
				user->AddAcount(accountInfo);
			}
		}
	}
}

void ProjectScenario::QueryDatas()
{
	auto conn = DatabaseManager::Get()->GetDefaultConnection();
	auto statement = conn->CreateStatement();
	auto query = statement->ExecuteQuery("SELECT * FROM UserTbl U JOIN AccountTbl A ON U.userId = A.userId");
	auto queryResult = query->GetResult();
	while (queryResult->next())
	{
		auto userId = queryResult->getInt64("userId");
		auto userName = queryResult->getString("userName");
		auto userInfo = UserFactory::Get()->GetUser(userId);
		if (userInfo == nullptr)
		{
			userInfo = UserFactory::Get()->CreateUser(userId, userName);
		}

		auto accountId = queryResult->getInt64("accountId");
		if (userInfo->GetAccount() == nullptr)
		{
			auto cashAmount = queryResult->getInt64("cashAmount");
			auto created = DateTime::FromString(queryResult->getString("created"));
			auto updated = DateTime::FromString(queryResult->getString("updated"));
			auto accountInfo = AccountInfo::Create(userInfo.get(), accountId, cashAmount, created, updated);
			userInfo->AddAcount(accountInfo);
		}
	}
}


void ProjectScenario::AddSomeCash()
{
	auto users = UserFactory::Get()->GetUsers();
	Random casher = Random::GetRandom(0, 10000000);

	long long total = 0;
	for (auto& user : users)
	{
		auto cashAmount = (long long)casher.Next();
		auto account = user->GetAccount();
		auto afterAmount = account->GetCash() + cashAmount;

		AddCashSP sp(account->GetKey(), cashAmount, afterAmount);
		if (sp.Execute())
		{
			account->Update(afterAmount, sp.updated);
			total += sp.GetElapsedMilliSec();
		}
	}

	Logger::DebugLog("AddCash Executes %lld ms", total);
}

void ProjectScenario::SendSomeCash()
{
	auto users = UserFactory::Get()->GetUsers();

	Random userSelector = Random::GetRandom(0, (int)(users.size() - 1));

	long long total = 0;
	for (int i = 0; i < 10000; ++i)
	{
		auto pair = userSelector.GetRandomUniqueSet(2);

		const auto& srcAccount = users[pair[0]]->GetAccount();
		const auto& dstAccount = users[pair[1]]->GetAccount();

		const auto& amount = srcAccount->GetCash() / 2;
		if (amount == 0)
		{
			continue;
		}

		long long afterSrcAmount = srcAccount->GetCash() - amount;
		long long afterDstAmount = dstAccount->GetCash() + amount;
		SendCashSP sp(srcAccount->GetKey(), dstAccount->GetKey(), amount, afterSrcAmount, afterDstAmount);
		if (sp.Execute())
		{
			srcAccount->Update(afterSrcAmount, sp.updated);
			dstAccount->Update(afterDstAmount, sp.updated);
			total += sp.GetElapsedMilliSec();
		}
	}

	Logger::DebugLog("SendCash Executes %lld ms", total);
}

void ProjectScenario::ConcurrentSendCashSingleWorker()
{
	auto users = UserFactory::Get()->GetUsers();
	int numOfConcurrency = (int)std::thread::hardware_concurrency();

	Random userSelector = Random::GetRandom(0, (int)(users.size() - 1));
	Random sendAmount = Random::GetRandom(100, 1000);

	long long befSumAmount = 0;
	for (auto& user : users)
	{
		befSumAmount += user->GetCash();
	}

	Logger::DebugLog("Before Send Cash : %lld", befSumAmount);

	Database* stockDb = DatabaseManager::Get()->FindDatabase("StockDb");
	vector<DbConnection*> conns;
	for (int i = 0; i < numOfConcurrency; ++i)
	{
		auto connName = Format::format("connect[%d]", i);
		DbConnection* conn = DatabaseManager::Get()->CreateConnection(connName.c_str(), Config::GetHostname(), Config::GetUsername(), Config::GetPassword());
		conn->UseDatabase(stockDb);
		conns.push_back(conn);
	}

	DbWorker dbWorker;
	volatile long long added = 0;
	volatile long long elapsed = 0;
	volatile long long success_count = 0;
	long long num_items_per_thread = 100;
	
	const auto do_work = [&](int id)
	{
		auto conn = conns[id];
		conn->SetAutoCommit(true);
		conn->SetIsolationLevel(sql::enum_transaction_isolation::TRANSACTION_READ_COMMITTED);

		for (int i = 0; i < (int)num_items_per_thread; ++i)
		{
			auto pair = userSelector.GetRandomUniqueSet(2);

			const auto& srcAccount = users[pair[0]]->GetAccount();
			const auto& dstAccount = users[pair[1]]->GetAccount();

			auto select_amount = (long long)sendAmount.Next();

			long long beforeSrcAmount = srcAccount->GetCash();
			if (beforeSrcAmount < select_amount)
			{
				select_amount = beforeSrcAmount;
			}

			if (select_amount == 0)
			{
				dbWorker.Queue(BaseSP::MakeSP<AddCashSP>(srcAccount->GetKey(), 1000, 1000), conn, [srcAccount, &added, &elapsed, &success_count](auto sp)
				{
					auto spc = (AddCashSP*)(sp.get());
					srcAccount->Update(0, 1000, spc->updated);
					_interlockedexchangeadd64(&added, 1000);
					_interlockedexchangeadd64(&elapsed, spc->GetElapsedMilliSec());
					_interlockedincrement64(&success_count);
				});

				dbWorker.Wait();
				continue;
			}

			long long beforeDstAmount = dstAccount->GetCash();
			long long afterSrcAmount = beforeSrcAmount - select_amount;
			long long afterDstAmount = beforeDstAmount + select_amount;

			dbWorker.Queue(BaseSP::MakeSP<SendCashSP>(srcAccount->GetKey(), dstAccount->GetKey(), select_amount, afterSrcAmount, afterDstAmount), conn, 
				[srcAccount, dstAccount, beforeSrcAmount, beforeDstAmount, &added, &elapsed, &success_count](auto sp)
			{
				auto spc = (SendCashSP*)(sp.get());
				srcAccount->Update(beforeSrcAmount, spc->afterSrcCashAmount, spc->updated);
				dstAccount->Update(beforeDstAmount, spc->afterDstCashAmount, spc->updated);
				_interlockedexchangeadd64(&elapsed, spc->GetElapsedMilliSec());
				_interlockedincrement64(&success_count);
			});
		}
	};

	vector<thread> workers;
	for (int i = 0; i < numOfConcurrency; ++i)
	{
		workers.emplace_back(do_work, i);
	}

	for (int i = 0; i < numOfConcurrency; ++i)
	{
		workers[i].join();
	}

	dbWorker.Wait();

	long long afterSumAmount = 0;
	for (auto& user : users)
	{
		afterSumAmount += user->GetCash();
	}

	auto conn = DatabaseManager::Get()->GetDefaultConnection();
	conn->UseDatabase(stockDb);
	long long afterSumAmountOnSelect = 0;
	auto statement = conn->CreateStatement();
	auto query = statement->ExecuteQuery("SELECT accountId, cashAmount FROM AccountTbl;");
	auto queryResult = query->GetResult();
	while (queryResult->next())
	{
		afterSumAmountOnSelect += queryResult->getInt64("cashAmount");
	}

	Logger::DebugLog("After ##Send Cash## cache : %lld, logic add : %lld, db select : %lld", afterSumAmount, added, afterSumAmountOnSelect);
	Logger::DebugLog("Elapsed : %.3fms per item", elapsed / float(num_items_per_thread * numOfConcurrency));
	Logger::DebugLog("Success rate : %.3f", float(success_count) / (float)(num_items_per_thread * numOfConcurrency));
}

static void TransactionByPrepareStatement()
{
	auto users = UserFactory::Get()->GetUsers();

	Random userSelector = Random::GetRandom(0, (int)(users.size() - 1));

	long long total = 0;
	for (int i = 0; i < 10000; ++i)
	{
		auto pair = userSelector.GetRandomUniqueSet(2);

		const auto& srcAccount = users[pair[0]];
		const auto& dstAccount = users[pair[1]];

		const auto& amount = srcAccount->GetCash() / 2;
		if (amount == 0)
		{
			continue;
		}

		auto statement = DatabaseManager::Get()->GetDefaultConnection()->CreatePrepareStatement("CALL sendTransaction(?,?,?, @result)");
		statement->SetParam(1, srcAccount->GetUserId());
		statement->SetParam(2, dstAccount->GetUserId());
		statement->SetParam(3, amount);
		statement->Execute();
		total += statement->GetElapsedMilliSec();
	}

	Logger::DebugLog("By PrepareStatement Executes %lld ms", total);
}