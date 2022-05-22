#pragma once
#include "SqlCmd.h"

class XStatement;
class XRowElement;
class XQueryResult : public SqlNoCmd<XQueryResult>
{
private:
	sql::SQLString mQueryString;
	std::unique_ptr<XStatement> mStatement;
	std::shared_ptr<sql::ResultSet> mResult;

public:
	XQueryResult(sql::Statement* const& statement, const std::string& queryString)
		:
		mStatement(std::make_unique<XStatement>(statement)),
		mQueryString(queryString)
	{
	}

protected:
	virtual bool OnExecute() override;
	virtual const char* Where() override { return "Query"; }

public:
	std::shared_ptr<sql::ResultSet> GetResult() const
	{
		return mResult;
	}
};
