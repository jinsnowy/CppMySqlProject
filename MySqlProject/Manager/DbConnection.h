#pragma once

class XStatement;
class XPrepareStatement;
class DbConnection
{
private:
	std::string mServer;
	std::string mUsername;
	std::string mPassword;

	std::unique_ptr<sql::Connection> mConn;

public:
	DbConnection(const std::string& server, const std::string& username, const std::string& password, sql::Connection* conn)
		:
		mServer(server),
		mUsername(username),
		mPassword(password),
		mConn(conn)
	{}

	sql::Connection* Raw() const { return mConn.get(); }

	sql::Statement* CreateRawStatement() { return mConn->createStatement(); }
	std::unique_ptr<XStatement> CreateStatement();
	std::unique_ptr<XPrepareStatement> CreatePrepareStatement(const std::string& statement, bool bTransaction = true);

	void SetAutoCommit(bool bAuto);
	void SetIsolationLevel(sql::enum_transaction_isolation level);
};