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

public:
	Database(const std::string& name);

	const char* GetName() const { return mName.c_str(); }

	static std::unique_ptr<Database> Initialize(const std::string& name);

	bool CreateTable(sqldef::XTable* table, bool bDropIfExists = true);
private:
	const char* getUseString() { return mUseString.c_str(); }
};