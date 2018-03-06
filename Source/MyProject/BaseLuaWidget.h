#pragma once
#include "UserWidget.h"
#include "BaseLuaWidget.generated.h"

UCLASS()
class MYPROJECT_API UBaseLuaWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UBaseLuaWidget(const FObjectInitializer& ObjectInitializer);

public:
	virtual bool Initialize();

public:
	UFUNCTION(BlueprintCallable)
	UWidget* GetWidget(const FName& Name) const;


};
