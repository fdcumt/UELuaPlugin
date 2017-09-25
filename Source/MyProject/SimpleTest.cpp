#include "SimpleTest.h"
DEFINE_LOG_CATEGORY(LogMyProject);

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
