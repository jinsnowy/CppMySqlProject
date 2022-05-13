#include "stdafx.h"
#include "XStoredProcedure.h"
#include "XStatement.h"
#include "XQueryResult.h"
#include "Manager/DbConnection.h"
#include "Manager/DatabaseManager.h"

void XStoredProcedure::Initialize()
{
	auto connection = DatabaseManager::Get()->GetDefaultConnection();
	if (connection == nullptr)
	{
		Logger::ErrorLog("No Default Connection");
		return;
	}

	auto statement = connection->CreateStatement();
	mStatement = statement;
	mStatement->Execute(mDropProcedureString);
	mStatement->Execute(mCreateProcedureString);

	Logger::DebugLog("Procedure %s Created.", mName.c_str());
}

void XStoredProcedure::OnExecute()
{
	mResult = -1;

	if (mCallString.length() == 0)
	{
		Bind();
	}

	mStatement->Execute(mCallString);
	auto query = mStatement->ExecuteQuery("SELECT @result;");
	auto queryResult = query->GetResult();
	if (queryResult->next())
	{
		mResult = queryResult->getInt("@result");
	}
	else
	{
		throw new std::exception("No Result On SP Query");
	}
}

void XStoredProcedure::OnError()
{
	mStatement->Execute("ROLLBACK");
}

void XStoredProcedure::BindStatement(const std::shared_ptr<XStatement> statement)
{
	mStatement = statement;
}
