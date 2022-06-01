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
			// TODO : amend option... ON UPDATE/DELETE CASCADE
			return Format::format("FOREIGN KEY(%s) REFERENCES %s(%s) ON DELETE CASCADE", mCol.c_str(), mFkTable.c_str(), mFkCol.c_str());
		}
	};

	struct IndexInfo
	{
		mutable std::string mName;
		std::string mTable;
		std::vector<std::string> mIndexCols;

		std::string Tag() const
		{
			mName = std::string("idx_") + Format::join(mIndexCols, "_");
			string index_str = Format::join(mIndexCols, ", ");
			return Format::format("CREATE INDEX %s ON %s (%s)", mName.c_str(), mTable.c_str(), index_str.c_str());
		}
	};

	class XTable
	{	
	private:
		template <typename T, typename = std::enable_if_t<std::is_constructible_v<const std::string&, const T&>>>
		using String = T;
	private:
		std::string mName;
		std::vector<std::unique_ptr<XColumn>> mColumns;
		std::vector<std::unique_ptr<FKInfo>> mFks;
		std::vector<std::unique_ptr<IndexInfo>> mIndexes;

	public:
		XTable(const std::string& name)
			:
			mName(name)
		{}

		XTable(const XTable&) = delete;
		XTable& operator=(const XTable&) = delete;

		std::string GetName() const { return mName; }
	
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

		bool IsValid() const;

		void PushFk(const char* srcCol, const char* FkTable, const char* FkCol);

		template <typename... Ts>
		void PushIndex(const String<Ts>&... ss)
		{
			IndexInfo* info = new IndexInfo();
			info->mTable = mName;
			PushIndexIn(info, ss...);
			mIndexes.emplace_back(info);
		}

		bool HasIndex() const { return mIndexes.size() != 0; }
		const vector<std::unique_ptr<IndexInfo>>& GetIndexInfos() const { return mIndexes; }

		std::string Drop(Database* database);
		std::string Create(Database* database);
	private:
		template <typename T, typename... Ts>
		void PushIndexIn(IndexInfo* info, const String<T>& argStr)
		{
			if (std::find_if(mColumns.begin(), mColumns.end(), [&](const std::unique_ptr<XColumn>& col) { return col->IsSame(argStr); })
				== mColumns.end())
			{
				auto msg = Format::format("No Column Name %s For Create Index", std::string(argStr).c_str());
				throw new std::exception(msg.c_str());
			}

			info->mIndexCols.emplace_back(argStr);
		}

		template <typename T, typename... Ts>
		void PushIndexIn(IndexInfo* info, const String<T>& argStr, const String<Ts>&... argStrs)
		{
			PushIndexIn(info, argStr);
			PushIndexIn(info, argStrs...);
		}
	};

	static std::unique_ptr<XTable> Table(const char* name)
	{
		return std::make_unique<XTable>(name);
	}

};
