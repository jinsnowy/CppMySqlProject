#pragma once
namespace sqldef
{
	enum class EDataType
	{
		BOOL,
		SMALLINT,
		INT,
		BIGINT,
		CHAR,
		VARCHAR,
		DATE,
		DATETIME,
		TIMESTAMP,
		JSON
	};

	enum class EColumnAttributeType
	{
		DataType,
		NullContraint,
		UniqueContraint,
		AutoIncrementConstraint,
		DefaultConstraint,
		KeyContraint,
	};

	class ColumnAttributeBase
	{
	protected:
		std::string mTag;
		EColumnAttributeType mType;

		virtual void SetTag() {};
	public:
		ColumnAttributeBase(EColumnAttributeType type)
			:
			mType(type)
		{}

		bool IsPk() { return mType == EColumnAttributeType::KeyContraint; }
		EColumnAttributeType GetType() { return mType; }

		virtual ~ColumnAttributeBase() = default;
		virtual const char* Tag() { SetTag();  return mTag.c_str(); }
	};

	template<EColumnAttributeType type>
	class ColumnAttribute : public ColumnAttributeBase
	{
	public:
		ColumnAttribute()
			:
			ColumnAttributeBase(type)
		{}
	};


	template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of_v<ColumnAttributeBase, T>>>
	std::shared_ptr<T> Attr(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	class DataType : public ColumnAttribute<EColumnAttributeType::DataType>
	{
	private:
		EDataType mValue;
		int mSize;

	public:
		DataType(EDataType value, int size = 0)
			:
			mValue(value),
			mSize(size)
		{}

	protected:
		virtual void SetTag() override
		{
			switch (mValue)	
			{
			case sqldef::EDataType::BOOL:
				mTag = "BOOL";
				break;
			case sqldef::EDataType::SMALLINT:
				mTag = "SMALLINT";
				break;
			case sqldef::EDataType::INT:
				mTag = "INT";
				break;
			case sqldef::EDataType::BIGINT:
				mTag = "BIGINT";
				break;
			case sqldef::EDataType::CHAR:
				mTag = Format::format("CHAR(%d)", mSize);
				break;
			case sqldef::EDataType::VARCHAR:
				mTag = Format::format("VARCHAR(%d)", mSize);
				break;
			case sqldef::EDataType::DATE:
				mTag = "DATE";
				break;
			case sqldef::EDataType::DATETIME:
				mTag = "DATETIME";
				break;
			case sqldef::EDataType::TIMESTAMP:
				mTag = "TIMESTAMP";
				break;
			case sqldef::EDataType::JSON:
				mTag = "JSON";
				break;
			default:
				break;
			}
		}
	};

	class NullConstraint : public ColumnAttribute<EColumnAttributeType::NullContraint>
	{
	private:
		bool mValue;

	public:
		NullConstraint(bool value)
			:
			mValue(value)
		{}

		

	protected:
		virtual void SetTag() override { mTag = mValue ? "NULL" : "NOT NULL"; }
	};

	class UniqueConstraint : public ColumnAttribute<EColumnAttributeType::UniqueContraint>
	{
	public:
		UniqueConstraint()
		{}

	public:
		virtual const char* Tag() override { return "UNIQUE"; }
	};

	class AutoIncrement : public ColumnAttribute<EColumnAttributeType::AutoIncrementConstraint>
	{
	public:
		AutoIncrement()
		{}

	public:
		virtual const char* Tag() override { return "AUTO_INCREMENT"; }
	};

	template<typename T>
	class DefaultConstraint : public ColumnAttribute<EColumnAttributeType::DefaultConstraint>
	{
	private:
		T mValue;

	public:
		DefaultConstraint(const T& value)
			:
			mValue(value)
		{}

	public:
		virtual void SetTag() override { std::stringstream ss; ss << "DEFAULT " << mValue; mTag = ss.str(); }
	};

	template<>
	class DefaultConstraint<std::string> : public ColumnAttribute<EColumnAttributeType::DefaultConstraint>
	{
	private:
		std::string mValue;

	public:
		DefaultConstraint(const std::string& value)
			:
			mValue(value)
		{}

	public:
		virtual void SetTag() override { std::stringstream ss; ss << "DEFAULT '" << mValue << "'";  mTag = ss.str(); }
	};

	class PrimaryKey : public ColumnAttribute<EColumnAttributeType::KeyContraint>
	{
	public:
		PrimaryKey()
		{}

	public:
		virtual const char* Tag() override { return "PRIMARY KEY"; }
	};

	struct Const
	{
		static std::shared_ptr<UniqueConstraint> Unique;
		static std::shared_ptr<AutoIncrement> Auto;
		static std::shared_ptr<PrimaryKey> Pk;
		static std::shared_ptr<NullConstraint> NotNull;
		static std::shared_ptr<NullConstraint> Null;
		static std::shared_ptr<DefaultConstraint<int>> Zero;
		static std::shared_ptr<DefaultConstraint<int>> MinusOne;
	};
}
