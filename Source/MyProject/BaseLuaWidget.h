#pragma once
#include "UserWidget.h"
#include "BaseLuaWidget.generated.h"

UCLASS()
class MYPROJECT_API UBaseLuaWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UBaseLuaWidget(const FObjectInitializer& ObjectInitializer);

	virtual bool Initialize();

};
