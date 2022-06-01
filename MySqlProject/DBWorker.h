#pragma once

struct BaseSP;
struct DbWorkerItem
{
	std::shared_ptr<BaseSP> sp;
	DbConnection* conn;
	function<void(std::shared_ptr<BaseSP>)> onSuccess;

	~DbWorkerItem()
	{
	}

	DbWorkerItem(std::shared_ptr<BaseSP>&& spIn, DbConnection* connIn, function<void(std::shared_ptr<BaseSP>)>&& onSuccess)
		:
		sp(std::move(spIn)),
		conn(connIn),
		onSuccess(std::move(onSuccess))
	{
	}

	DbWorkerItem(DbWorkerItem&& other) noexcept
		:
		sp(std::move(other.sp)),
		conn(other.conn),
		onSuccess(std::move(other.onSuccess))
	{
	}

	DbWorkerItem& operator=(DbWorkerItem&& other) noexcept
	{
		sp = std::move(other.sp);
		conn = other.conn;
		onSuccess = std::move(other.onSuccess);
	}
};

class DbWorker
{
private:
	bool bExitFlag;
	std::queue<DbWorkerItem> items;
	std::condition_variable cv;
	std::mutex mtx;
	std::thread worker;
public:
	DbWorker();

	~DbWorker();

	void Queue(std::shared_ptr<BaseSP> sp, DbConnection* conn, function<void(std::shared_ptr<BaseSP>)> onSuccess);

	void Wait();
private:
	void Work();
};