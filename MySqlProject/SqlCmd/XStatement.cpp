#include "stdafx.h"
#include "XStatement.h"
#include "XQueryResult.h"

std::shared_ptr<XQueryResult> XStatement::ExecuteQuery(const std::string& statement)
{
	auto query = std::shared_ptr<XQueryResult>(new XQueryResult(mStatement->getConnection()->createStatement(), statement));
	query->Execute();

	return query;
}

void XStatement::ExecuteQueryResult(const sql::SQLString& statement, std::unique_ptr<sql::ResultSet>& resultset)
{
	resultset.reset(mStatement->executeQuery(statement));
}
