#include "Nonce.h"
#include <iostream>
#include <sstream>
#include <thread>

HANDLE hConsole = nullptr;
HANDLE hHeap = nullptr;
HANDLE ofile = nullptr;
HANDLE ofile_stream = nullptr;

enum colour { DARKBLUE = 1, DARKGREEN, DARKTEAL, DARKRED, DARKPINK, DARKYELLOW, GRAY, DARKGRAY, BLUE, GREEN, TEAL, RED, PINK, YELLOW, WHITE };
std::array <char*, HASH_CAP * sizeof(char*)> cache;
std::array <char*, HASH_CAP * sizeof(char*)> cache_write;
std::string out_path = "";
std::string g_move_path = "";
std::string g_file_name_s = "";
std::string g_file_name_d = "";
std::vector<std::string> argsp;
std::vector<size_t> worker_status;
std::thread TMove;
unsigned long long written_scoops = 0;
bool first_plot = true;
unsigned long long move_plots = 0;
unsigned long long addr = 0;
unsigned long long startnonce = 0;
unsigned long long nonces = 0;
unsigned long long threads = 1;
unsigned long long nonces_per_thread = 0;
unsigned long long memory = 0;
unsigned long long lcounter = 0;
double Percentage;

void printCopyProgress(double percentage)
{
	int val = (int)(percentage * 100);
	printf(" |%d%", val);

	// Flush to disk every time this is called
	fflush(stdout);
}

DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData)
{
	// Calculate the percentage
	Percentage = (double(StreamBytesTransferred.QuadPart) / double(StreamSize.QuadPart));
	printCopyProgress(Percentage);
	// Continue
	return PROGRESS_CONTINUE;
}

BOOL SetPrivilege(void)
{
	LUID luid;
	if (!LookupPrivilegeValue(
		NULL,					// lookup privilege on local system
		SE_MANAGE_VOLUME_NAME,  // privilege to lookup
		&luid))					// receives LUID of privilege
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf("LookupPrivilegeValue error: %u\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return FALSE;
	}

	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf("OpenProcessToken error: %u\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return FALSE;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf("AdjustTokenPrivileges error: %u\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf("\nThe token does not have the specified privilege.\nFor faster writing you should restart plotter with Administrative rights.\n");

		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return FALSE;
	}
	return TRUE;
}

unsigned long long getFreeSpace(const char* path)
{
	ULARGE_INTEGER lpFreeBytesAvailable;
	ULARGE_INTEGER lpTotalNumberOfBytes;
	ULARGE_INTEGER lpTotalNumberOfFreeBytes;

	GetDiskFreeSpaceExA(path, &lpFreeBytesAvailable, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);

	return lpFreeBytesAvailable.QuadPart;
}

unsigned long long getTotalSystemMemory()
{
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailPhys;
}

void writer_i(const unsigned long long offset, const unsigned long long nonces_to_write, const unsigned long long glob_nonces)
{
	LARGE_INTEGER liDistanceToMove;
	LARGE_INTEGER start_time, end_time;
	DWORD dwBytesWritten;
	double PCFreq = 0.0;

	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	PCFreq = double(li.QuadPart);

	written_scoops = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&start_time);
	for (size_t scoop = 0; scoop < HASH_CAP; scoop++)
	{
		liDistanceToMove.QuadPart = (scoop*glob_nonces + offset) * SCOOP_SIZE;
		if (!SetFilePointerEx(ofile, liDistanceToMove, nullptr, FILE_BEGIN))
		{
			SetConsoleTextAttribute(hConsole, colour::RED);
			printf(" error SetFilePointerEx (code = %u)\n", GetLastError());
			SetConsoleTextAttribute(hConsole, colour::GRAY);
			exit(-1);
		}
		if (!WriteFile(ofile, &cache_write[scoop][0], DWORD(SCOOP_SIZE * nonces_to_write), &dwBytesWritten, nullptr))
		{
			SetConsoleTextAttribute(hConsole, colour::RED);
			printf(" Failed WriteFile (code = %u)\n", GetLastError());
			SetConsoleTextAttribute(hConsole, colour::GRAY);
			exit(-1);
		}
		written_scoops = scoop + 1;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&end_time);

	write_to_stream(offset + nonces_to_write);
	return;
}

