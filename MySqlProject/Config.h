#pragma once

class Config
{
private:
	static std::unique_ptr<Config> mInst;

	std::unordered_map<std::string, std::string> mMySqlConfig;

public:
	static void Initialize(const char* filePath);

	static std::string GetHostname();
	static std::string GetUsername();
	static std::string GetPassword();

private:
	void Load(const char* filePath);
};

