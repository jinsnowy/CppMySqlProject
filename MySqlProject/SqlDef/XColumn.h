#pragma once

#include "ColumnAttribute.h"

namespace sqldef
{
	using AttrInfo = std::shared_ptr<ColumnAttributeBase>;

	
	class XColumn
	{
	private:
		std::string mName;

		std::map<EColumnAttributeType, AttrInfo> mAttributes;

	public:
		XColumn(const std::string& name, EDataType dataType, int size = 0)
			:
			mName(name)
		{
			Append(Attr<DataType>(dataType, size));
		}

		const char* GetName() const { return mName.c_str(); }

		bool IsSame(std::string name) const 
		{
			std::string ref = mName;
			std::transform(ref.begin(), ref.end(), ref.begin(), [](const char& c) { return std::tolower(c); }); 
			std::transform(name.begin(), name.end(), name.begin(), [](const char& c) { return std::tolower(c); });

			return name == ref;
		}

		bool IsPk() const { for (const auto& el : mAttributes) { if ((el.second.get())->IsPk()) return true; } return false; }

		template<typename T, typename ...Args, typename = std::enable_if_t<std::is_base_of_v<ColumnAttributeBase, T>>>
		XColumn& Append(Args&&... args)
		{
			auto attribute = Attr<T>(std::forward<Args>(args)...);
			auto type = attribute->GetType();
			if (mAttributes.find(type) != mAttributes.end())
			{
				auto msg = Format::format("Duplicate Attribute %d", type);
				throw std::exception(msg.c_str());
			}

			mAttributes.emplace(type, std::move(attribute));

			return *this;
		}

		XColumn& Append(const AttrInfo& attribute)
		{
			auto type = attribute->GetType();
			if (mAttributes.find(type) != mAttributes.end())
			{
				auto msg = Format::format("Duplicate Attribute %d", type);
				throw std::exception(msg.c_str());
			}

			mAttributes.emplace(type, attribute);

			return *this;
		}

		template<typename ...AttrInfos>
		XColumn& Append(const AttrInfo& attribute, const AttrInfos&... attributes)
		{
			Append(attribute);
			Append(attributes...);

			return *this;
		}

		XColumn(XColumn&& rhs) noexcept
			:
			mName(std::move(rhs.mName)),
			mAttributes(std::move(rhs.mAttributes))
		{}

		XColumn& operator=(XColumn&& rhs) noexcept
		{
			mName = std::move(rhs.mName);
			mAttributes = std::move(rhs.mAttributes);
			return *this;
		}

		std::string Build() const;
	};

	template<EDataType dataType, int size = 0>
	static std::unique_ptr<XColumn> Column(const char* name)
	{
		return std::make_unique<XColumn>(name, dataType, size);
	}

	template<EDataType dataType, int size = 0, typename ...AttrInfos>
	static std::unique_ptr<XColumn> Column(const char* name, const AttrInfos& ...attributes)
	{
		auto column = std::make_unique<XColumn>(name, dataType, size);
		column->Append(attributes...);

		return column;
	}

}