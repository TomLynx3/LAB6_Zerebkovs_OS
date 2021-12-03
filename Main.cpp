#undef UNICODE
#include <windows.h>
#include "resource.h"
#include "TPhilosopher.h"
#include "UIHelper.h"
#include <CommCtrl.h>



#pragma warning (disable:4996)


BOOL CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);




TPhilosopher* philos[5] = { NULL };

HANDLE DinningRoomSem;

HANDLE ForkMutexs[5];

HWND mainHWND;


bool isDeleteRunning = false;

HINSTANCE hInstance;

int voracity = 50;


void CreatePhilosophers() {

	for (int i = 0; i < 5; i++) {
		TPhilosopher* philo = new TPhilosopher();
		philo->SetVoracity(voracity);
		philos[i] = philo;
	}

	
}


void Initialize() {

	DinningRoomSem = CreateSemaphore(NULL, 4, 4, "DinningRoomSem");

	for (int i = 0; i < 5; i++) {
		HANDLE forkMutex = CreateMutex(NULL, FALSE, NULL);

		ForkMutexs[i] = forkMutex;
	}
}


DWORD WINAPI PhilosopherThread(void* Id) {
	

	int id = (int)Id;

	TPhilosopher* philosopher = philos[id];


	while (philosopher->GetState()!=psDie && !isDeleteRunning)
	{
		philosopher->Think();


			WaitForSingleObject(DinningRoomSem, INFINITE);

			
			WaitForSingleObject(ForkMutexs[id], INFINITE);
			WaitForSingleObject(ForkMutexs[(id + 1) % 5], INFINITE);
			

			philosopher->Eat();


			ReleaseMutex(ForkMutexs[id]);
			ReleaseMutex(ForkMutexs[(id + 1) % 5]);

			ReleaseSemaphore(DinningRoomSem, 1,NULL);


	}

	return 0;
}


void HandleStart(HWND hWnd) {



	CreatePhilosophers();
	
	
	isDeleteRunning = false;

	for (int i = 0; i < 5; i++) {

		CloseHandle(CreateThread(NULL, 0, PhilosopherThread, (LPVOID)i, 0, NULL));
	}

	EnableWindow(GetDlgItem(hWnd, IDC_START), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_STOP), TRUE);
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)MainWndProc);

	::hInstance = hInstance;

	return 0;
}

void HandleStop(HWND hWnd) {

	isDeleteRunning = true;

	EnableWindow(GetDlgItem(hWnd, IDC_STOP), FALSE);

	for (int i = 0; i < 5; i++) {
		TPhilosopher* philo = philos[i];

		philo->Kill();
		
		delete philo;

		philos[i] = NULL;
	}

	EnableWindow(GetDlgItem(hWnd, IDC_START), TRUE);
	

}

void UpdateUI(HWND hWnd, int pos) {

	TPhilosopher* philo = philos[pos];
	

	if (philo != NULL) {
		UIHelper* uiHelper = new UIHelper(hWnd, philo, pos);

		uiHelper->UpdateUI();

		delete uiHelper;
	}
}

void SetStateColor(LPARAM param, HWND hWnd,HDC hdc) {

	int control = GetDlgCtrlID((HWND)param);

	int index = control - IDC_STATE1;

	if (index >= 0 && index < 5) {

		TPhilosopher* philo = philos[index];

		if (philo != NULL) {

			UIHelper* uiHelper = new UIHelper(hWnd, philo, index);

			uiHelper->UpdateStateTextColor(hdc);

			delete uiHelper;


		}


	}
}



void SetSliderTxtValue(HWND hWnd,int pos) {
	char buf[4];

	sprintf(buf, "%d", pos);

	SetDlgItemText(hWnd, IDC_VORACITYTXT, buf);
}

void InitializeSlider(HWND hWnd) {

	SetSliderTxtValue(hWnd,voracity);
	
	SendMessage(GetDlgItem(hWnd, IDC_SLIDER1), TBM_SETRANGE, (WPARAM)FALSE, MAKELPARAM(0, 100));
	SendMessage(GetDlgItem(hWnd, IDC_SLIDER1), TBM_SETPOS, TRUE, voracity);
}

void HandleScrollChange(HWND hWnd) {

	DWORD pos = SendMessage(GetDlgItem(hWnd, IDC_SLIDER1), TBM_GETPOS, 0, 0);

	SetSliderTxtValue(hWnd, static_cast<int>(pos));
}

void SetVoracity(HWND hWnd) {

	DWORD ver = SendMessage(GetDlgItem(hWnd, IDC_SLIDER1), TBM_GETPOS, 0, 0);

	::voracity = static_cast<int>(ver);

	if (philos[0] != NULL) {
		for (int i = 0; i < 5; i++) {
			TPhilosopher* philosopher = philos[i];

			philosopher->SetVoracity(voracity);

		}
	}
	
	
	
}

BOOL CALLBACK VoracityWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	switch (Msg) {
	case WM_INITDIALOG:
		
		InitializeSlider(hWnd);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDC_SETVORACITY:

			SetVoracity(hWnd);
			EndDialog(hWnd, NULL);
			return TRUE;

		}
		return FALSE;
	case WM_DESTROY:
		EndDialog(hWnd, NULL);
		return TRUE;
	case WM_HSCROLL:
		if (lParam != 0)
		{
			switch (LOWORD(wParam))
			{
			case TB_THUMBTRACK:
			case TB_ENDTRACK:
				HandleScrollChange(hWnd);
				break;
			}
		}
		
		return TRUE;
	case WM_CLOSE:
		EndDialog(hWnd, NULL);
		return TRUE;
	}
	return FALSE;
}



BOOL CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_INITDIALOG:
		mainHWND = hWnd;

		Initialize();
		EnableWindow(GetDlgItem(hWnd, IDC_STOP), FALSE);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		
		case IDC_START:
			HandleStart(hWnd);
			return TRUE;
		case IDC_EXIT:
			PostQuitMessage(0);
			return TRUE;	
		case IDC_STOP:
			HandleStop(hWnd);
			return TRUE;
		case IDC_VORACITY:
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_VORACITYDIAG),hWnd, VoracityWndProc);
			return TRUE;
			
		}
		return FALSE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	case WM_PHILOSOPHER:
		UpdateUI(hWnd, wParam);
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return TRUE;
	case WM_CTLCOLORSTATIC:

		SetBkMode((HDC)wParam, TRANSPARENT);

		SetStateColor(lParam, hWnd, (HDC)wParam);

		return (BOOL)GetSysColorBrush(COLOR_MENU);
	}
	return FALSE;
}