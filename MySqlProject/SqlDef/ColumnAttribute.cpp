#include "stdafx.h"
#include "ColumnAttribute.h"

using namespace sqldef;

std::shared_ptr<NullConstraint> Const::Null{ new NullConstraint(true) };
std::shared_ptr<NullConstraint> Const::NotNull{ new NullConstraint(false) };

std::shared_ptr<UniqueConstraint> Const::Unique{ new UniqueConstraint() };
std::shared_ptr<AutoIncrement> Const::Auto{ new AutoIncrement() };
std::shared_ptr<PrimaryKey> Const::Pk{ new PrimaryKey() };
std::shared_ptr<DefaultConstraint<int>> Const::Zero(new DefaultConstraint<int>(0));
std::shared_ptr<DefaultConstraint<int>> Const::MinusOne(new DefaultConstraint<int>(-1));