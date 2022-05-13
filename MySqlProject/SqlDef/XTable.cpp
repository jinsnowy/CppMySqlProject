#include "stdafx.h"
#include "XTable.h"
#include "Manager/Database.h"

static std::string wrap(const std::string& s, char c = '`')
{
	return Format::format("%c%s%c", c, s.c_str(), c);
}

bool sqldef::XTable::IsValid() const
{
	int pk_count = (int)std::count_if(mColumns.begin(), mColumns.end(), [&](const std::unique_ptr<XColumn>& col) { return col->IsPk(); });
	if (pk_count != 1)
		return false;

	return true;
}

void sqldef::XTable::PushFk(const char* srcCol, const char* FkTable, const char* FkCol)
{
	for (const auto& col : mColumns)
	{
		if (col->IsPk() && col->IsSame(srcCol))
		{
			auto msg = Format::format("Duplicate Key : %s and %s", col->GetName(), srcCol);
			throw std::exception(msg.c_str());
		}
	}

	std::string cmp = srcCol;
	if (mFks.size() > 0)
	{
		auto iter = std::find_if(mFks.begin(), mFks.end(), [&cmp](const std::unique_ptr<FKInfo>& info) { return info->mCol == cmp; });
		if (iter != mFks.end())
		{
			auto msg = Format::format("Duplicate Foreign Key : %s", srcCol);
			throw std::exception(msg.c_str());
		}
	}
	
	FKInfo* info = new FKInfo();
	info->mCol = std::move(cmp);
	info->mFkCol = FkCol;
	info->mFkTable = FkTable;

	mFks.emplace_back(info);
}

std::string sqldef::XTable::Drop(Database* database)
{
	std::stringstream ss;
	ss << "DROP TABLE IF EXISTS ";
	ss << wrap(database->GetName()) << '.' << wrap(mName) << ";";

	return ss.str();
}

std::string sqldef::XTable::Create(Database* database)
{
	std::stringstream ss;
	ss << "CREATE TABLE ";
	ss << database->GetName() << '.' << mName << " (";

	int num = (int)mColumns.size();
	for (int i = 0; i < num; ++i)
	{
		ss << mColumns[i]->Build();
		if (i != num - 1)
		{
			ss << ", ";
		}
	}

	for (const auto& info : mFks)
	{
		ss << ", CONSTRAINT " << info->Tag();
	}
	ss << ")";

	return ss.str();
}