//The following include files are necessary to allow your ModelPanel.cpp to compile.
#include "cbase.h"
#include "model_panel.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/TextEntry.h>

#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Image.h>
#include <vgui_controls/Controls.h>
#include <vgui/ISurface.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/FocusNavGroup.h>
#include <vgui_controls/FileOpenDialog.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/ImagePanel.h>

#include "hl2mp_gamerules.h"
#include "c_team.h"

#include <stdio.h>
#include <assert.h>
#include <utlvector.h>
#include <vstdlib/IKeyValuesSystem.h>
#include <ctype.h>	// isdigit()

#include <materialsystem/imaterial.h>

#include <vgui/IBorder.h>
#include <vgui/IInput.h>
#include <vgui/IPanel.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/ILocalize.h>
#include <vgui/IVGui.h>
#include <KeyValues.h>
#include <vgui/MouseCode.h>

#include <vgui_controls/Panel.h>
#include <vgui_controls/BuildGroup.h>
#include <vgui_controls/Tooltip.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/Controls.h>
#include "vgui_controls/Menu.h"
#include "vgui_controls/MenuItem.h"

#include "UtlSortVector.h"

#include "tier1/utldict.h"
#include "tier1/utlbuffer.h"
#include "mempool.h"
#include "filesystem.h"
#include "tier0/icommandline.h"
#include "tier0/minidump.h"

#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>


//CModelPanel class: Tutorial example class
class CModelPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CModelPanel, vgui::Frame);
	//CModelPanel : This Class / vgui::Frame : BaseClass

	CModelPanel(vgui::VPANEL parent); 	// Constructor
	~CModelPanel() {};				// Destructor

protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);
private:
	//Other used VGUI control Elements:
	Button* m_pCloseButton;
	Button* m_pSpectateButton;
	Button* m_pJoinButton;

	Button* m_pModelButton1;
	Button* m_pModelButton2;
	Button* m_pModelButton3;
	Button* m_pModelButton4;
	Button* m_pModelButton5;
	Button* m_pModelButton6;
	Button* m_pModelButton7;
	Button* m_pModelButton8;
	Button* m_pModelButton9;
	Button* m_pModelButton10;
	Button* m_pModelButton11;
	Button* m_pModelButton12;
	Button* m_pModelButton13;
	Button* m_pModelButton14;
	Button* m_pModelButton15;
	Button* m_pModelButton16;
	Button* m_pModelButton17;
	Button* m_pModelButton18;
	Button* m_pModelButton19;
	Button* m_pModelButton20;
	Button* m_pModelButton21;


	vgui::ImagePanel* m_pModelImage;
	vgui::ImagePanel* m_pModelImage2;


public:
	void MoveToCenterOfScreen();
};

