#pragma once

template<typename T>
class KeyInfo
{
private:
	T mKey;

public:
	KeyInfo(const T& _key)
		:
		mKey(_key)
	{}

	virtual ~KeyInfo() = default;

	const T& GetKey() const { return mKey; }
};

