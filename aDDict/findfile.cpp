#include "findfile.h"

tfilelist *findfile(const char *path)
{
	BOOL fFinished = FALSE;

	tfilelist *list = NULL;
	tfilelist *newlist = NULL;
	tfilelist *check = NULL;
	tfilelist *check2 = NULL;

	HANDLE hSearch;
	WIN32_FIND_DATA FileData; 

	hSearch = FindFirstFile(path, &FileData); 
	if (hSearch != INVALID_HANDLE_VALUE) 
	{ 
		while (!fFinished)
		{
			if (StrCmp(FileData.cFileName,".") && StrCmp(FileData.cFileName,".."))
			{
				newlist = new tfilelist[1];
				memset(newlist,0,sizeof(tfilelist));
				newlist->filedata=FileData;
				
				check = list;
				check2 = list;

				while ( (check!=NULL) && ( StrCmp(check->filedata.cFileName,FileData.cFileName)<0 ))
				{
					check2=check;
					check=check->next;
				}

				if (list == NULL)
				{
					newlist->next=NULL;
					list=newlist;
				}
				else
					if (check!=check2)
					{
						newlist->next=check2->next;
						check2->next=newlist;		
					}
					else
					{
						newlist->next=list;
						list=newlist;
					}
			}
			fFinished = !FindNextFile(hSearch, &FileData);

		}

	}

	return list;
}

int filenum(tfilelist *list)
{
	tfilelist *buffer=list;
	int cntr = 0;
	while (list!=NULL)
	{
		cntr++;
		list=list->next;
	}
	return cntr;
}