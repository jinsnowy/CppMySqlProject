#include "stdafx.h"
#include "Config.h"
#include "tinyxml2.h"

using namespace tinyxml2;

std::unique_ptr<Config> Config::mInst = nullptr;

void Config::Initialize(const char* filePath)
{
	mInst.reset(new Config());
	mInst->Load(filePath);
}

std::string Config::GetHostname() 
{
	const auto& config = mInst->mMySqlConfig;
	return Format::format("tcp://%s:%s", config.at("hostaddress").c_str(), config.at("port").c_str());
}

std::string Config::GetUsername() 
{
	const auto& config = mInst->mMySqlConfig;
	return config.at("username");
}

std::string Config::GetPassword() 
{
	const auto& config = mInst->mMySqlConfig;
	return config.at("password");
}

void Config::Load(const char* filePath)
{
	std::string fullPath = PathManager::GetPath(filePath);
	XMLDocument doc;
	try 
	{
		doc.LoadFile(fullPath.c_str());
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Load Config File Failed : Error %s, Path %s", e.what(), filePath);
		return;
	}

	mMySqlConfig.clear();
	
	try 
	{
		auto root = doc.FirstChildElement("Config");
		auto sqlRoot = root->FirstChildElement("MySQL");
		for (auto node = sqlRoot->FirstChild(); node; node = node->NextSibling())
		{
			auto element = node->ToElement();
			mMySqlConfig.emplace(element->Value(), element->GetText());
		}
	}
	catch (std::exception e)
	{
		Logger::ErrorLog("Load Config File Failed : Error %s", e.what());
	}
}