// Constuctor: Initializes the Panel
CModelPanel::CModelPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "ModelPanel")
{


	m_pModelImage = new ImagePanel(this, "ModelImage");
	m_pModelImage2 = new ImagePanel(this, "ModelImage2");



	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetVisible(true);



	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/UI/modelpanel.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	DevMsg("ModelPanel has been constructed\n");


	m_pCloseButton = new Button(this, "Button", "Cancel", this, "turnoff");
	m_pSpectateButton = new Button(this, "Button", "Spectate", this, "spectate");
	m_pJoinButton = new Button(this, "Button", "Join Game", this, "joingame");


	m_pModelButton1 = new Button(this, "Player_Model_male_01", "&", this, "Male01");
	m_pModelButton2 = new Button(this, "Player_Model_male_02", "&", this, "Male02");
	m_pModelButton3 = new Button(this, "Player_Model_male_03", "&", this, "Male03");
	m_pModelButton4 = new Button(this, "Player_Model_male_04", "&", this, "Male04");
	m_pModelButton5 = new Button(this, "Player_Model_male_05", "&", this, "Male05");
	m_pModelButton6 = new Button(this, "Player_Model_male_06", "&", this, "Male06");
	m_pModelButton7 = new Button(this, "Player_Model_male_07", "&", this, "Male07");
	m_pModelButton8 = new Button(this, "Player_Model_male_08", "&", this, "Male08");
	m_pModelButton9 = new Button(this, "Player_Model_male_09", "&", this, "Male09");


	m_pModelButton10 = new Button(this, "Player_Model_female_01", "&", this, "Female01");
	m_pModelButton11 = new Button(this, "Player_Model_female_02", "&", this, "Female02");
	m_pModelButton12 = new Button(this, "Player_Model_female_03", "&", this, "Female03");
	m_pModelButton13 = new Button(this, "Player_Model_female_04", "&", this, "Female04");
	m_pModelButton14 = new Button(this, "Player_Model_female_06", "&", this, "Female06");
	m_pModelButton15 = new Button(this, "Player_Model_female_07", "&", this, "Female07");


	m_pModelButton16 = new Button(this, "Player_Model_combine_soldier", "&", this, "CombineSoldier");
	m_pModelButton17 = new Button(this, "Player_Model_combine_soldier_prisonguard", "&", this, "CombineSoldierPrisonguard");
	m_pModelButton18 = new Button(this, "Player_Model_combine_super_soldier", "&", this, "CombineSuperSoldier");
	m_pModelButton19 = new Button(this, "Player_Model_police", "&", this, "MPolice");


	m_pModelButton20 = new Button(this, "Player_Model_gordon_01", "&", this, "Gordon01");
	m_pModelButton21 = new Button(this, "Player_Model_alyx_01", "&", this, "Alyx01");

	/*
	m_pModelButton1->SetVisible(true);
	m_pModelButton2->SetVisible(true);
	m_pModelButton3->SetVisible(true);
	m_pModelButton4->SetVisible(true);
	m_pModelButton5->SetVisible(true);
	m_pModelButton6->SetVisible(true);
	m_pModelButton7->SetVisible(true);
	m_pModelButton8->SetVisible(true);
	m_pModelButton9->SetVisible(true);
	m_pModelButton10->SetVisible(true);
	m_pModelButton11->SetVisible(true);
	m_pModelButton12->SetVisible(true);
	m_pModelButton13->SetVisible(true);
	m_pModelButton14->SetVisible(true);
	m_pModelButton15->SetVisible(true);
	m_pModelButton16->SetVisible(true);
	m_pModelButton17->SetVisible(true);
	m_pModelButton18->SetVisible(true);
	m_pModelButton19->SetVisible(true);
	m_pModelButton20->SetVisible(true);
	m_pModelButton21->SetVisible(true);
	*/
	m_pModelImage->SetVisible(true);
	m_pModelImage2->SetVisible(true);





	m_pCloseButton->SetPos(950, 472);
	m_pCloseButton->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pCloseButton->SetReleasedSound("ui/buttonclick.wav");


	m_pSpectateButton->SetPos(850, 472);
	m_pSpectateButton->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pSpectateButton->SetReleasedSound("ui/buttonclick.wav");

	m_pJoinButton->SetPos(750, 472);
	m_pJoinButton->SetWide(70);
	m_pJoinButton->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pJoinButton->SetReleasedSound("ui/buttonclick.wav");
	//m_pJoinButton->SetVisible(true);

	m_pModelButton1->SetPos(25, 44);
	m_pModelButton1->SetSize(82, 144);
	m_pModelButton1->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton1->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton2->SetPos(138, 44);
	m_pModelButton2->SetSize(82, 144);
	m_pModelButton2->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton2->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton3->SetPos(252, 44);
	m_pModelButton3->SetSize(82, 144);
	m_pModelButton3->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton3->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton4->SetPos(366, 44);
	m_pModelButton4->SetSize(82, 144);
	m_pModelButton4->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton4->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton5->SetPos(480, 44);
	m_pModelButton5->SetSize(82, 144);
	m_pModelButton5->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton5->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton6->SetPos(594, 44);
	m_pModelButton6->SetSize(82, 144);
	m_pModelButton6->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton6->SetReleasedSound("ui/buttonclick.wav");




	m_pModelButton7->SetPos(708, 44);
	m_pModelButton7->SetSize(82, 144);
	m_pModelButton7->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton7->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton8->SetPos(822, 44);
	m_pModelButton8->SetSize(82, 144);
	m_pModelButton8->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton8->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton9->SetPos(936, 44);
	m_pModelButton9->SetSize(82, 144);
	m_pModelButton9->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton9->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton10->SetPos(25, 200);
	m_pModelButton10->SetSize(82, 144);
	m_pModelButton10->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton10->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton11->SetPos(138, 200);
	m_pModelButton11->SetSize(82, 144);
	m_pModelButton11->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton11->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton12->SetPos(252, 200);
	m_pModelButton12->SetSize(82, 144);
	m_pModelButton12->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton12->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton13->SetPos(366, 200);
	m_pModelButton13->SetSize(82, 144);
	m_pModelButton13->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton13->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton14->SetPos(480, 200);
	m_pModelButton14->SetSize(82, 144);
	m_pModelButton14->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton14->SetReleasedSound("ui/buttonclick.wav");




	m_pModelButton15->SetPos(594, 200);
	m_pModelButton15->SetSize(82, 144);
	m_pModelButton15->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton15->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton16->SetPos(138, 355);
	m_pModelButton16->SetSize(82, 144);
	m_pModelButton16->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton16->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton17->SetPos(252, 355);
	m_pModelButton17->SetSize(82, 144);
	m_pModelButton17->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton17->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton18->SetPos(366, 355);
	m_pModelButton18->SetSize(82, 144);
	m_pModelButton18->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton18->SetReleasedSound("ui/buttonclick.wav");



	m_pModelButton19->SetPos(25, 355);
	m_pModelButton19->SetSize(82, 144);
	m_pModelButton19->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton19->SetReleasedSound("ui/buttonclick.wav");


	m_pModelButton20->SetPos(708, 200);
	m_pModelButton20->SetSize(82, 144);
	m_pModelButton20->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton20->SetReleasedSound("ui/buttonclick.wav");




	m_pModelButton21->SetPos(822, 200);
	m_pModelButton21->SetSize(82, 144);
	m_pModelButton21->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pModelButton21->SetReleasedSound("ui/buttonclick.wav");




}

