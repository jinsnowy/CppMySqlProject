#include "stdafx.h"
#include "XStatement.h"
#include "XQueryResult.h"

std::shared_ptr<XQueryResult> XStatement::ExecuteQuery(const std::string& statement)
{
	auto query = std::shared_ptr<XQueryResult>(new XQueryResult(shared_from_this(), statement));
	query->Execute();

	return query;
}