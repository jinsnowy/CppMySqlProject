#pragma once
#include "SqlCmd.h"

class XStatement;
class XStoredProcedure : public SqlNoCmd<XStoredProcedure>
{
private:
	std::string mName;
	sql::SQLString mDropProcedureString;
	sql::SQLString mCreateProcedureString;
	sql::SQLString mCallString;

	std::shared_ptr<XStatement> mStatement;
private:
	int mResult;

public:
	XStoredProcedure(const std::string& name, const std::string& dropString, const std::string& createString)
		:
		mName(name),
		mDropProcedureString(dropString),
		mCreateProcedureString(createString),
		mResult(-1)
	{}

	void Initialize(const std::shared_ptr<XStatement>& statement);

	int GetResult() const { return mResult; }
protected:
	virtual void OnExecute() override;
	virtual void OnError() override;
	virtual const char* Where() override { return mCallString.c_str(); }

public:
	template<typename ...Args>
	void Bind(const std::shared_ptr<XStatement>& statement, Args&&... args)
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
		ss << "@result)";

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
		ss << "@result)";

		mCallString = ss.str();
	}
};

