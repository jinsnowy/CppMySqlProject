#pragma once

class UserInfo;
class UserFactory
{
private:
	static std::unique_ptr<UserFactory> mInst;

	std::unordered_map<long long, std::shared_ptr<UserInfo>> mUsers;
	std::map<std::string, std::shared_ptr<UserInfo>> mUsersByName;
public:
	static void Initialize();

	static UserFactory* Get() { return mInst.get(); }

	std::vector<std::shared_ptr<UserInfo>> GetUsers() const;

	void CreateUser(long long userId, const std::string& userName);

	std::shared_ptr<UserInfo> GetUser(long long userId);

	std::shared_ptr<UserInfo> GetUser(const std::string& userName);

	void DeleteUser(long long userId);

	void ClearAll();
};