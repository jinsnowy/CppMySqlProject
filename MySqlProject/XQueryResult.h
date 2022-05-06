#pragma once
#include "SqlCmd.h"

class XStatement;
class XRowElement;
class XQueryResult : public SqlNoCmd<XQueryResult>
{
private:
	sql::SQLString mQueryString;
	std::shared_ptr<XStatement> mStatement;
	std::shared_ptr<sql::ResultSet> mResult;

public:
	XQueryResult(const std::shared_ptr<XStatement>& statement, const std::string& queryString)
		:
		mStatement(statement),
		mQueryString(queryString)
	{
	}

protected:
	virtual void OnExecute() override;
	virtual const char* Where() override { return "Query"; }

public:
	std::shared_ptr<sql::ResultSet> GetResult() const
	{
		return mResult;
	}
};
