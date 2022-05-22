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
	statement->ExecuteRaw(mDropProcedureString);
	statement->ExecuteRaw(mCreateProcedureString);

	Logger::DebugLog("Procedure %s Created.", mName.c_str());
}

bool XStoredProcedure::OnExecute()
{
	mCallResult = -1;
	if (mCallString.length() == 0)
	{
		Bind();
	}

	auto conn = DatabaseManager::Get()->GetDefaultConnection();
	mStatement = conn->CreateStatement();
	mStatement->ExecuteRaw(mCallString);

	mStatement = conn->CreateStatement();
	mStatement->ExecuteQueryResult(mQueryResultString, mResultSet);
	if (mResultSet == nullptr || mResultSet->next() == false)
	{
		throw new std::exception("No Result On SP Query");
	}

	mCallResult = mResultSet->getInt("@RESULT");
	if (mCallResult != 0)
	{
		Logger::ErrorLog("Stored Procedure %s Error Result : %d", mName.c_str(), mCallResult);
		mStatement->ExecuteRaw("ROLLBACK");

		return false;
	}

	return true;
}

void XStoredProcedure::OnError()
{
	mStatement->ExecuteRaw("ROLLBACK");
}
