#include "SimpleTest.h"
DEFINE_LOG_CATEGORY(LogMyProject);

FMyTest* FMyTest::CreateSelf()
{
	return new FMyTest();
}

void FMyTest::SetMember(int32 m)
{
	m_member = m;
}

void FMyTest::Print() const
{
	UE_LOG(LogMyProject, Log, TEXT("class memeber m_member is:%d"), m_member);
}

void FMyTest::StaticPrint()
{
	UE_LOG(LogMyProject, Log, TEXT("StaticPrint"));
}

void FMyTest::StaticPrint1()
{
	UE_LOG(LogMyProject, Log, TEXT("StaticPrint111"));
}
