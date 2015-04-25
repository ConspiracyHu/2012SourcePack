#include "EventHandler.h"

void glErrorCheck()
{
	switch (glGetError())
	{
	case GL_INVALID_VALUE:
		MessageBox(0,"GL_INVALID_VALUE\0","ERROR",MB_OK);
		break;
	case GL_INVALID_OPERATION:
		MessageBox(0,"GL_INVALID_OPERATION\0","ERROR",MB_OK);
		break;
	case GL_STACK_OVERFLOW:
		MessageBox(0,"GL_STACK_OVERFLOW\0","ERROR",MB_OK);
		break;
	case GL_STACK_UNDERFLOW:
		MessageBox(0,"GL_STACK_UNDERFLOW\0","ERROR",MB_OK);
		break;
	case GL_OUT_OF_MEMORY:
		MessageBox(0,"GL_OUT_OF_MEMORY\0","ERROR",MB_OK);
		break;
	}
}

#ifdef MINIMALTWEAKER
int MaxPass=-1;
#endif

float AspectRatio=4/3.0f;
bool CropAspect=false;

float IntroAspect=16/9.0f;
int IntroXRes=0;
int IntroYRes=0;
int IntroXOff=0;
int IntroYOff=0;

int ToolYoff=(int)((600-(800/IntroAspect))/2.0f);

void DisplayFrame(int Frame, MATERIAL *MaterialList, SCENE *SceneList, WORLD *WorldList, EVENT *EventList, int EventNum)
{
	EyeShake=V3_Make(0,0,0);
	TargetShake=V3_Make(0,0,0);
	for (int x=0; x<EventNum; x++)
	{
		EVENT *e=&EventList[x];
#ifdef MINIMALTWEAKER
		if (MaxPass==-1 || e->Pass<=MaxPass)
#endif
		if (Frame>=e->StartFrame && Frame<=e->EndFrame)
		{
			/*WriteDebug("Frame: %.5d Pass: %d Effect: ",Frame,e->Pass);
			switch (e->EventType)
			{
			case 0:
				WriteDebug("No Effect.\n");
				break;
			case 1:
				WriteDebug("2D layer.\n");
				break;
			case 2:
				WriteDebug("Particle Calc.\n");
				break;
			case 3:
				WriteDebug("Clear.\n");
				break;
			case 4:
				WriteDebug("R2TXT.\n");
				break;
			case 5:
				WriteDebug("Feedback.\n");
				break;
			case 6:
				WriteDebug("Blur.\n");
				break;
			case 7:
				WriteDebug("3D scene.\n");
				break;
			case 8:
				WriteDebug("Particle Reset.\n");
				break;
			}*/
			//glDisable(GL_LIGHTING);
			//glColor4f(1,1,1,1);
			SwitchTo2D();
			DEFAULTEVENTDATA d;
			d.TimePos=(Frame-e->StartFrame)/(float)(e->EndFrame-e->StartFrame);
			d.StartFrame=e->StartFrame;

			float ex1=LinearInterpolate((float)e->sx1,(float)e->ex1,d.TimePos);
			float ex2=LinearInterpolate((float)e->sx2,(float)e->ex2,d.TimePos);
			float ey1=LinearInterpolate((float)e->sy1,(float)e->ey1,d.TimePos)-ToolYoff+1;
			float ey2=LinearInterpolate((float)e->sy2,(float)e->ey2,d.TimePos)-ToolYoff+1;

			float XMult=1;
			float YMult=1;
			float XOff=0;
			float YOff=0;

			if (AspectRatio>IntroAspect)
			{
				//x iranyban kell osszehuzni a kepet hogy az y-t kitoltse
				//XMult=ScreenAspect*4/3.0f;
				XMult=IntroAspect/AspectRatio;
				XOff=(XRes-XRes*XMult)/2.0f;
			}
			else
			{
				//y iranyban kell osszehuzni a kepet hogy az x-t kitoltse
				YMult=AspectRatio/IntroAspect;
				YOff=(YRes-YRes*YMult)/2.0f;
			}

			d.x1=(int)((ex1/799.0f)*XRes*XMult+XOff);
			d.x2=(int)((ex2/799.0f)*XRes*XMult+XOff);

			d.y1=(int)((ey1/(800.0f/IntroAspect))*YRes*YMult+YOff);
			d.y2=(int)((ey2/(800.0f/IntroAspect))*YRes*YMult+YOff);

			d.OnScreenInLastFrame=e->OnScreenInLastFrame;
			d.CurrentFrame=Frame*10;
			d.Aspect=(ex2-ex1)/(ey2-ey1);

			glPushAttrib(GL_ALL_ATTRIB_BITS);
			e->Effect(&d,e->EffectData);
			glPopAttrib();
			
			e->OnScreenInLastFrame=true;
		}
		else		
			e->OnScreenInLastFrame=false;
	}	
	//glErrorCheck();
}