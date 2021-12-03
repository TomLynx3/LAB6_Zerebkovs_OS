#undef UNICODE
#include <windows.h>
#include "resource.h"
#include "TPhilosopher.h"
#include "UIHelper.h"




#pragma warning (disable:4996)


BOOL CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);




TPhilosopher* philos[5] = { NULL };

HANDLE DinningRoomSem;

HANDLE ForkMutexs[5];

HWND mainHWND;


bool isDeleteRunning = false;


void CreatePhilosophers() {

	for (int i = 0; i < 5; i++) {
		TPhilosopher* philo = new TPhilosopher();
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