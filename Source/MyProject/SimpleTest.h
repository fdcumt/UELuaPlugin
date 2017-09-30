#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogMyProject, Log, All);

class MYPROJECT_API FMyTestBase
{
public:
	static FMyTestBase* CreateSelf();
	float GetMember() const { return m_member; }
	void SetMember(float m);
	void Print() const;
	void TestBaseFunc() const;

private:
	float m_member;
};

class MYPROJECT_API FMyTest1 :public FMyTestBase
{
public:
	static FMyTest1* CreateSelf();
	int32 GetMember() const { return m_member; }
	void SetMember(int32 m);
	void Print() const;

private:
	int32 m_member;
};

class MYPROJECT_API FMyTest : public FMyTest1
{
public:
	static FMyTest* CreateSelf();

	void TestConstPoint(const FMyTest1 *InTest) const;
	void TestPointRef(FMyTest1 *&InTest) const;
	void TestConstRef(const FMyTest1 &InTest) const;
	void TestRef(FMyTest1 &InTest) const;


	static void StaticPrint();
	static void StaticPrint1();

private:
	FMyTest1 *m_member;
};
