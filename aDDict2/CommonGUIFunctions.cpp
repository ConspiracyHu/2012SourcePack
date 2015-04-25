#include "CommonGUIFunctions.h"

void __stdcall GUI_CheckByteSwitch(cINTERFACE* Interface, int ButtonId)
{
	Interface->ItemList[ButtonId].ButtonPushed=(*(bool*)Interface->ItemList[ButtonId].data);
}


void __stdcall Scroller_SelectItem(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=
		
		(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)
		
		/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
}

int CountSubItems(ITEM *List)
{
	int i=0;
	if (List->SubOpened) i+=List->SubItemNum;
	ITEM *SubItemList=List->SubItems;
	while (SubItemList)
	{
		i+=CountSubItems(SubItemList);
		SubItemList=SubItemList->Next;
	}

	return i;
}

void __stdcall SubOpenClose(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	if (ScrollerInterface->ClickedItem)
	{
		bool a=ScrollerInterface->ClickedItem->SubOpened;
		ScrollerInterface->ClickedItem->SubOpened=true;
		int sub=CountSubItems(ScrollerInterface->ClickedItem);
		ScrollerInterface->ClickedItem->SubOpened=a;
		if (a) ScrollerInterface->ItemList[ScrollerID].ItemNum-=sub; else 
			   ScrollerInterface->ItemList[ScrollerID].ItemNum+=sub;
		if (ScrollerInterface->ClickedNum<ScrollerInterface->ItemList[ScrollerID].SelectedItem)
		{
			if (a) ScrollerInterface->ItemList[ScrollerID].SelectedItem-=sub; else 
				ScrollerInterface->ItemList[ScrollerID].SelectedItem+=sub;

		}
		if (ScrollerInterface->ClickedItem->SubItemNum)
			ScrollerInterface->ClickedItem->SubOpened=!ScrollerInterface->ClickedItem->SubOpened;
		WaitMiddleButton=true;
	}
}


ITEM *ScrollerFindItem(ITEMLIST *List, int Number)
{
	ITEM *ItemList=List->List;
	for (int x=0; x<Number; x++) if (ItemList) ItemList=ItemList->Next;
	return ItemList;
}

void __stdcall Scroller_SelectOnlyThis(cINTERFACE* ScrollerInterface, int ScrollerID)
{
	ScrollerInterface->ItemList[ScrollerID].SelectedItem=(min(max(my,ScrollerInterface->ItemList[ScrollerID].y1),ScrollerInterface->ItemList[ScrollerID].y2-11)-ScrollerInterface->ItemList[ScrollerID].y1)/11+ScrollerInterface->ItemList[ScrollerID].StartItem;
}

void SetupFileList(ITEMLIST *List, char *FileMask)
{
	tfilelist *FileList=findfile(FileMask);
	for (tfilelist *Buffer=FileList;Buffer;Buffer=Buffer->next)
	{
		List->AddItem(Buffer->filedata.cFileName,NULL,Scroller_SelectOnlyThis,nop,nop);
	}
}