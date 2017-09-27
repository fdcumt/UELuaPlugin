#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogMyProject, Log, All);

class MYPROJECT_API FMyTest1
{
public:
	static FMyTest1* CreateSelf();
	int32 GetMember() const { return m_member; }
	void SetMember(int32 m);
	void Print() const;

private:
	int32 m_member;
};

class MYPROJECT_API FMyTest
{
public:
	static FMyTest* CreateSelf();
	FMyTest1* GetMember() const { return m_member; }
	void SetMember( FMyTest1 *InTest1);
	void Print() const ;

	static void StaticPrint();
	static void StaticPrint1();

private:
	FMyTest1 *m_member;
};
