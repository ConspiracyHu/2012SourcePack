#include "aDDict2GUI.h"

cINTERFACE MainGUI;

void glErrorCheck()
{
	switch (glGetError())
	{
	case GL_INVALID_ENUM:
		ErrorMessage="GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		ErrorMessage="GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		ErrorMessage="GL_INVALID_OPERATION";
		break;
	case GL_STACK_OVERFLOW:
		ErrorMessage="GL_STACK_OVERFLOW";
		break;
	case GL_STACK_UNDERFLOW:
		ErrorMessage="GL_STACK_UNDERFLOW";
		break;
	case GL_OUT_OF_MEMORY:
		ErrorMessage="GL_OUT_OF_MEMORY";
		break;
	}
}

void __stdcall GUI_ButtonSelect_ParticleFreeze(cINTERFACE* Interface, int ButtonId)
{
	GUI_ButtonSelect(Interface,ButtonId);
	for (WORLD *w=Worlds; w;w=w->Next)
	{
		for (int a=0; a<w->ObjectNum; a++)
		{
			if (w->ObjectList[a].EmitterData)
				w->ObjectList[a].EmitterData->LastFrameChecked=timeGetTime();
		}
	}
	LastTime=timeGetTime();
}

void __stdcall GUI_OnWheelPushAttrib(cINTERFACE* Interface, int ButtonId)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
}

void __stdcall GUI_OnWheelPopAttrib(cINTERFACE* Interface, int ButtonId)
{
	glPopAttrib();
}

void InitGUI()
{
	BuildFont();

	glClearColor(backgroundcol);
	glClear(0x4100);
	glColor4f(1,1,1,1);
	glRasterPos2f(-0.07f,0);
	glPrint("INITIALIZING...",base);
	SwapBuffers(hDC);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL,0);
	glEnable(GL_NORMALIZE);
	InitGui();
	cINTERFACE *IntroEditor,*TexGen,*Modeller,*Keyframer,*MainFileMenu;

	
	IntroEditor=MainGUI.AddSkinnedButton(10,6,IntroEditorMenu,"INTRO EDITOR AND TIMELINER TOOL",GUI_ButtonSelect_ParticleFreeze,nop,nop,nop);
	TexGen=MainGUI.AddSkinnedButton(117,6,TextureGeneratorMenu,"TEXTURE GENERATOR TOOL",GUI_ButtonSelect_ParticleFreeze,nop,nop,nop);
	Modeller=MainGUI.AddSkinnedButton(180,6,ModellerMenu,"MODELLER TOOL",GUI_ButtonSelect_ParticleFreeze,nop,nop,nop);
	Modeller->ParentItem->OnWheel=GUI_OnWheelPushAttrib;
	Modeller->ParentItem->OnWheelAfter=GUI_OnWheelPopAttrib;
	Keyframer=MainGUI.AddSkinnedButton(258,6,KeyframerMenu,"LAYOUTER TOOL",GUI_ButtonSelect_ParticleFreeze,nop,nop,nop);
	MainFileMenu=MainGUI.AddSkinnedButton(342,6,FileMenu,"FILE MENU",GUI_ButtonSelect_ParticleFreeze,nop,nop,nop);

	MainGUI.AddImage(880-26,2,aDDictLogo);
	
	MainGUI.SetActiveBranch(4);
	MainGUI.ItemList[4].ButtonPushed=true;

	BuildIntroEditorGUI(IntroEditor);
	BuildTexGenGUI(TexGen);
	BuildModellerGUI(Modeller);	
	BuildLayouterGUI(Keyframer);
	BuildFileMenuGui(MainFileMenu);

}

