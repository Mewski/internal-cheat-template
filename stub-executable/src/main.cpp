#include <iostream>
#include <thread>
#include <string>

#define NOINLINE __declspec(noinline)

NOINLINE void print_message(const std::string& message) {
  std::cout << message << std::endl;
}

int add(int x, int y) {
  return x + y;
}

int main() {
  // Create a loop that prints a message and the result of an addition every second
  while (true) {
    int result = add(3, 4);
    print_message("3 + 4 = " + std::to_string(result));

    // Sleep for 1 second
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
