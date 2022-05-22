#include "stdafx.h"
#include "UserFactory.h"
#include "UserInfo.h"

std::unique_ptr<UserFactory> UserFactory::mInst = nullptr;

void UserFactory::Initialize()
{
	mInst.reset(new UserFactory());
}

void UserFactory::ClearAll()
{
	mUsers.clear();
	mUsersByName.clear();
}

std::vector<std::shared_ptr<UserInfo>> UserFactory::GetUsers() const
{
	std::vector<std::shared_ptr<UserInfo>> result;
	for (const auto& pair : mUsersByName)
	{
		result.push_back(pair.second);
	}

	return result;
}

std::shared_ptr<UserInfo> UserFactory::CreateUser(long long userId, const std::string& userName)
{
	mUsers.emplace(userId, std::make_shared<UserInfo>(userId, userName));
	mUsersByName.emplace(userName, mUsers[userId]);

	return mUsers[userId];
}

std::shared_ptr<UserInfo> UserFactory::GetUser(long long userId)
{
	if (mUsers.find(userId) == mUsers.end())
		return nullptr;

	return mUsers[userId];
}

std::shared_ptr<UserInfo> UserFactory::GetUser(const std::string& userName)
{
	if (mUsersByName.find(userName) == mUsersByName.end())
		return nullptr;

	return mUsersByName[userName];
}

void UserFactory::DeleteUser(long long userId)
{
	if (mUsers.find(userId) == mUsers.end())
		return;

	auto user = mUsers[userId];
	mUsers.erase(user->GetUserId());
	mUsersByName.erase(user->GetUserName());
}