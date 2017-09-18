#pragma once

class FSimple
{
public:
	FSimple() {}
	~FSimple() {}

public:
	void SetAge(int32 InAge) { return m_Age; }
	int32 GetAge() { return m_Age; }

private:
	int32 m_Age;
};
