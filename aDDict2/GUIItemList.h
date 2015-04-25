#ifndef __GUIITEMLIST__
#define __GUIITEMLIST__

#include "memtrack.h"

class cINTERFACE;
class ITEM;
typedef void (WINAPI *GUICALLBACK) (cINTERFACE*, int);

struct CHILD
{
	ITEM *Child;
	CHILD *Next;
};

class ITEM
{
public:
	char *Name;
	void *Data;
	ITEM *Next,*Prev,*SubItems,*LastSub;
	CHILD *ParentList,*ChildList;
	bool Selected,OldSelected;
	bool Hidden;
	bool SubOpened;
	int SubItemNum;
	int Num;
	cINTERFACE *SelectedInterface;
	GUICALLBACK OnLeftClick,OnRightClick,OnMiddleClick;
	int ID;
	ITEM() {
		Name=""; 
		Data=NULL; 
		Next=NULL; 
		Prev=NULL; 
		SubItems=NULL;
		LastSub=NULL;
		Selected=false; 
		SubOpened=false; 
		Hidden=false;
		ChildList=NULL; 
		ParentList=NULL;
		SubItemNum=0;
		SelectedInterface=NULL;
	}
	ITEM *AddSubItem(char *Name, void *Data, GUICALLBACK OnLeft, GUICALLBACK OnMiddle, GUICALLBACK OnRight)
	{
		ITEM *Buffer = new ITEM();
		memset(Buffer,0,sizeof(ITEM));
		Buffer->Prev=LastSub;
		Buffer->Name=Name;
		Buffer->Data=Data;
		Buffer->OnLeftClick=OnLeft;
		Buffer->OnRightClick=OnRight;
		Buffer->OnMiddleClick=OnMiddle;
		
		SubItemNum++;

		if (LastSub) 
		{
			Buffer->Num=LastSub->Num+1;
			LastSub->Next=Buffer; 
		}
		else 
		{
			Buffer->Num=0;
			SubItems=Buffer;
		}
		LastSub=Buffer;
		return Buffer;
	}
	void AddChild(ITEM *Item)
	{
		CHILD *c=new CHILD;
		c->Next=ChildList;
		c->Child=Item;
		ChildList=c;

		c=new CHILD;
		c->Next=Item->ParentList;
		c->Child=this;
		Item->ParentList=c;
	}
};

class ITEMLIST
{
public:
	ITEM *List,*Last;

	int ItemNum;

	ITEMLIST() {List = NULL; Last = NULL; ItemNum=0;}
	ITEM *AddItem(char *Name, void *Data, GUICALLBACK OnLeft, GUICALLBACK OnMiddle, GUICALLBACK OnRight)
	{
		ITEM *Buffer = new ITEM();
		memset(Buffer,0,sizeof(Buffer));

		Buffer->Prev=Last;
		Buffer->Name=Name;
		Buffer->Data=Data;
		Buffer->OnLeftClick=OnLeft;
		Buffer->OnRightClick=OnRight;
		Buffer->OnMiddleClick=OnMiddle;

		ItemNum++;

		if (Last) 
		{
			Buffer->Num=Last->Num+1;
			Last->Next=Buffer; 
		}
		else 
		{
			Buffer->Num=0;
			List=Buffer;
		}
		Last=Buffer;
		return Buffer;
	}
	void DeleteSelected()
	{

	}
};

#endif