## CppMySqlProject
 
## Overview
### Scenario Based Study C++ MySQL Connector
+ Config for DbConnection
	- MySQL C++ ODBC Connector
	- TinyXml2

+ User and Account (User deposits some cash in Account...)
	- Tables and StoredProcedures (SendCash, AddCash)

+ Programmable Database Object (Table Define, Query, PrepareStatement, StoreProcedure)
	- <u>Table Define</u>

```cpp
auto userTbl = Table("UserTbl");
userTbl->Append(Column<EDataType::BIGINT>("userId", Const::NotNull, Const::Auto, Const::Pk));
userTbl->Append(Column<EDataType::VARCHAR, 24>("userName", Const::NotNull, Const::Unique));
userTbl->Append(Column<EDataType::DATETIME>("created", Const::NotNull));
userTbl->PushIndex("userName");
if (db->CreateTable(std::move(userTbl)) == nullptr)
	return;
```

   - <u>Statement Wrappers</u>
   * statement create from connection 
   * XStatement, XPrepareStatement, XQueryResult, XStoredProcedure 

+ Scenario Test
	- No Select Using By Cache Object (Object In Server Side)
	- DbWorker (Single Thread) For SP

## Dependencies
### MySQL C++ ODBC Connector
![MySQL Connector](./mysql.png)
- Guide : https://dev.mysql.com/doc/connector-odbc/en/
- Download : https://dev.mysql.com/downloads/connector/odbc/