#pragma once
#include "SqlCmd.h"

class XQueryResult;
class XStatement : public SqlCmd<XStatement>
{
	friend class XQueryResult;
private:
	std::shared_ptr<sql::Statement> mStatement;

public:
	XStatement(sql::Statement* const &statement)
		:
		mStatement(statement)
	{}

	virtual void OnExecute(const std::string& exeCmd) override
	{
		mStatement->execute(exeCmd);
	}

	virtual const char* Where() override
	{
		return "Statement";
	}

	std::shared_ptr<XQueryResult> ExecuteQuery(const std::string& statement);
};

