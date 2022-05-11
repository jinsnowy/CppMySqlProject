#include "stdafx.h"
#include "Database.h"
#include "DatabaseManager.h"
#include "DbConnection.h"
#include "SqlCmd/XStatement.h"
#include "SqlDef/XTable.h"

Database::Database(const std::string& name)
	:
	mName(name),
	mUseString(Format::format("USE %s;", name.c_str()))
{}

std::unique_ptr<Database> Database::Initialize(const std::string & name)
{
	return std::make_unique<Database>(name);
}

bool Database::CreateTable(sqldef::XTable* table, bool bDropIfExists)
{
	using namespace sqldef;

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

	if (table == nullptr)
	{
		Logger::ErrorLog("Create Null Table");
		return false;
	}

	auto connection = DatabaseManager::Get()->GetDefaultConnection();
	if (connection == nullptr)
	{
		Logger::ErrorLog("Null Default Connection");
		return false;
	}

	auto statement = connection->CreateStatement();

	if (bDropIfExists)
	{
		fk_check _(statement);
		auto dropStatement = table->Drop(this);
		if (!statement->Execute(dropStatement))
			return false;
	}

	auto createStatement = table->Create(this);
	if (!statement->Execute(createStatement))
		return false;

	return true;
}
