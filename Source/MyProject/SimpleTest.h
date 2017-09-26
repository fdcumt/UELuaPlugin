#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogMyProject, Log, All);

class MYPROJECT_API FMyTest
{
public:
	static FMyTest* CreateSelf();
	int32 GetMember() const { return m_member; }
	void SetMember(int32 m);
	void Print() const ;
	static void StaticPrint();
	static void StaticPrint1();

private:
	int32 m_member;
};
