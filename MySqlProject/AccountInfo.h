#pragma once

class UserInfo;
class AccountInfo
{
private:
	long long mAccountId;
	long long mCashAmount;

	DateTime mCreated;
	DateTime mUpdated;

	UserInfo* mOwner;
public:
	AccountInfo(UserInfo* user, long long accountId, long long cashAmount, const DateTime& created, const DateTime& updated)
		:
		mOwner(user),
		mAccountId(accountId),
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

