#pragma once

#include "SqlCmd.h"

class XStatement;
class XPrepareStatement : public SqlNoCmd<XPrepareStatement>
{
private:
	bool mTransaction;
	std::shared_ptr<sql::PreparedStatement> mStatement;

public:
	XPrepareStatement(sql::PreparedStatement* const& statement, bool bTransaction)
		:
		mStatement(statement),
		mTransaction(bTransaction)
	{}

public:
	virtual bool OnExecute() override { return mStatement->execute(); };
	virtual void OnError() override { if (mTransaction) mStatement->execute("ROLLBACK;"); }
	virtual const char* Where() override { return "PrepareStatement"; };

	std::shared_ptr<sql::ResultSet> ExecuteQuery(const std::string& statement) const { return std::shared_ptr<sql::ResultSet>(mStatement->executeQuery(statement)); }

public:
	void SetParam(int idx, const char* str) { mStatement->setString(idx, str); }
	void SetParam(int idx, const std::string& str) { mStatement->setString(idx, str); }
	void SetParam(int idx, const int& value) { mStatement->setInt(idx, value); }
	void SetParam(int idx, const unsigned int& value) { mStatement->setUInt(idx, value); }
	void SetParam(int idx, const long long& value) { mStatement->setInt64(idx, value); }
	void SetParam(int idx, const unsigned long long& value) { mStatement->setUInt64(idx, value); }
	void SetParam(int idx, const DateTime& date) { mStatement->setDateTime(idx, date.ToSqlString()); }
};