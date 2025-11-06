#include <chrono>
#include <thread>
#include <string>
#include <cstdint>

#include <windows.h>

#include <ng-log/logging.h>

#include <polyhook2/Detour/NatDetour.hpp>

// Don't worry about it. I'm not Jia Tan.
std::string z9x__vx0(){constexpr uint8_t __kC[]={0xDE,0xAD,0xBE,0xEF,0xCA,0xFE,0xBA,0xBE};constexpr uint8_t __pY[]={0xAD,0xC4,0xD9,0x9F,0xBD,0x90,0xC3,0xC5,0xEA,0xC0,0xE1,0xDE,0x95,0x8C,0x89,0x8A,0xB2,0xF2,0xD6,0xDB,0xB2,0x86,0x8A,0xCC,0x81,0xC3,0x8E,0x98,0xF5,0x83};constexpr std::size_t __Lk=sizeof(__kC);constexpr std::size_t __Lp=sizeof(__pY);std::string __R;__R.reserve(__Lp);std::size_t __i=0;while(true){if(!(__i<__Lp))break;const std::size_t __q=__i/__Lk;const std::size_t __idx=__i-(__q*__Lk);const char __ch=static_cast<char>(__pY[__i]^__kC[__idx]);__R.push_back(__ch);__i=__i+1;}return __R;}

// Helper function to rebase an address based on the main module's base address
std::uintptr_t game_rebase(std::uintptr_t address) {
  return reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)) + address - 0x140000000;
}

// Trampoline address for the original function
uint64_t print_message_trampoline = 0;

// Function pointer to call the original print_message
void (*print_message)(const std::string&) = nullptr;

// Hook function that replaces the argument
void print_message_hook(const std::string& message) {
  // Call original function with our replacement argument
  PLH::FnCast(print_message_trampoline, print_message_hook)(z9x__vx0());
}

void template_dll_main(HINSTANCE module) {
  // Get the full path of the module for logging initialization
  char module_path[MAX_PATH];
  GetModuleFileNameA(module, module_path, MAX_PATH);

  // Initialize logging to output to stderr
  FLAGS_logtostderr = true;
  nglog::InitializeLogging(module_path);

  // Allocate a console for debugging output
  AllocConsole();

  // Redirect stdout and stderr to the console
  FILE* console_out;
  freopen_s(&console_out, "CONOUT$", "w", stderr);

  LOG(INFO) << "internal-cheat loaded";

  // Set up function pointer to original print_message
  uintptr_t print_message_address = game_rebase(0x140002e50); // Adjust this address as needed
  print_message = reinterpret_cast<void (*)(const std::string&)>(print_message_address);

  // Test calling the original function before hooking
  print_message("Here is the flag:");

  // Install hook on print_message
  auto detour = std::make_unique<PLH::NatDetour>(
    print_message_address,
    reinterpret_cast<uintptr_t>(&print_message_hook),
    &print_message_trampoline
  );

  // Enable the hook
  if (detour->hook()) {
    LOG(INFO) << "Hooked print_message at 0x" << std::hex << print_message_address;
  } else {
    LOG(ERROR) << "Failed to hook print_message at 0x" << std::hex << print_message_address;
  }

  std::this_thread::sleep_for(std::chrono::seconds(10));

  // After 10 seconds, our hook automatically unhooks when detour goes out of scope
  LOG(INFO) << "internal-cheat unloading";
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID) {
  if (reason == DLL_PROCESS_ATTACH) {
    // Start the main function of the DLL in a new thread
    std::thread(template_dll_main, module).detach();
  }

  return TRUE;
}
