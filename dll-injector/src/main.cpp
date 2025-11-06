#include <filesystem>
#include <iostream>
#include <string>

#include <windows.h>

#include <ng-log/logging.h>

int main(int argc, char* argv[]) {
  // Initialize logging to output to stdout
  FLAGS_logtostdout = true;
  nglog::InitializeLogging(argv[0]);

  // Extract the exe filename from argv[0] for usage message
  std::filesystem::path exe_path(argv[0]);
  std::string exe_name = exe_path.filename().string();

  if (argc != 3) {
    std::cout << "Usage: " << exe_name << " <pid> <dll_path>" << std::endl;
    return 1;
  }

  DWORD pid = std::stoul(argv[1]);
  const char* dll_path = argv[2];

  // Convert to absolute path - LoadLibraryA requires absolute paths
  std::filesystem::path full_path = std::filesystem::absolute(dll_path);
  std::string dll_path_absolute = full_path.string();

  // Extract just the DLL filename from the full path
  std::string dll_name = full_path.filename().string();

  // Open the target process with necessary permissions for injection
  HANDLE process = OpenProcess(
    PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
    FALSE,
    pid
  );
  if (!process) {
    LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
               << " (OpenProcess failed)";
    return 1;
  }

  // Allocate memory in the target process for the DLL path
  SIZE_T path_size = dll_path_absolute.length() + 1;
  LPVOID remote_memory =
    VirtualAllocEx(process, nullptr, path_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!remote_memory) {
    LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
               << " (VirtualAllocEx failed)";
    CloseHandle(process);
    return 1;
  }

  // Write the DLL path into the allocated memory
  if (!WriteProcessMemory(process, remote_memory, dll_path_absolute.c_str(), path_size, nullptr)) {
    LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
               << " (WriteProcessMemory failed)";
    VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
    CloseHandle(process);
    return 1;
  }

  // Get the address of LoadLibraryA from kernel32.dll
  HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
  FARPROC load_library = GetProcAddress(kernel32, "LoadLibraryA");
  if (!load_library) {
    LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
               << " (GetProcAddress failed)";
    VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
    CloseHandle(process);
    return 1;
  }

  // Create a remote thread that calls LoadLibraryA with the DLL path
  HANDLE thread = CreateRemoteThread(
    process,
    nullptr,
    0,
    reinterpret_cast<LPTHREAD_START_ROUTINE>(load_library),
    remote_memory,
    0,
    nullptr
  );
  if (!thread) {
    LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
               << " (CreateRemoteThread failed)";
    VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
    CloseHandle(process);
    return 1;
  }

  // Wait for the injection to complete (10 second timeout like Cheat Engine)
  DWORD wait_result = WaitForSingleObject(thread, 10000);
  if (wait_result == WAIT_TIMEOUT) {
    LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
               << " (injection thread timed out)";
    CloseHandle(thread);
    VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
    CloseHandle(process);
    return 1;
  }

  // Get the exit code to determine if LoadLibrary succeeded
  DWORD exit_code = 0;
  if (GetExitCodeThread(thread, &exit_code)) {
    if (exit_code == 0) {
      // LoadLibrary failed (returned NULL)
      LOG(ERROR) << "Failed to inject " << dll_name << " into process " << pid
                 << " (LoadLibrary returned NULL)";
      CloseHandle(thread);
      VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
      CloseHandle(process);
      return 1;
    }
  }

  // Clean up handles and allocated memory
  CloseHandle(thread);
  VirtualFreeEx(process, remote_memory, 0, MEM_RELEASE);
  CloseHandle(process);

  std::cout << "Injected " << dll_name << " into process " << pid << std::endl;

  return 0;
}
