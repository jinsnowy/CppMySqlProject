#pragma once
#include "KeyInfo.h"

class UserInfo;
class AccountInfo : public KeyInfo<long long>
{
private:
	UserInfo* mOwner;
	long long mCashAmount;

	DateTime mCreated;
	DateTime mUpdated;

public:
	AccountInfo(UserInfo* user, long long accountId, long long cashAmount, const DateTime& created, const DateTime& updated)
		:
		KeyInfo(accountId),
		mOwner(user),
		mCashAmount(cashAmount),
		mCreated(created),
		mUpdated(updated)
	{}

	static std::shared_ptr<AccountInfo> Create(UserInfo* user, long long accountId, long long cashAmount, const DateTime& created, const DateTime& updated)
	{
		return std::make_shared<AccountInfo>(user, accountId, cashAmount, created, updated);
	}

	void Update(long long afterCashAmount, const DateTime& updated)
	{
		if (afterCashAmount < 0)
			throw std::exception(Format::format("Updated To Negative Cash Amount %lld", afterCashAmount).c_str());

		mCashAmount = afterCashAmount;
		mUpdated = updated;
	}

	long long GetCash() const { return mCashAmount; }
};

