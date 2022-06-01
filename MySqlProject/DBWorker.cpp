#include "stdafx.h"
#include "DBWorker.h"
#include "StoredProcedure.h"

DbWorker::DbWorker()
	:
	bExitFlag(false),
	worker([this]() { Work(); })
{
}

DbWorker::~DbWorker()
{
	{
		std::lock_guard<std::mutex> lk(mtx);
		bExitFlag = true;
	}

	cv.notify_one();
	worker.join();
}
void DbWorker::Queue(std::shared_ptr<BaseSP> sp, DbConnection* conn, function<void(std::shared_ptr<BaseSP>)> onSuccess)
{
	{
		std::lock_guard<std::mutex> lk(mtx);
		items.emplace(std::move(sp), conn, std::move(onSuccess));
	}

	cv.notify_one();
}

void DbWorker::Wait()
{
	do
	{
		{
			std::lock_guard<mutex> lk(mtx);
			if (items.size() == 0)
				return;
		}

		cv.notify_one();
		std::this_thread::sleep_for(chrono::milliseconds(100));

	} while (true);
}

void DbWorker::Work()
{
	const auto sp_work = [](DbWorkerItem& item)
	{
		if (item.sp->Execute(item.conn))
		{
			item.onSuccess(item.sp);
		}
	};

	while (!bExitFlag)
	{
		unique_lock<mutex> lk(mtx);
		cv.wait(lk, [this]() { return bExitFlag || items.size() > 0; });

		if (bExitFlag)
			return;

		auto item = std::move(items.front());
		items.pop();
		lk.unlock();

		sp_work(item);
	}
}