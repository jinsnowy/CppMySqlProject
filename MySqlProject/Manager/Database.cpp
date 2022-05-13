#include "stdafx.h"
#include "Database.h"
#include "DatabaseManager.h"
#include "DbConnection.h"
#include "SqlCmd/XStatement.h"
#include "SqlDef/XTable.h"

struct fk_check
{
	std::shared_ptr<XStatement> statment;
	fk_check(const std::shared_ptr<XStatement>& _statment)
		:
		statment(_statment)
	{
		statment->Execute("SET foreign_key_checks=0;");
	}
	~fk_check()
	{
		statment->Execute("SET foreign_key_checks=1;");
	}
};

Database::Database(const std::string& name)
	:
	mName(name),
	mUseString(Format::format("USE %s;", name.c_str()))
{}

Database::~Database()
{
}

std::unique_ptr<Database> Database::Initialize(const std::string & name)
{
	return std::make_unique<Database>(name);
}

sqldef::XTable* Database::CreateTable(std::unique_ptr<sqldef::XTable> table, bool bDropIfExists)
{
	using namespace sqldef;
	const auto key = table->GetName();
	if (mTables.find(key) != mTables.end())
	{
		Logger::ErrorLog("Table Already Exists");
		return nullptr;
	}

	if (table == nullptr)
	{
		Logger::ErrorLog("Create Null Table");
		return nullptr;
	}

	auto connection = DatabaseManager::Get()->GetDefaultConnection();
	if (connection == nullptr)
	{
		Logger::ErrorLog("Null Default Connection");
		return nullptr;
	}

	if (table->IsValid() == false)
	{
		Logger::ErrorLog("Invalid Table %s", table->GetName().c_str());
		return nullptr;
	}

	auto statement = connection->CreateStatement();
	if (bDropIfExists)
	{
		fk_check _(statement);
		auto dropStatement = table->Drop(this);
		if (!statement->Execute(dropStatement))
			return nullptr;
	}

	auto createStatement = table->Create(this);
	if (!statement->Execute(createStatement))
		return nullptr;

	mTables.emplace(key, std::move(table));

	return mTables[key].get();
}

bool Database::ClearTable(const std::string& tableName)
{
	using namespace sqldef;

	const std::string& key = tableName;
	if (mTables.find(key) == mTables.end())
	{
		Logger::ErrorLog("Not Found Table");
		return false;
	}

	auto table = mTables[key].get();
	auto connection = DatabaseManager::Get()->GetDefaultConnection();
	if (connection == nullptr)
	{
		Logger::ErrorLog("Null Default Connection");
		return false;
	}

	auto statement = connection->CreateStatement();
	fk_check _(statement);
	auto dropStatement = table->Drop(this);

	return statement->Execute(dropStatement);
}

std::vector<sqldef::XTable*> Database::GetAllTables() const
{
	std::vector<sqldef::XTable*> tables;
	for (const auto& pair : mTables)
	{
		tables.push_back(pair.second.get());
	}

	return tables;
}
