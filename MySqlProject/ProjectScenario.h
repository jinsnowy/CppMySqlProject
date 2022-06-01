#pragma once

class ProjectScenario
{
private:
	static std::unique_ptr<ProjectScenario> mInst;

public:
	static void Initialize();

	static void CreateTables();

	static void InitializeDatas();

	static void QueryDatas();

	static void CreateUsers(int nUsers = 1000);

	static void CreateAccountDatas();

	static void AddSomeCash();

	static void SendSomeCash();

	static void ConcurrentSendCashSingleWorker();
};