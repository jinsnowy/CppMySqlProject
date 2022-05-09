#pragma once
class PathManager
{
private:
	std::string mRoot;
	static std::unique_ptr<PathManager> mInst;

private:
	PathManager()
	{
		mRoot = SOLUTION_DIR;
	}

public:
	static void Initialize()
	{
		mInst.reset(new PathManager());
	}

	static std::string GetPath(const std::string& filePath)
	{
		return mInst->mRoot + filePath;
	}
};

