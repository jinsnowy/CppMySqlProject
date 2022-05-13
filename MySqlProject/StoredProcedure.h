#pragma once

#include <memory>
#include "Manager/SPManager.h"
#include "SqlCmd/XStoredProcedure.h"
#include "SqlCmd/XStatement.h"

struct Executable
{
	virtual bool Execute(const std::shared_ptr<XStatement>& statement) = 0;
};

struct SendCashToUserAccount : public Executable
{
	long srcAccountId;
	long dstAccountId;
	long cashAmount;

	SendCashToUserAccount(long src, long dst, long cash)
		:
		srcAccountId(src), dstAccountId(dst), cashAmount(cash)
	{}

	virtual bool Execute(const std::shared_ptr<XStatement>& statement) override
	{
		static XStoredProcedure* procedure = SPManager::GetProcedure("SendCashToUserAccount");
		procedure->Bind(srcAccountId, dstAccountId, cashAmount);
		procedure->BindStatement(statement);

		return procedure->Execute();
	}
};