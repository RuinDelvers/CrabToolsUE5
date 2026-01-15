#pragma once

#include "SGraphNode.h"
#include "SGraphPin.h"


class STATEMACHINEEDITOR_API SEdStateNode : public SGraphNode
{
protected:

	TSharedPtr<SErrorText> ErrorText;

public:

	SLATE_BEGIN_ARGS(SEdStateNode) {}
	SLATE_END_ARGS()

	virtual ~SEdStateNode();

	void Construct(const FArguments& InArgs, UEdBaseStateNode* InNode);
	
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;
	virtual EVisibility GetDragOverMarkerVisibility() const;
	virtual void UpdateGraphNode() override;
	virtual const FSlateBrush* GetNameIcon() const;

	virtual void GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) override;

	FName GetStateName() const;
	UEdBaseStateNode* GetStateNode() const { return Cast<UEdBaseStateNode>(this->GraphNode); }

private:

	bool OnVerifyNameTextChanged(const FText& InText, FText& ErrorMessage);

	//bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	void OnNodeNameChanged(FName OldName, FName Name);

	void OnErrorTextUpdate(FText ErrText);
	void OnAttemptRename();
};

