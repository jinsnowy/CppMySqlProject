#pragma once
#include "SqlCmd.h"

class XQueryResult;
class XStatement : public SqlCmd<XStatement>
{
	friend class XQueryResult;
private:
	std::unique_ptr<sql::Statement> mStatement;

public:
	XStatement(sql::Statement* const &statement)
		:
		mStatement(statement)
	{}

protected:
	virtual bool OnExecute(const std::string& exeCmd) override
	{
		mStatement->execute(exeCmd);

		return true;
	}

	virtual const char* Where() override
	{
		return "Statement";
	}

public:
	std::shared_ptr<XQueryResult> ExecuteQuery(const std::string& statement);

	void ExecuteQueryResult(const sql::SQLString& statement, std::unique_ptr<sql::ResultSet>& resultset);

	void ExecuteRaw(const sql::SQLString& exeCmd)
	{
		mStatement->execute(exeCmd);
	}

	sql::ResultSet* ExecuteRawQuery(const sql::SQLString& query)
	{
		return mStatement->executeQuery(query);
	}
};

