#include "stdafx.h"
#include "DbConnection.h"
#include "SqlCmd/XStatement.h"
#include "SqlCmd/XPrepareStatement.h"

std::shared_ptr<XStatement> DbConnection::CreateStatement()
{
	return std::shared_ptr<XStatement>(new XStatement(mConn->createStatement()));
}

std::shared_ptr<XPrepareStatement> DbConnection::CreatePrepareStatement(const std::string& statement, bool bTransaction)
{
	return std::shared_ptr<XPrepareStatement>(new XPrepareStatement(mConn->prepareStatement(statement), bTransaction));
}

void DbConnection::SetAutoCommit(bool bAuto)
{
	mConn->setAutoCommit(bAuto);
}

void DbConnection::SetIsolationLevel(sql::enum_transaction_isolation level)
{
	mConn->setTransactionIsolation(level);
}
