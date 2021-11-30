#undef UNICODE
#include <windows.h>
#include "resource.h"
#include "TPhilosopher.h"
#include "UIHelper.h"




#pragma warning (disable:4996)

#define WM_PHILOSOPHER WM_USER

BOOL CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);




TPhilosopher* philos[5] = { NULL };

HANDLE DinningRoomSem;

HANDLE ForkMutexs[5];

HWND mainHWND;





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

	
	PostMessage(mainHWND,WM_PHILOSOPHER,(WPARAM) Id, 0);

	while (philosopher->GetState()!=psDie)
	{
		philosopher->Think();



		
		Sleep(100);
	


		PostMessage(mainHWND, WM_PHILOSOPHER, (WPARAM)Id, 0);
	}

	return 0;
}


void HandleStart() {

	CreatePhilosophers();

	for (int i = 0; i < 5; i++) {

		CloseHandle(CreateThread(NULL, 0, PhilosopherThread, (LPVOID)i, 0, NULL));
	}
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC)MainWndProc);


	return 0;
}

void UpdateUI(HWND hWnd, int pos) {

	TPhilosopher* philo = philos[pos];
	

	if (philo != NULL) {
		UIHelper* uiHelper = new UIHelper(hWnd, philo, pos);

		uiHelper->UpdateUI();
	}
}



BOOL CALLBACK MainWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	switch (Msg) {
	case WM_INITDIALOG:
		mainHWND = hWnd;

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		
		case IDC_START:
			HandleStart();
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
	}
	return FALSE;
}