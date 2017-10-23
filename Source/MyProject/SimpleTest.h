#pragma once
#include "SimpleTest.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyProject, Log, All);

UCLASS()
class MYPROJECT_API UMyTestClass : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	int32 mVariable;

};

UCLASS()
class MYPROJECT_API UMyTestClass1 : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	DEPRECATED(4.11, "This property is deprecated. Please use AnimClass instead")
	UPROPERTY(BlueprintReadOnly, Category = Animation, meta = (DeprecationMessage = "This property is deprecated. Please use AnimClass instead"))
		class UMyTestClass *myTestClass;
};

class MYPROJECT_API FMyTestBase
{
public:
	static FMyTestBase* CreateSelf();
	float GetMember() const { return m_member; }
	void SetMember(float m);
	void Print() const;
	void TestBaseFunc() const;
public:
	int32 mVariable;

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
	static void GetPlayerController();



private:
	FMyTest1 *m_member;
};


USTRUCT(BlueprintType)
struct FBaseStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStruct)
		int32 m;
};

USTRUCT(BlueprintType)
struct FBaseStruct1
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStruct1)
		FBaseStruct m_Struct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStruct1)
		int32 m_Value1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStruct1)
		int32 m_Value2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseStruct1)
		TArray<FBaseStruct> m_BaseStructs;
};
