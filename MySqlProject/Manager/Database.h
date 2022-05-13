#pragma once

namespace sqldef
{
	class XTable;
}

class Database
{
private:
	std::string mName;
	std::string mUseString;

	std::unordered_map<std::string, std::unique_ptr<sqldef::XTable>> mTables;
public:
	Database(const std::string& name);

	~Database();

	const char* GetName() const { return mName.c_str(); }

	static std::unique_ptr<Database> Initialize(const std::string& name);

	sqldef::XTable* CreateTable(std::unique_ptr<sqldef::XTable> table, bool bDropIfExists = true);

	bool ClearTable(const std::string& tableName);

	std::vector<sqldef::XTable*> GetAllTables() const;

	const std::string& GetUseString() { return mUseString; }
};