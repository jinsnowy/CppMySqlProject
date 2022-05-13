
CREATE PROCEDURE SendCashToUserAccount(IN srcAccountIdIn BIGINT, IN dstAccountIdIn BIGINT, IN amount BIGINT, OUT result INT)
BEGIN
	DECLARE prevSrcAmount BIGINT;
	DECLARE nextSrcAmount BIGINT;
	DECLARE prevDstAmount BIGINT;
	DECLARE nextDstAmount BIGINT;
	DECLARE transactionTime DATETIME DEFAULT CURRENT_TIMESTAMP;

	SET result = 0;

	SELECT cashAmount INTO prevSrcAmount FROM AccountTbl WHERE accountId = srcAccountIdIn;
	SELECT cashAmount INTO prevDstAmount FROM AccountTbl WHERE accountId = dstAccountIdIn;

	SET nextSrcAmount = prevSrcAmount - amount;
	SET nextDstAmount = prevDstAmount + amount;

	START TRANSACTION;
	UPDATE AccountTbl SET cashAmount = nextSrcAmount WHERE accountId = srcAccountIdIn;
	UPDATE AccountTbl SET cashAmount = nextDstAmount WHERE accountId = dstAccountIdIn;
	INSERT INTO TransactionTbl(cashAmount, srcAccountId, dstAccountId, created) VALUES (amount, srcAccountIdIn, dstAccountIdIn, transactionTime);
	COMMIT;
END