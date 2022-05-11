#pragma once

#include "XColumn.h"
class Database;

namespace sqldef
{
	struct FKInfo
	{
		std::string mCol;
		std::string mFkTable;
		std::string mFkCol;

		std::string Tag() const
		{

			return Format::format("FOREIGN KEY(%s) REFERENCES %s(%s) ON DELETE CASCADE", mCol.c_str(), mFkTable.c_str(), mFkCol.c_str());
		}
	};

	class XTable
	{	
	private:
		std::string mName;
		std::vector<std::unique_ptr<XColumn>> mColumns;
		std::vector<std::unique_ptr<FKInfo>> mFks;

	public:
		XTable(const std::string& name)
			:
			mName(name)
		{}

		XTable(const XTable&) = delete;
		XTable& operator=(const XTable&) = delete;
	
		XTable& Append(std::unique_ptr<XColumn>&& column)
		{
			if (std::find_if(mColumns.begin(), mColumns.end(), [&](const std::unique_ptr<XColumn>& col) { return col->IsSame(column->GetName()); })
				!= mColumns.end())
			{
				auto msg = Format::format("Duplicate Column Name %s", column->GetName());
				throw new std::exception(msg.c_str());
			}

			mColumns.emplace_back(std::move(column));

			return *this;
		}

		void PushFk(const char* srcCol, const char* FkTable, const char* FkCol);

		std::string Drop(Database* database);
		std::string Create(Database* database);
	};

	static std::unique_ptr<XTable> Table(const char* name)
	{
		return std::make_unique<XTable>(name);
	}
};
