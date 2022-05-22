#pragma once

#include "KeyInfo.h"
#include "AccountInfo.h"

class UserInfo : public KeyInfo<long long>
{
private:
	std::string mUserName;
	std::shared_ptr<AccountInfo> mAccount;

public:
	UserInfo(long long _userId, const std::string& _userName)
		: KeyInfo(_userId), mUserName(_userName), mAccount(nullptr)
	{}

	long long GetUserId() const { return GetKey(); }

	const std::string& GetUserName() const { return mUserName; }

	void UpdateCash(long long afterAmount, const DateTime& updated) 
	{
		if (mAccount == nullptr) 
			throw std::exception(Format::format("No Account On %s", mUserName.c_str()).c_str()); 

		mAccount->Update(afterAmount, updated);
	}

	long long GetCash() const { if (mAccount == nullptr) return 0; return mAccount->GetCash(); }

	bool HasAccount() const { return mAccount != nullptr; }

	bool HasEnough(long long amount) const { if (mAccount == nullptr) return 0; return mAccount->GetCash() >= amount; }

	void AddAcount(const std::shared_ptr<AccountInfo>& info)
	{
		if (mAccount != nullptr)
			throw std::exception(Format::format("Already Has Account On %s", mUserName.c_str()).c_str());

		mAccount = info;
	}

	std::shared_ptr<AccountInfo> GetAccount() const { return mAccount; }
};