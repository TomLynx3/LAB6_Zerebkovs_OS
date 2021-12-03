#pragma once
#include <windows.h>
#include "TPhilosopher.h"
#include "resource.h"
#include <wingdi.h>

#pragma warning (disable:4996)

struct UIFormIds{ int stateID; int forceID; int wisdomID; };


class UIHelper
{
private:
	HWND hWnd;
	TPhilosopher* philosopher;
	int position;
	UIFormIds formIds;
	void UpdateState();
	void PhilosopherStateToText(char* output);
	void UpdateForce();
	void UpdateWisdom();
public:
	UIHelper(HWND AhWnd, TPhilosopher* Aphilosopher,int Apos);
	void UpdateUI();
	void UpdateStateTextColor(HDC hdc);
	~UIHelper();

};

