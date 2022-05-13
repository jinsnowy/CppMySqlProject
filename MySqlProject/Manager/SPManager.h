#pragma once

class XStatement;
class XStoredProcedure;
class SPManager
{
private:
	static std::unique_ptr<SPManager> mInst;

private:
	std::unordered_map<std::string, std::unique_ptr<XStoredProcedure>> mContainer;

private:
	SPManager();

public:
	~SPManager();

	static void Initialize(const std::string& spFilePath);

	static void Install();

	static XStoredProcedure* GetProcedure(const std::string& spName);

private:
	bool ReadSqlScript(const std::string& spFilePath, std::string& script);

	bool ParseScript(const std::string& script);

	std::vector<std::string> Split(std::string source, std::string delimiter);
};

