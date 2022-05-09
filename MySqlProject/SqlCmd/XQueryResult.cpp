#include "stdafx.h"
#include "XQueryResult.h"
#include "XStatement.h"

void XQueryResult::OnExecute()
{
	mResult.reset(mStatement->mStatement->executeQuery(mQueryString));
}
