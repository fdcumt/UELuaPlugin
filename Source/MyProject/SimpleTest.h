#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogMyProject, Log, All);

class MYPROJECT_API FMyTest
{
public:
	int32 GetMember() const { return m_member; }
	void SetMember(int32 m);
	void Print() const ;
	static void StaticPrint();

private:
	int32 m_member;
};
