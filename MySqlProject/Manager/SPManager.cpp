#include "stdafx.h"
#include "SPManager.h"
#include "SqlCmd/XStatement.h"
#include "SqlCmd/XStoredProcedure.h"

#include <locale>
#include <codecvt>
#include <regex>

std::unique_ptr<SPManager> SPManager::mInst = nullptr;

SPManager::SPManager()
{
}

SPManager::~SPManager()
{
}

void SPManager::Initialize(const std::string& spFileName)
{
	mInst.reset(new SPManager());

	std::string script;
	if (!mInst->ReadSqlScript(spFileName, script))
	{
		Logger::ErrorLog("SPManager Initialize Failed");
		return;
	}

	if (!mInst->ParseScript(script))
	{
		Logger::ErrorLog("SPManager Initialize Failed");
		return;
	}
}

void SPManager::Install(const std::shared_ptr<XStatement>& statement)
{
	auto& container = mInst->mContainer;
	for (auto& pair : container)
	{
		pair.second->Initialize(statement);
	}
}

XStoredProcedure* SPManager::GetProcedure(const std::string& spName)
{
	const auto& container = mInst->mContainer;
	auto found = container.find(spName);
	if (found != container.end())
		return found->second.get();

	return nullptr;
}

bool SPManager::ReadSqlScript(const std::string& spFileName, std::string& script)
{
	try 
	{
		std::string filePath = PathManager::GetPath(spFileName);
		std::ifstream fin(filePath, std::ios::in);
		if (!fin.is_open())
		{
			return false;
		}

		//skip BOM
		fin.seekg(2);

		//read as raw bytes
		std::stringstream ss;
		ss << fin.rdbuf();
		std::string bytes = ss.str();

		//make sure len is divisible by 2
		int len = (int)bytes.size();
		if (len % 2) len--;

		std::wstring sw;
		for (size_t i = 0; i < len;)
		{
			//little-endian
			int lo = bytes[i++] & 0xFF;
			int hi = bytes[i++] & 0xFF;
			sw.push_back(hi << 8 | lo);
		}

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
		script = convert.to_bytes(sw);
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Load Stored Procedure File Failed : %s\n", e.what());
		return false;
	}

	return true;
}

// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

bool SPManager::ParseScript(const std::string& script)
{
	try {
		std::string rawScript = std::regex_replace(script, std::regex("\r\n"), "\n");
		std::vector<std::string> lines = Split(rawScript, "\n");

		for (auto& line : lines)
		{
			ltrim(line);
		}

		mContainer.clear();
		std::string sp_name, sp_drop_string, sp_create_string;
		std::regex sp_def_regex("PROCEDURE\\s?(\\w+)");

		size_t i = 0, sz = lines.size();
		while (i < sz)
		{
			const auto& line = lines[i++];
			if (line.find("CREATE") == std::string::npos)
				continue;

			std::match_results<std::string::const_iterator > mr;
			if (!std::regex_search(line, mr, sp_def_regex))
				continue;

			sp_name = mr[1];
			sp_drop_string = Format::format("DROP PROCEDURE IF EXISTS %s;", sp_name.c_str());

			std::stringstream ss;
			ss << line;

			int count = 0;
			while (i < sz)
			{
				const auto& inner_line = lines[i++];
				ss << inner_line;
				++count;
				if (inner_line.find("END") != std::string::npos)
					break;
			}

			if (count > 2)
			{
				sp_create_string = ss.str();
				mContainer.emplace(sp_name, std::make_unique<XStoredProcedure>(sp_name, sp_drop_string, sp_create_string));
			}
		}
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Parse Stored Procedure Failed : %s\n", e.what());
		return false;
	}
	
	return true;
}

std::vector<std::string> SPManager::Split(std::string source, std::string delimiter)
{
	size_t delim_sz = delimiter.size();
	size_t pos = 0, offset = 0;

	std::vector<std::string> result;
	while ((pos = source.find(delimiter, offset)) != std::string::npos)
	{
		auto str = source.substr(offset, pos - offset + 1);
		result.emplace_back(std::move(str));
		offset = pos + delim_sz;
	}
	
	if (offset < source.size())
	{
		result.emplace_back(source.substr(offset));
	}
	
	return result;
}