bool write_to_stream(const unsigned long long data)
{
	LARGE_INTEGER liDistanceToMove;
	DWORD dwBytesWritten;
	liDistanceToMove.QuadPart = 0;
	unsigned long long buf = data;
	if (!SetFilePointerEx(ofile_stream, liDistanceToMove, nullptr, FILE_BEGIN))
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf(" error stream SetFilePointerEx (code = %u)\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return false;
	}
	if (!WriteFile(ofile_stream, &buf, DWORD(sizeof(buf)), &dwBytesWritten, nullptr))
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf(" Failed stream WriteFile (code = %u)\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return false;
	}
	if (SetEndOfFile(ofile_stream) == 0)
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf(" Failed stream SetEndOfFile (code = %u)\n", GetLastError());
		CloseHandle(ofile_stream);
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return false;
		exit(-1);
	}
	FlushFileBuffers(ofile_stream);
	return true;
}

unsigned long long read_from_stream()
{
	LARGE_INTEGER liDistanceToMove;
	DWORD dwBytesRead;
	liDistanceToMove.QuadPart = 0;
	if (!SetFilePointerEx(ofile_stream, liDistanceToMove, nullptr, FILE_BEGIN))
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf(" error stream SetFilePointerEx (code = %u)\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return 0;
	}
	unsigned long long buf = 0;
	if (!ReadFile(ofile_stream, &buf, DWORD(sizeof(buf)), &dwBytesRead, nullptr))
	{
		SetConsoleTextAttribute(hConsole, colour::RED);
		printf(" Failed stream ReadFile (code = %u)\n", GetLastError());
		SetConsoleTextAttribute(hConsole, colour::GRAY);
		return 0;
	}
	//printf(" read_from_stream = %llu\n", buf);
	return buf;
}

bool is_number(const std::string& s)
{
	return(strspn(s.c_str(), "0123456789") == s.size());
}

void get_args_start()
{
	for (auto & it : argsp)
		for (auto & c : it) c = tolower(c);

	for (size_t i = 1; i < argsp.size() - 1; i++)
	{
		if ((argsp[i] == "-id") && is_number(argsp[++i])) {
			addr = strtoull(argsp[i].c_str(), 0, 10);
		}
		if ((argsp[i] == "-sn") && is_number(argsp[++i])) {
			startnonce = strtoull(argsp[i].c_str(), 0, 10);
		}
		if ((argsp[i] == "-n") && is_number(argsp[++i])) {
			nonces = strtoull(argsp[i].c_str(), 0, 10);
		}
		if ((argsp[i] == "-t") && is_number(argsp[++i])) {
			threads = strtoull(argsp[i].c_str(), 0, 10);
		}
		if (argsp[i] == "-path") {
			out_path = argsp[++i];
		}
		if (argsp[i] == "-move") {
			g_move_path = argsp[++i];

			move_plots = 1;
		}
		if (argsp[i] == "-mem")
		{
			i++;
			memory = strtoull(argsp[i].substr(0, argsp[i].find_last_of("0123456789") + 1).c_str(), 0, 10);
			switch (argsp[i][argsp[i].length() - 1])
			{
			case 't':
			case 'T':
				memory *= 1024;
			case 'g':
			case 'G':
				memory *= 1024;
			}
		}
		if ((argsp[i] == "-repeat") && is_number(argsp[++i]))
		{
			lcounter = strtoull(argsp[i].c_str(), 0, 10);
		}
	}
}

void get_args_next()
{
	for (auto & it : argsp)
		for (auto & c : it) c = tolower(c);
	for (size_t i = 1; i < argsp.size() - 1; i++)
	{
		if ((argsp[i] == "-t") && is_number(argsp[++i]))
			threads = strtoull(argsp[i].c_str(), 0, 10);
		if (argsp[i] == "-mem")
		{
			i++;
			memory = strtoull(argsp[i].substr(0, argsp[i].find_last_of("0123456789") + 1).c_str(), 0, 10);
			switch (argsp[i][argsp[i].length() - 1])
			{
			case 't':
			case 'T':
				memory *= 1024;
			case 'g':
			case 'G':
				memory *= 1024;
			}
		}
	}
}

