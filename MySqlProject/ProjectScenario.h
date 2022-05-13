#pragma once

class ProjectScenario
{
private:
	static std::unique_ptr<ProjectScenario> mInst;

public:
	static void Initialize();

	static void CreateTables();

	static void InitializeDatas();

	static void CreateUsers(int nUsers = 1000);

	static void CreateAccountDatas();
};