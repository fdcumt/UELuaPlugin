#pragma once

class IScriptGenerator;

class FClassParentManager
{
public:
	FClassParentManager();
	~FClassParentManager();

public:
	void Init(const TArray<IScriptGenerator*> &ClassGenerators);
	TArray<FString> GetParentClassNames(const FString &ClassName);

private:
	int32 GetClassIndex(const FString &InClassName);
	void InitParentIndex();
	void CreateParentMatrix();
	void InitParentMatrix(const TArray<IScriptGenerator*> &ClassGenerators);
	void DestoryParentMatrix();

private:
	int32 m_ClassNum;
	TMap<FString, int32> m_ClassName2Index;
	TMap<int32, FString> m_Index2ClassName;
	bool **m_ParentClassMatrix;
	int32 m_FirstAvailableIndex;
	TArray<TArray<int32>> m_ParentIndexs;
};

