#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

void f();
struct F {
	void operator()();
};

int sum;

int main()
{
	auto start_t = high_resolution_clock::now();
	for (auto i = 0; i < 50000000; ++i) sum = sum + 2;
	auto du = high_resolution_clock::now() - start_t;

	std::cout << "Sum1 = " << sum << endl;
	cout << "[Sum1] Time " << duration_cast<milliseconds>(du).count() << " ms\n"; // 6ms

	cout << _Thrd_hardware_concurrency() << endl; // 20

//	std::thread t1{ f }; // f() executes in separate thread
//	std::thread t2{ F() }; // F()() executes in separate thread
}