//Class: CMyPanelInterface Class. Used for construction.
class CModelPanelInterface : public ModelPanel
{
private:
	CModelPanel* ModelPanel;
public:
	CModelPanelInterface()
	{
		ModelPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		ModelPanel = new CModelPanel(parent);
	}
	void Destroy()
	{
		if (ModelPanel)
		{
			ModelPanel->SetParent((vgui::Panel*)NULL);
			delete ModelPanel;
		}
	}
	void Activate(void)
	{
		if (ModelPanel)
		{
			ModelPanel->SetPos((ScreenWidth() - ModelPanel->GetWide()) / 2, (ScreenHeight() - ModelPanel->GetTall()) / 2);
			ModelPanel->Activate();
		}
	}
};
static CModelPanelInterface g_ModelPanel;
ModelPanel* modelpanel = (ModelPanel*)&g_ModelPanel;

ConVar cl_showmodelpanel("cl_showmodelpanel", "0", FCVAR_CLIENTDLL, "Sets the state of modelPanel <state>");



void CModelPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_showmodelpanel.GetBool()); //CL_SHOWMODELPANEL / 1 BY DEFAULT
	MoveToCenterOfScreen();

	/*
	if ((GetLocalTeam() && GetLocalTeam()->GetTeamNumber() == TEAM_SPECTATOR))
	{
		m_pJoinButton->SetVisible(true);
	}
	else
		m_pJoinButton->SetVisible(false);







	if ((GetLocalTeam() && GetLocalTeam()->GetTeamNumber() == TEAM_SPECTATOR))
	{

		m_pModelButton1->SetEnabled(false);
		m_pModelButton2->SetEnabled(false);
		m_pModelButton3->SetEnabled(false);
		m_pModelButton4->SetEnabled(false);
		m_pModelButton5->SetEnabled(false);
		m_pModelButton6->SetEnabled(false);
		m_pModelButton7->SetEnabled(false);
		m_pModelButton8->SetEnabled(false);
		m_pModelButton9->SetEnabled(false);
		m_pModelButton10->SetEnabled(false);
		m_pModelButton11->SetEnabled(false);
		m_pModelButton12->SetEnabled(false);
		m_pModelButton13->SetEnabled(false);
		m_pModelButton14->SetEnabled(false);
		m_pModelButton15->SetEnabled(false);
		m_pModelButton16->SetEnabled(false);
		m_pModelButton17->SetEnabled(false);
		m_pModelButton18->SetEnabled(false);
		m_pModelButton19->SetEnabled(false);
		m_pModelButton20->SetEnabled(false);
		m_pModelButton21->SetEnabled(false);
	}

	if ((GetLocalTeam() && GetLocalTeam()->GetTeamNumber() == TEAM_REBELS))
	{
		m_pModelButton1->SetEnabled(true);
		m_pModelButton2->SetEnabled(true);
		m_pModelButton3->SetEnabled(true);
		m_pModelButton4->SetEnabled(true);
		m_pModelButton5->SetEnabled(true);
		m_pModelButton6->SetEnabled(true);
		m_pModelButton7->SetEnabled(true);
		m_pModelButton8->SetEnabled(true);
		m_pModelButton9->SetEnabled(true);
		m_pModelButton10->SetEnabled(true);
		m_pModelButton11->SetEnabled(true);
		m_pModelButton12->SetEnabled(true);
		m_pModelButton13->SetEnabled(true);
		m_pModelButton14->SetEnabled(true);
		m_pModelButton15->SetEnabled(true);
		m_pModelButton16->SetEnabled(true);
		m_pModelButton17->SetEnabled(true);
		m_pModelButton18->SetEnabled(true);
		m_pModelButton19->SetEnabled(true);
		m_pModelButton20->SetEnabled(true);
		m_pModelButton21->SetEnabled(true);
	}
	if ((GetLocalTeam() && GetLocalTeam()->GetTeamNumber() == TEAM_COMBINE))
	{
		m_pModelButton1->SetEnabled(true);
		m_pModelButton2->SetEnabled(true);
		m_pModelButton3->SetEnabled(true);
		m_pModelButton4->SetEnabled(true);
		m_pModelButton5->SetEnabled(true);
		m_pModelButton6->SetEnabled(true);
		m_pModelButton7->SetEnabled(true);
		m_pModelButton8->SetEnabled(true);
		m_pModelButton9->SetEnabled(true);
		m_pModelButton10->SetEnabled(true);
		m_pModelButton11->SetEnabled(true);
		m_pModelButton12->SetEnabled(true);
		m_pModelButton13->SetEnabled(true);
		m_pModelButton14->SetEnabled(true);
		m_pModelButton15->SetEnabled(true);
		m_pModelButton16->SetEnabled(true);
		m_pModelButton17->SetEnabled(true);
		m_pModelButton18->SetEnabled(true);
		m_pModelButton19->SetEnabled(true);
		m_pModelButton20->SetEnabled(true);
		m_pModelButton21->SetEnabled(true);
	}
	if ((GetLocalTeam() && GetLocalTeam()->GetTeamNumber() == TEAM_UNASSIGNED))
	{
		m_pModelButton1->SetEnabled(true);
		m_pModelButton2->SetEnabled(true);
		m_pModelButton3->SetEnabled(true);
		m_pModelButton4->SetEnabled(true);
		m_pModelButton5->SetEnabled(true);
		m_pModelButton6->SetEnabled(true);
		m_pModelButton7->SetEnabled(true);
		m_pModelButton8->SetEnabled(true);
		m_pModelButton9->SetEnabled(true);
		m_pModelButton10->SetEnabled(true);
		m_pModelButton11->SetEnabled(true);
		m_pModelButton12->SetEnabled(true);
		m_pModelButton13->SetEnabled(true);
		m_pModelButton14->SetEnabled(true);
		m_pModelButton15->SetEnabled(true);
		m_pModelButton16->SetEnabled(true);
		m_pModelButton17->SetEnabled(true);
		m_pModelButton18->SetEnabled(true);
		m_pModelButton19->SetEnabled(true);
		m_pModelButton20->SetEnabled(true);
		m_pModelButton21->SetEnabled(true);
	}
	*/




	ConVar* cl_playermodel = cvar->FindVar("cl_playermodel");
	//--------------------------------------------------------------
	// Male
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_01.mdl") == 0)
	{
		m_pModelImage->SetPos(-62, -13);
		m_pModelImage2->SetPos(-62, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_02.mdl") == 0)
	{
		m_pModelImage->SetPos(51, -13);
		m_pModelImage2->SetPos(51, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_03.mdl") == 0)
	{
		m_pModelImage->SetPos(165, -13);
		m_pModelImage2->SetPos(165, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_04.mdl") == 0)
	{
		m_pModelImage->SetPos(279, -13);
		m_pModelImage2->SetPos(279, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_05.mdl") == 0)
	{
		m_pModelImage->SetPos(393, -13);
		m_pModelImage2->SetPos(393, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_06.mdl") == 0)
	{
		m_pModelImage->SetPos(507, -13);
		m_pModelImage2->SetPos(507, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_07.mdl") == 0)
	{
		m_pModelImage->SetPos(621, -13);
		m_pModelImage2->SetPos(621, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_08.mdl") == 0)
	{
		m_pModelImage->SetPos(735, -13);
		m_pModelImage2->SetPos(735, -13);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_09.mdl") == 0)
	{
		m_pModelImage->SetPos(849, -13);
		m_pModelImage2->SetPos(849, -13);
	}
	//--------------------------------------------------------------
	// Female
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_01.mdl") == 0)
	{
		m_pModelImage->SetPos(-62, 144);
		m_pModelImage2->SetPos(-62, 144);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_02.mdl") == 0)
	{
		m_pModelImage->SetPos(51, 144);
		m_pModelImage2->SetPos(51, 144);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_03.mdl") == 0)
	{
		m_pModelImage->SetPos(165, 144);
		m_pModelImage2->SetPos(165, 144);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_04.mdl") == 0)
	{
		m_pModelImage->SetPos(279, 144);
		m_pModelImage2->SetPos(279, 144);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_06.mdl") == 0)
	{
		m_pModelImage->SetPos(393, 144);
		m_pModelImage2->SetPos(393, 144);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_07.mdl") == 0)
	{
		m_pModelImage->SetPos(507, 144);
		m_pModelImage2->SetPos(507, 144);
	}
	//--------------------------------------------------------------
	// Combine
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/combine_soldier.mdl") == 0)
	{
		m_pModelImage->SetPos(51, 298);
		m_pModelImage2->SetPos(51, 298);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/combine_soldier_prisonguard.mdl") == 0)
	{
		m_pModelImage->SetPos(165, 298);
		m_pModelImage2->SetPos(165, 298);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/combine_super_soldier.mdl") == 0)
	{
		m_pModelImage->SetPos(279, 298);
		m_pModelImage2->SetPos(279, 298);
	}


	if (Q_strcmp(cl_playermodel->GetString(), "models/police.mdl") == 0)
	{
		m_pModelImage->SetPos(-62, 298);
		m_pModelImage2->SetPos(-62, 298);
	}



	//--------------------------------------------------------------
    // Gordon
    //--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/gordon.mdl") == 0)
	{
		m_pModelImage->SetPos(621, 144);
		m_pModelImage2->SetPos(621, 144);
	}
	//--------------------------------------------------------------
	// Alyx
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/alyx.mdl") == 0)
	{
		m_pModelImage->SetPos(735, 144);
		m_pModelImage2->SetPos(735, 144);
	}





	bool isEscapeKeyDown = input()->IsKeyDown(KEY_ESCAPE);
	if (isEscapeKeyDown)
	{
		cl_showmodelpanel.SetValue(0);

	}
	
}

void CModelPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);
	ConVar* cl_playermodel = cvar->FindVar("cl_playermodel");




	if (!Q_stricmp(pcCommand, "turnoff"))
	{
		cl_showmodelpanel.SetValue(0);
	}

	if (!Q_stricmp(pcCommand, "spectate"))
	{
		engine->ClientCmd(const_cast<char*>("jointeam 1"));
		cl_showmodelpanel.SetValue(0);
	}

    //--------------------------------------------------------------
    // Join Game Start
    //--------------------------------------------------------------
	if (!Q_stricmp(pcCommand, "joingame"))
	{


	//--------------------------------------------------------------
	// Male
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_01.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_02.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_03.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_04.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_05.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_06.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_07.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_08.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/male_09.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));
	//--------------------------------------------------------------
	// Female
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_01.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_02.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_03.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_04.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_06.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/female_07.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));
	//--------------------------------------------------------------
	// Combine
	//--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/combine_soldier.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 2"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/combine_soldier_prisonguard.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 2"));

	if (Q_strcmp(cl_playermodel->GetString(), "models/combine_super_soldier.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 2"));


	if (Q_strcmp(cl_playermodel->GetString(), "models/police.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 2"));


	//--------------------------------------------------------------
    // Gordon
    //--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/gordon.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));
    //--------------------------------------------------------------
    // Alyx
    //--------------------------------------------------------------
	if (Q_strcmp(cl_playermodel->GetString(), "models/humans/group03/alyx.mdl") == 0)
		engine->ClientCmd(const_cast<char*>("jointeam 3"));



		cl_showmodelpanel.SetValue(0);
	}

	//--------------------------------------------------------------
    // Join Game End
    //--------------------------------------------------------------


	//--------------------------------------------------------------
	// Male
	//--------------------------------------------------------------

	if (!Q_stricmp(pcCommand, "Male01"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_01.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	if (!Q_stricmp(pcCommand, "Male02"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_02.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	if (!Q_stricmp(pcCommand, "Male03"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_03.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Male04"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_04.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Male05"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_05.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Male06"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_06.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Male07"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_07.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Male08"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_08.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Male09"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/male_09.mdl"));
		cl_showmodelpanel.SetValue(0);
	}

	//--------------------------------------------------------------
	// Female
	//--------------------------------------------------------------

	if (!Q_stricmp(pcCommand, "Female01"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/female_01.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	if (!Q_stricmp(pcCommand, "Female02"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/female_02.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	if (!Q_stricmp(pcCommand, "Female03"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/female_03.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Female04"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/female_04.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	if (!Q_stricmp(pcCommand, "Female06"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/female_06.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "Female07"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/female_07.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	//--------------------------------------------------------------
	// Combine
	//--------------------------------------------------------------

	if (!Q_stricmp(pcCommand, "CombineSoldier"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/combine_soldier.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "CombineSoldierPrisonguard"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/combine_soldier_prisonguard.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



	if (!Q_stricmp(pcCommand, "CombineSuperSoldier"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/combine_super_soldier.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	if (!Q_stricmp(pcCommand, "MPolice"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/police.mdl"));
		cl_showmodelpanel.SetValue(0);
	}


	//--------------------------------------------------------------
    // Gordon
    //--------------------------------------------------------------
	if (!Q_stricmp(pcCommand, "Gordon01"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/gordon.mdl"));
		cl_showmodelpanel.SetValue(0);
	}
	//--------------------------------------------------------------
	// Alyx
	//--------------------------------------------------------------
	if (!Q_stricmp(pcCommand, "Alyx01"))
	{
		engine->ClientCmd(const_cast<char*>("cl_playermodel models/humans/group03/alyx.mdl"));
		cl_showmodelpanel.SetValue(0);
	}



}






CON_COMMAND(ToggleModelPanel, "Toggles modelPanel on or off")
{
	cl_showmodelpanel.SetValue(!cl_showmodelpanel.GetBool());
	modelpanel->Activate();
};

void CModelPanel::MoveToCenterOfScreen()
{
	int wx, wy, ww, wt;
	surface()->GetWorkspaceBounds(wx, wy, ww, wt);
	SetPos((ww - GetWide()) / 2, (wt - GetTall()) / 2);
}