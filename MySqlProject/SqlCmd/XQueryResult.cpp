#include "stdafx.h"
#include "XQueryResult.h"
#include "XStatement.h"

bool XQueryResult::OnExecute()
{
	mResult.reset(mStatement->ExecuteRawQuery(mQueryString));

	return mResult != nullptr;
}
