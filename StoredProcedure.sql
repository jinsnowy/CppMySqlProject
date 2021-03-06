CREATE PROCEDURE AddCash(IN srcAccountIdIn BIGINT, IN amount BIGINT, IN afterAmount BIGINT, OUT result INT, OUT transactionTime DATETIME)
proc_label:BEGIN
	DECLARE prevSrcAmount BIGINT;
	DECLARE nextSrcAmount BIGINT;
	
	SET result = -1;
	SET transactionTime = CURRENT_TIMESTAMP;

	SELECT cashAmount INTO prevSrcAmount FROM AccountTbl WHERE accountId = srcAccountIdIn;
	SET nextSrcAmount = prevSrcAmount + amount;
	IF (afterAmount != nextSrcAmount) THEN
		SET result = 1;
		LEAVE proc_label;
	END IF;

	START TRANSACTION;
	UPDATE AccountTbl SET cashAmount = nextSrcAmount WHERE accountId = srcAccountIdIn;
	INSERT INTO AccountHistoryTbl(cashAmount, infoType, accountId, created) VALUES (amount, 0, srcAccountIdIn, transactionTime);
	COMMIT;

	SET result = 0;
END

CREATE PROCEDURE SendCash(IN srcAccountIdIn BIGINT, IN dstAccountIdIn BIGINT, IN amount BIGINT, IN nextSrcAmountIn BIGINT, IN nextDstAmountIn BIGINT, OUT result INT, OUT transactionTime DATETIME)
proc_label:BEGIN
	DECLARE prevSrcAmount BIGINT;
	DECLARE nextSrcAmount BIGINT;
	DECLARE prevDstAmount BIGINT;
	DECLARE nextDstAmount BIGINT;
	DECLARE lastInsertId BIGINT;

	SET result = -1;

	SELECT cashAmount INTO prevSrcAmount FROM AccountTbl WHERE accountId = srcAccountIdIn;
	SELECT cashAmount INTO prevDstAmount FROM AccountTbl WHERE accountId = dstAccountIdIn;

	SET nextSrcAmount = prevSrcAmount - amount;

	IF (nextSrcAmount != nextSrcAmountIn) THEN
		SET result = 1;
		LEAVE proc_label;
	END IF;

	SET nextDstAmount = prevDstAmount + amount;
	IF (nextDstAmount != nextDstAmountIn) THEN
		SET result = 2;
		LEAVE proc_label;
	END IF;

	SET transactionTime = CURRENT_TIMESTAMP;

	START TRANSACTION;
	UPDATE AccountTbl SET cashAmount = nextSrcAmount WHERE accountId = srcAccountIdIn;
	UPDATE AccountTbl SET cashAmount = nextDstAmount WHERE accountId = dstAccountIdIn;
	INSERT INTO SendInfoTbl(srcAccountId, dstAccountId, amount) VALUES (srcAccountIdIn, dstAccountIdIn, amount);
	SET lastInsertId = LAST_INSERT_ID();
	INSERT INTO AccountHistoryTbl(cashAmount, infoType, infoId, accountId, created) VALUES (-amount, 1, lastInsertId, srcAccountIdIn, transactionTime);
	INSERT INTO AccountHistoryTbl(cashAmount, infoType, infoId, accountId, created) VALUES (amount, 1, lastInsertId, dstAccountIdIn, transactionTime);
	COMMIT;

	SET result = 0;
END