#include "BaseLuaWidget.h"
#include "LuaUtil.h"
#include "MyProjectGameMode.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"

UBaseLuaWidget::UBaseLuaWidget(const FObjectInitializer& ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{

}

bool UBaseLuaWidget::Initialize()
{
	UUserWidget::Initialize();

	UE_LOG(LogProject, Log, TEXT("UBaseLuaWidget::Initialize"));

	if ( g_LuaState)
	{
		if (UWidget* pWidget = GetWidget("b"))
		{
			UTextBlock *pTextBlock = Cast<UTextBlock>(pWidget);
			pTextBlock->SetText(FText::FromString("abcdef"));
			if (pTextBlock)
			{
				UBaseLuaWidget *pBaseLuaWidget = this;
				FLuaUtil::CallR(
					FLuaFuncName("Init"),
					FLuaClassType<UBaseLuaWidget*>(pBaseLuaWidget, "UBaseLuaWidget")
				);
			}
		}
	}

	return true;
}

UWidget* UBaseLuaWidget::GetWidget(const FName& Name) const
{
	return WidgetTree->FindWidget(Name);
}
