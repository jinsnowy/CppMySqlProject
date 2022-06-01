#pragma once
#include "SqlCmd.h"

class XStatement;
class XStoredProcedure : public SqlNoCmd<XStoredProcedure>
{
private:
	sql::SQLString mDropProcedureString;
	sql::SQLString mCreateProcedureString;
	sql::SQLString mCallString;
	sql::SQLString mQueryResultString;

protected:
	std::string mName;
	std::vector<std::string> mOutParams;
	std::unique_ptr<XStatement> mStatement;

public:
	int mCallResult;
	std::unique_ptr<sql::ResultSet> mResultSet;

	XStoredProcedure(const std::string& name, const std::string& dropString, const std::string& createString, const std::vector<std::string>& outParams)
		:
		mName(name),
		mDropProcedureString(dropString),
		mCreateProcedureString(createString),
		mCallResult(-1),
		mOutParams(outParams),
		mStatement(nullptr)
	{}

	XStoredProcedure(const std::string& name, const std::vector<std::string>& outParams)
		:
		mName(name),
		mCallResult(-1),
		mOutParams(outParams),
		mStatement(nullptr)
	{}

	void Initialize();

	int GetResult() const { return mCallResult; }
	void CreateStatement(DbConnection* const& conn);
	std::unique_ptr<XStoredProcedure> GetExecutable()
	{
		return std::make_unique<XStoredProcedure>(mName, mOutParams);
	}

protected:
	virtual bool OnExecute() override;
	virtual void OnError() override;
	virtual const char* Where() override { return mCallString.c_str(); }

public:
	template<typename ...Args>
	void Bind(XStatement* const& statement, Args&&... args)
	{
		mStatement = statement;

		std::stringstream ss;
		ss << Format::format("CALL %s(", mName.c_str());

		if (sizeof...(args) > 0)
		{
			using List = int[];
			(void)List {
				0, ((void)(ss << args << ", "), 0) ...
			};
		}

		BindOutParams(ss);
		ss << ")";

		mCallString = ss.str();
	}

	template<typename ...Args>
	void Bind(Args&&... args)
	{
		std::stringstream ss;
		ss << Format::format("CALL %s(", mName.c_str());

		if (sizeof...(args) > 0)
		{
			using List = int[];
			(void)List {0, ((void)(ss << args << ", "), 0) ... };
		}

		BindOutParams(ss);
		ss << ")";

		mCallString = ss.str();
	}

private:
	void BindOutParams(std::stringstream& ss)
	{
		if (mOutParams.size() == 0)
			return;

		std::stringstream rs;

		int szParams = (int)mOutParams.size();
		for (int i = 0; i < szParams; ++i)
		{
			rs << "@" << mOutParams[i];
			if (i != szParams - 1)
			{
				rs << ", ";
			}
		}
		
		auto rs_str = rs.str();
		ss << rs_str;
		mQueryResultString = Format::format("SELECT %s;", rs_str.c_str());
	}
};