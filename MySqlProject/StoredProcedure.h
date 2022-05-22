#pragma once
#include <memory>
#include "Common/Format.h"
#include "Common/DateTime.h"
#include "Logger/Logger.h"
#include "Manager/SPManager.h"
#include "SqlCmd/XStoredProcedure.h"
#include "SqlCmd/XStatement.h"

struct BaseSP
{
	bool executed;
	XStoredProcedure* procedure;

	BaseSP(const char* sp_name)
		:
		executed(false),
		procedure(SPManager::GetProcedure(sp_name))
	{}
	
	bool Execute()
	{
		executed = OnExecute();
		if (executed)
		{
			executed &= OnSuccess();
		}
		else
		{
			OnError(Logger::Get());
		}
		
		return executed;
	}

	long long GetElapsedMilliSec() const
	{
		return procedure->GetElapsedMilliSec();
	}

protected:
	virtual bool OnExecute() = 0;
	virtual bool OnSuccess() { return true; };
	virtual void OnError(Logger* logger) {};
};

struct AddCashSP : public BaseSP
{
	long long srcAccountId;
	long long cashAmount;
	long long cashAmountAfter;

	DateTime updated;

	AddCashSP(long long srcAccountIdIn, long long cashAmountIn, long long cashAmountAfterIn)
		:
		srcAccountId(srcAccountIdIn), cashAmount(cashAmountIn), cashAmountAfter(cashAmountAfterIn),
		BaseSP("AddCash")
	{}

	virtual bool OnExecute() override
	{
		procedure->Bind(srcAccountId, cashAmount, cashAmountAfter);

		return procedure->Execute();
	}

	virtual bool OnSuccess()
	{
		updated = DateTime::FromString(procedure->mResultSet->getString("@TRANSACTIONTIME"));

		return true;
	}
};

struct SendCashSP : public BaseSP
{
	long long srcAccountId;
	long long dstAccountId;
	long long cashAmount;
	long long afterSrcCashAmount;
	long long afterDstCashAmount;

	DateTime updated;

	SendCashSP(long long srcAccountIdIn, long long dstAccountIdIn, long long cashAmountIn, long long afterSrcCashAmountIn, long long afterDstCashAmountIn)
		:
		srcAccountId(srcAccountIdIn), dstAccountId(dstAccountIdIn), cashAmount(cashAmountIn), afterSrcCashAmount(afterSrcCashAmountIn), afterDstCashAmount(afterDstCashAmountIn),
		BaseSP("SendCash")
	{}

	virtual bool OnExecute() override
	{
		procedure->Bind(srcAccountId, dstAccountId, cashAmount, afterSrcCashAmount, afterDstCashAmount);

		return procedure->Execute();
	}

	virtual bool OnSuccess()
	{
		updated = DateTime::FromString(procedure->mResultSet->getString("@TRANSACTIONTIME"));

		return true;
	}
};