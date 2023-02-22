//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef VGUI_HELPERS_H
#define VGUI_HELPERS_H
#ifdef _WIN32
#pragma once
#endif


#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>


inline int PanelTop(vgui::Panel *pPanel)	{int x,y,w,h; pPanel->GetBounds(x,y,w,h); return y;}
inline int PanelLeft(vgui::Panel *pPanel)	{int x,y,w,h; pPanel->GetBounds(x,y,w,h); return x;}
inline int PanelRight(vgui::Panel *pPanel)	{int x,y,w,h; pPanel->GetBounds(x,y,w,h); return x+w;}
inline int PanelBottom(vgui::Panel *pPanel)	{int x,y,w,h; pPanel->GetBounds(x,y,w,h); return y+h;}
inline int PanelWidth(vgui::Panel *pPanel)	{int x,y,w,h; pPanel->GetBounds(x,y,w,h); return w;}
inline int PanelHeight(vgui::Panel *pPanel)	{int x,y,w,h; pPanel->GetBounds(x,y,w,h); return h;}

// Places child at the requested position inside pParent. iAlignment is from Label::Alignment.
void AlignPanel(vgui::Panel *pChild, vgui::Panel *pParent, int alignment);


#define DeclarePanel(className,panelClassName,globalPanel)\
    class I##className\
    {\
    public:\
        virtual void        Create( vgui::VPANEL parent ) = 0;\
        virtual void        Destroy( void ) = 0;\
    };\
    class className : public I##className\
    {\
    private:\
        panelClassName *myPanel;\
    public:\
        className(void)\
        {\
            myPanel = NULL;\
        }\
        void Create( vgui::VPANEL parent )\
        {\
            myPanel = new panelClassName( parent );\
        }\
        void Destroy( void )\
        {\
            if(myPanel)\
            {\
                myPanel->SetParent( (vgui::Panel *)NULL );\
                delete myPanel;\
            }\
        }\
    };\
    extern I##className *globalPanel
//Allows access to the panel through globalPanel
#define DeclareAccessiblePanel(className,panelClassName,globalPanel)\
    class I##className\
    {\
    public:\
        virtual void        Create( vgui::VPANEL parent ) = 0;\
        virtual void        Destroy( void ) = 0;\
        virtual vgui::Panel *GetPanel(void) = 0;\
    };\
    class className : public I##className\
    {\
    private:\
        panelClassName *myPanel;\
    public:\
        className(void)\
        {\
            myPanel = NULL;\
        }\
        void Create( vgui::VPANEL parent )\
        {\
            myPanel = new panelClassName( parent );\
        }\
        vgui::Panel *GetPanel(void)\
        {\
            return myPanel;\
        }\
        void Destroy( void )\
        {\
            if(myPanel)\
            {\
                myPanel->SetParent( (vgui::Panel *)NULL );\
                delete myPanel;\
            }\
        }\
    };\
    extern I##className *globalPanel
//Doesn't use panelClassName but has it to base code on copy and paste work
#define PanelGlobals(className,panelClassName,globalPanel)\
    static className g_##className##Panel;\
    I##className *globalPanel = (I##className *)&g_##className##Panel

#define ToggleVisibility(panel)\
    panel->SetVisible(!panel->IsVisible())

//only the prototype for AlignPanel exists
#define CenterThisPanelOnScreen()\
    int x,w,h;\
    GetBounds(x,x,w,h);\
    SetPos((ScreenWidth()-w)/2,(ScreenHeight()-h)/2)
#define CenterPanelOnScreen(panel)\
    int x,w,h;\
    panel->GetBounds(x,x,w,h);\
    panel->SetPos((panel->ScreenWidth()-w)/2,(panel->ScreenHeight()-h)/2)









#endif // VGUI_HELPERS_H

