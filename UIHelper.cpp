#include "UIHelper.h"

UIHelper::UIHelper(HWND AhWnd, TPhilosopher* Aphilosopher, int Apos)
{
	position = Apos;
	hWnd = AhWnd;
	philosopher = Aphilosopher;

	switch (Apos) {
	case 0:
		formIds = UIFormIds{ IDC_STATE1,IDC_FORCE1,IDC_WISDOM1 };
		break;
	case 1:
		formIds = UIFormIds{ IDC_STATE2,IDC_FORCE2,IDC_WISDOM2 };
		break;
	case 2:
		formIds = UIFormIds{ IDC_STATE3,IDC_FORCE3,IDC_WISDOM3 };
		break;
	case 3:
		formIds = UIFormIds{ IDC_STATE4,IDC_FORCE4,IDC_WISDOM4 };
		break;
	case 4:
		formIds = UIFormIds{ IDC_STATE5,IDC_FORCE5,IDC_WISDOM5 };
		break;
	}
}

void UIHelper::UpdateUI()
{
	UpdateState();
	UpdateForce();
	UpdateWisdom();
}

UIHelper::~UIHelper()
{
}
void UIHelper::UpdateState()
{
	char stateBuf[9];

	PhilosopherStateToText(stateBuf);

	SetDlgItemText(hWnd, formIds.stateID, stateBuf);
}

void UIHelper::PhilosopherStateToText(char* output)
{
	int state = philosopher->GetState();

	if(state ==psIdle){
		sprintf(output, "%s", "Idling");
		return;
	}
	else if (state == psThink) {
		sprintf(output, "%s", "Thinking");
		return;
	}
	else if (state == psEat) {
		sprintf(output, "%s", "Eating");
		return;
	}
	else if (state == psDie) {
		sprintf(output, "%s", "Dead");
		return;
	}
}

void UIHelper::UpdateForce()
{
	int force = philosopher->GetForce();

	char forceBuf[10];

	sprintf(forceBuf, "%d", force);

	SetDlgItemText(hWnd, formIds.forceID, forceBuf);
}

void UIHelper::UpdateWisdom()
{
	int wisdom = philosopher->GetWisdom();
	
	char wisdomBuf[10];

	sprintf(wisdomBuf, "%d", wisdom);

	SetDlgItemText(hWnd, formIds.wisdomID, wisdomBuf);
}

