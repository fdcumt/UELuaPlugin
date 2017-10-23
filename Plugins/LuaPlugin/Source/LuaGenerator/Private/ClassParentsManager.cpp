#include "ClassParentsManager.h"
#include "GeneratorDefine.h"
#include "IScriptGenerator.h"
#include "ScriptGeneratorManager.h"

using namespace NS_LuaGenerator;

FClassParentManager::FClassParentManager()
	: m_ParentClassMatrix(nullptr)
	, m_ClassNum(0)
	, m_FirstAvailableIndex(0)
{

}

FClassParentManager::~FClassParentManager()
{

}

void FClassParentManager::Init(const TArray<IScriptGenerator*> &ClassGenerators)
{
	int32 ClassNum = ClassGenerators.Num();

	if (ClassNum <= 0)
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("FClassParentManager::Init num is:%d, cannot init!"), ClassNum);
		return;
	}

	m_ClassNum = ClassNum;
	CreateParentMatrix();
	InitParentMatrix(ClassGenerators);
	InitParentIndex();
	DestoryParentMatrix();
}

TArray<FString> FClassParentManager::GetParentClassNames(const FString &ClassName)
{
	TArray<FString> ParentNames;
	int32 *pIndex = m_ClassName2Index.Find(ClassName);
	if (pIndex)
	{
		for ( int32 ParentIndex : m_ParentIndexs[*pIndex])
		{
			FString *pClasName = m_Index2ClassName.Find(ParentIndex);

			if (pClasName)
			{
				ParentNames.Add(*pClasName);
			}
			else
			{
				UE_LOG(LogLuaGenerator, Error, TEXT("FClassParentManager::GetParentClassNames not find %s`s parent, parent index %d"), *ClassName, ParentIndex);
			}
		}
	}
	else
	{
		UE_LOG(LogLuaGenerator, Error, TEXT("GetParentClassNames error, not find class:%s"), *ClassName);
	}

	return ParentNames;
}

int32 FClassParentManager::GetClassIndex(const FString &InClassName)
{
	int32 *pIndex = m_ClassName2Index.Find(InClassName);
	if (pIndex)
	{
		return *pIndex;
	}
	else
	{
		if (g_ScriptGeneratorManager->ContainClassName(InClassName))
		{
			m_ClassName2Index.Add(InClassName, m_FirstAvailableIndex);
			m_Index2ClassName.Add(m_FirstAvailableIndex, InClassName);
			++m_FirstAvailableIndex;
			return m_FirstAvailableIndex-1;
		}
		else
		{
			UE_LOG(LogLuaGenerator, Error, TEXT("FClassParentManager::GetClassIndex class name:%s is not exist!"), *InClassName);
			return -1;
		}
	}
}

void FClassParentManager::InitParentIndex()
{
	for (int32 ClassIndex=0; ClassIndex<m_ClassNum; ++ClassIndex)
	{
		TArray<int32> ParentIndexs;
		for (int32 ParentIndex=0; ParentIndex<m_ClassNum; ++ParentIndex)
		{
			if (m_ParentClassMatrix[ClassIndex][ParentIndex])
			{
				ParentIndexs.Add(ParentIndex);
			}
		}
		m_ParentIndexs.Add(ParentIndexs);
	}
}

void FClassParentManager::CreateParentMatrix()
{
	m_ParentClassMatrix = new bool *[m_ClassNum];
	for (int32 i = 0; i < m_ClassNum; ++i)
	{
		m_ParentClassMatrix[i] = new bool[m_ClassNum];

		for (int32 j = 0; j < m_ClassNum; ++j)
		{
			m_ParentClassMatrix[i][j] = false;
		}
	}
}

void FClassParentManager::InitParentMatrix(const TArray<IScriptGenerator*> &ClassGenerators)
{
	for (IScriptGenerator *const pGenerator : ClassGenerators)
	{
		FString GeneratorClassName = pGenerator->GetKey();
		int32 GeneratorIndex = GetClassIndex(GeneratorClassName);
		TArray<FString> ParentNames;
		pGenerator->GetParentNames(ParentNames);

		for (const FString &ParentName : ParentNames)
		{
			int32 ParentIndex = GetClassIndex(ParentName);
			m_ParentClassMatrix[GeneratorIndex][ParentIndex] = true;
		}
	}

	// flody algorithm, cal is reachable
	for (int32 k=0; k<m_ClassNum; ++k)
	{
		for (int32 i=0; i<m_ClassNum; ++i)
		{
			for (int32 j=0; j<m_ClassNum; ++j)
			{
				m_ParentClassMatrix[i][j] = m_ParentClassMatrix[i][j] || (m_ParentClassMatrix[i][k] && m_ParentClassMatrix[k][j]);
			}
		}
	}
}

void FClassParentManager::DestoryParentMatrix()
{
	for (int32 i = 0; i < m_ClassNum; ++i)
	{
		delete []m_ParentClassMatrix[i];
	}

	delete[]m_ParentClassMatrix;
}
