#include "BaseLuaWidget.h"
#include "LuaUtil.h"
#include "MyProjectGameMode.h"

UBaseLuaWidget::UBaseLuaWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{

}

bool UBaseLuaWidget::Initialize()
{
	UUserWidget::Initialize();
	UE_LOG(LogProject, Log, TEXT("UBaseLuaWidget::Initialize"));
	UBaseLuaWidget *pSelf = this;
	FLuaUtil::CallR(
		FLuaFuncName("Init"),
		FLuaClassType<UBaseLuaWidget*>(pSelf, "UBaseLuaWidget")
	);

	return true;
}
