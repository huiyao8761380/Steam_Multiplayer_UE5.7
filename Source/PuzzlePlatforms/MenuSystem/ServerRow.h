//@009 九. 服务器列表行 Widget
//     功能: 在主菜单服务器列表中展示单个服务器信息（名称/玩家数/房主），点击选中后加入

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

// 服务器列表单行 Widget：显示服务器信息并向主菜单回传选中事件
UCLASS()
class PUZZLEPLATFORMS_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerName;		//(1) 服务器名称文本

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUser;		//(2) 房主用户名文本

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ConnectionFraction;		//(3) 玩家数/最大玩家数文本

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;		//(4) 是否被选中

	void Setup(class UMainMenu* Parent, uint32 Index);		//(5) 初始化父菜单和索引

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;		//(6) 行按钮

	UPROPERTY()
	class UMainMenu* Parent;		//(7) 父菜单指针
	
	uint32 Index;		//(8) 在列表中的索引

	UFUNCTION()
	void OnClicked();		//(9) 点击回调

};