std::wstring string2LPCWSTR(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void MoveThread() {
	printf("\nSpawned Mover Thread!");
	printf("\nMove Path   : %s", g_file_name_d.c_str());
	std::wstring gfns1 = string2LPCWSTR(g_file_name_s);
	LPCWSTR g_file_name_s_l = gfns1.c_str();
	std::wstring gfns2 = string2LPCWSTR(g_file_name_d);
	LPCWSTR g_file_name_d_l = gfns2.c_str();
	try {
		MoveFileWithProgress(g_file_name_s_l, g_file_name_d_l, CopyProgressRoutine, 0, MOVEFILE_COPY_ALLOWED);
	}
	catch (std::exception& e) { printf("\nMover Thread Died And/Or Failed!...( %s )\n", e); }
}

int main(int argc, char* argv[])
{
	std::vector<std::string> args(argv, &argv[argc]);
	argsp = args;
	std::thread writer;
	std::vector<std::thread> workers;

	// Loop
	do
	{
		unsigned long long start_timer = 0;

		// First Plot
		if (first_plot == true) {
			get_args_start();

			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hConsole == NULL) {
				SetConsoleTextAttribute(hConsole, colour::RED);
				printf("Failed to retrieve handle of the process (%u).\n", GetLastError());
				SetConsoleTextAttribute(hConsole, colour::GRAY);
				exit(-1);
			}

			SetConsoleTextAttribute(hConsole, colour::GREEN);
			printf("SPlotter for BURST\n");
			printf("This software allows you to make lots of small pre-optimized plots automatically\n Please consider donating: BURST-ZNEH-ZB8X-9T38-HSND9");

			if (out_path.empty() || (out_path.find(":") == std::string::npos))
			{
				char Buffer[MAX_PATH];
				GetCurrentDirectoryA(MAX_PATH, Buffer);
				std::string _path = Buffer;
				out_path = _path + "\\" + out_path;
			}
			if (out_path.rfind("\\") < out_path.length() - 1) out_path += "\\";
			if (g_move_path.rfind("\\") < g_move_path.length() - 1) g_move_path += "\\";

			SetConsoleTextAttribute(hConsole, colour::GRAY);
			printf("\n\nChecking Directory Exists...\n");

			if (!CreateDirectoryA(out_path.c_str(), nullptr) && ERROR_ALREADY_EXISTS != GetLastError())
			{
				SetConsoleTextAttribute(hConsole, colour::RED);
				printf("\nCan't create directory %s for plots (Error %u)\n", out_path.c_str(), GetLastError());
				SetConsoleTextAttribute(hConsole, colour::GRAY);
				exit(-1);
			}

			SetConsoleTextAttribute(hConsole, colour::BLUE);
			printf("\nWallet ID : %llu\n", addr);
			printf("Start Nonce : %llu\n", startnonce);
			printf("Nonces      : %llu\n", nonces);
			printf("End Nonce   : %llu\n", startnonce + nonces);
			if (lcounter == 1) { printf("# of Plots  : %llu\n", lcounter); }
			if (move_plots == 1) { printf("Move Plots  : Enabled\n"); }
		}
		// First Plot

		DWORD sectorsPerCluster;
		DWORD bytesPerSector;
		DWORD numberOfFreeClusters;
		DWORD totalNumberOfClusters;
		if (!GetDiskFreeSpaceA(out_path.c_str(), &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters))
		{
			SetConsoleTextAttribute(hConsole, colour::RED);
			printf("\nGetDiskFreeSpace failed (Error %u)\n", GetLastError());
			SetConsoleTextAttribute(hConsole, colour::GRAY);
			exit(-1);
		}
		if (nonces == 0) 	nonces = getFreeSpace(out_path.c_str()) / PLOT_SIZE;
		nonces = (nonces / (bytesPerSector / SCOOP_SIZE)) * (bytesPerSector / SCOOP_SIZE);
		std::string filename = std::to_string(addr) + "_" + std::to_string(startnonce) + "_" + std::to_string(nonces) + "_" + std::to_string(nonces);

		BOOL granted = SetPrivilege();
		ofile_stream = CreateFileA((out_path + filename + ":stream").c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (ofile_stream == INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(hConsole, colour::RED);
			printf("\nError creating stream for file %s\n", (out_path + filename).c_str());
			SetConsoleTextAttribute(hConsole, colour::GRAY);
			exit(-1);
		}
		unsigned long long nonces_done = read_from_stream();
		if (nonces_done == nonces) // exit
		{
			SetConsoleTextAttribute(hConsole, colour::RED);
			printf("\nFile is already finished. Delete the existing file to start over\n");
			SetConsoleTextAttribute(hConsole, colour::GRAY);
			CloseHandle(ofile_stream);
			exit(0);
		}
		if (nonces_done > 0)
		{
			SetConsoleTextAttribute(hConsole, colour::YELLOW);
			printf("\nContinuing with plot from nonce: %llu\n", nonces_done);
		}

		SetConsoleTextAttribute(hConsole, colour::DARKGRAY);
		g_file_name_d = (g_move_path + filename);
		g_file_name_s = (out_path + filename);
		printf("\n--------------------------------------------");
		printf("\nCreating file: %s\n", g_file_name_s.c_str());
		ofile = CreateFileA((out_path + filename).c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_ALWAYS, FILE_FLAG_NO_BUFFERING, nullptr); //FILE_ATTRIBUTE_NORMAL     FILE_FLAG_WRITE_THROUGH |
		if (ofile == INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(hConsole, colour::RED);
			printf("\nError creating file %s\n", (out_path + filename).c_str());
			SetConsoleTextAttribute(hConsole, colour::GRAY);
			CloseHandle(ofile_stream);
			exit(-1);
		}

		// Reserve Free Space
		LARGE_INTEGER liDistanceToMove;
		liDistanceToMove.QuadPart = nonces * PLOT_SIZE;
		SetFilePointerEx(ofile, liDistanceToMove, nullptr, FILE_BEGIN);
		if (SetEndOfFile(ofile) == 0)
		{
			if (first_plot == true) {
				SetConsoleTextAttribute(hConsole, colour::RED);
				printf("\n Not enough free space, reduce \"nonces\"... (code = %u)\n", GetLastError());
				CloseHandle(ofile);
				CloseHandle(ofile_stream);
				DeleteFileA((out_path + filename).c_str());
				SetConsoleTextAttribute(hConsole, colour::GRAY);
				exit(-1);
			}
			else {
				SetConsoleTextAttribute(hConsole, colour::GREEN);
				printf("\n Not enough free space to make the next plot, Cleaning up... (code = %u)\n", GetLastError());
				printf("\n If you were expecting another plot, Try reducing the number of \"nonces\"");
				CloseHandle(ofile);
				CloseHandle(ofile_stream);
				DeleteFileA((out_path + filename).c_str());
				SetConsoleTextAttribute(hConsole, colour::GRAY);
				printf("\n Done... Bye!");
				exit(-1);
			}
		}

		if (granted)
		{
			if (SetFileValidData(ofile, nonces * PLOT_SIZE) == 0)
			{
				SetConsoleTextAttribute(hConsole, colour::RED);
				printf("\nSetFileValidData error (code = %u)\n", GetLastError());
				CloseHandle(ofile);
				CloseHandle(ofile_stream);
				SetConsoleTextAttribute(hConsole, colour::GRAY);
				exit(-1);
			}
		}

		unsigned long long freeRAM = getTotalSystemMemory();

		if (memory) nonces_per_thread = memory * 2 / threads;
		else nonces_per_thread = 1024; //(bytesPerSector / SCOOP_SIZE) * 1024 / threads;

		if (nonces < nonces_per_thread * threads) 	nonces_per_thread = nonces / threads;

		// Check Free RAM
		if (freeRAM < nonces_per_thread * threads * PLOT_SIZE * 2) nonces_per_thread = freeRAM / threads / PLOT_SIZE / 2;

		nonces_per_thread = (nonces_per_thread / (bytesPerSector / SCOOP_SIZE)) * (bytesPerSector / SCOOP_SIZE);

		SetConsoleTextAttribute(hConsole, colour::TEAL);
		printf("\nUsing %llu MB of %llu MB of usable RAM\n", nonces_per_thread * threads * 2 * PLOT_SIZE / 1024 / 1024, freeRAM / 1024 / 1024);

		cache.fill(nullptr);
		cache_write.fill(nullptr);
		for (size_t i = 0; i < HASH_CAP; i++)
		{
			cache[i] = (char *)VirtualAlloc(nullptr, threads * nonces_per_thread * SCOOP_SIZE, MEM_COMMIT, PAGE_READWRITE);
			cache_write[i] = (char *)VirtualAlloc(nullptr, threads * nonces_per_thread * SCOOP_SIZE, MEM_COMMIT, PAGE_READWRITE);
			if ((cache[i] == nullptr) || (cache_write[i] == nullptr))
			{
				SetConsoleTextAttribute(hConsole, colour::RED);
				printf(" Error allocating memory... Try lowering the amount of RAM \n");
				CloseHandle(ofile);
				CloseHandle(ofile_stream);
				SetConsoleTextAttribute(hConsole, colour::GRAY);
				exit(-1);
			}
		}

		unsigned long long t_timer;
		unsigned long long x = 0;
		unsigned long long leftover = 0;
		unsigned long long nonces_in_work = 0;
		start_timer = GetTickCount64();

		while (nonces_done < nonces) {
			t_timer = GetTickCount64();
			leftover = nonces - nonces_done;

			if (leftover / (nonces_per_thread*threads) == 0)
			{
				if (leftover >= threads*(bytesPerSector / SCOOP_SIZE)) {
					nonces_per_thread = leftover / threads;
					nonces_per_thread = (nonces_per_thread / (bytesPerSector / SCOOP_SIZE)) * (bytesPerSector / SCOOP_SIZE);
				}
				else {
					threads = 1;
					nonces_per_thread = leftover;
				}
			}

			for (size_t i = 0; i < threads; i++)
			{
#if defined __AVX2__
				std::thread th(std::thread(AVX2::work_i, i, addr, startnonce + nonces_done + i*nonces_per_thread, nonces_per_thread));
#elif defined (__AVX__)
				std::thread th(std::thread(AVX1::work_i, i, addr, startnonce + nonces_done + i*nonces_per_thread, nonces_per_thread));
#else defined (__SSE4_1__) || __SSE4_2__)
				std::thread th(std::thread(SSE4::work_i, i, addr, startnonce + nonces_done + i*nonces_per_thread, nonces_per_thread));
#endif
				workers.push_back(move(th));
				worker_status.push_back(0);
			}

			nonces_in_work = threads*nonces_per_thread;
			SetConsoleTextAttribute(hConsole, colour::WHITE);
			printf("\r[%llu%%] Generating nonces from %llu to %llu\t\t\t\t\t\t\n", (nonces_done * 100) / nonces, startnonce + nonces_done, startnonce + nonces_done + nonces_in_work);
			SetConsoleTextAttribute(hConsole, colour::YELLOW);

			do
			{
				Sleep(100);
				x = 0;
				for (auto it = worker_status.begin(); it != worker_status.end(); ++it) x += *it;
				printf("\r[CPU]N: %llu (%llu nonces/min)", nonces_done + x, x * 60000 / (GetTickCount64() - t_timer));
				printf("\t[HDD]S: %.2f%%", (double)(written_scoops * 100) / (double)HASH_CAP);
			} while (x < nonces_in_work);
			SetConsoleTextAttribute(hConsole, colour::GRAY);

			for (auto it = workers.begin(); it != workers.end(); ++it)	if (it->joinable()) it->join();
			for (auto it = worker_status.begin(); it != worker_status.end(); ++it) *it = 0;

			while ((written_scoops != 0) && (written_scoops < HASH_CAP))
			{
				Sleep(100);
				printf("\r[CPU] Nonces Done: %llu ", nonces_done + x);
				printf("\t\t\t\t[HDD] Writing Scoops: %.2f%% ", (double)(written_scoops * 100) / (double)HASH_CAP);
			}

			if (writer.joinable())	writer.join();
			cache_write.swap(cache);
			writer = std::thread(writer_i, nonces_done, nonces_in_work, nonces);
			nonces_done += nonces_in_work;
		}

		while ((written_scoops != 0) && (written_scoops < HASH_CAP))
		{
			Sleep(100);
			printf("\r[CPU]N: %llu", nonces_done + x);
			printf("\t[HDD]S: %.2f%%", (double)(written_scoops * 100) / (double)HASH_CAP);
		}

		printf("\nFinishing up writing this plot to file... Please wait...\n");
		if (writer.joinable()) writer.join();
		FlushFileBuffers(ofile);  //https://msdn.microsoft.com/en-en/library/windows/desktop/aa364218(v=vs.85).aspx
		CloseHandle(ofile_stream);
		CloseHandle(ofile);
		printf("\rThat plot took %llu seconds...\n", (GetTickCount64() - start_timer) / 1000);
		_flushall();

		// Freeing up RAM
		SetConsoleTextAttribute(hConsole, colour::DARKGRAY);
		printf("Releasing memory...\n ");
		for (size_t i = 0; i < HASH_CAP; i++)
		{
			VirtualFree(cache[i], 0, MEM_RELEASE);VirtualFree(cache_write[i], 0, MEM_RELEASE);
		}

		// Check if we should loop
		if (lcounter == 0) {
			printf("Done plotting!... Exiting... \n");
			Sleep(1000);
			exit(-1);
		}
		else {
			// Update the user
			SetConsoleTextAttribute(hConsole, colour::YELLOW);
			printf("\nStarting the next plot, Please wait... \n");

			//Set flags for the next plot
			get_args_next();
			first_plot = false;

			// Reduce Counter
			lcounter = lcounter - 1;

			// Next plot information
			SetConsoleTextAttribute(hConsole, colour::BLUE);
			printf("\nLast Start Nonce: %llu", startnonce);
			printf("\nNonces Per Plot : %llu", nonces);
			printf("\nNext Start Nonce: %llu ", startnonce + nonces + 1);
			printf("\nThreads         : %llu ", threads);
			printf("\nPlots Remaining : %llu ", lcounter);

			// Set next Start Nonce
			startnonce = startnonce + nonces + 1;
		}

		// Create Mover Thread
		if (move_plots == 1) {
			std::thread TMove(MoveThread);
			TMove.detach();
		}
		// Loop
	} while (true);
}
