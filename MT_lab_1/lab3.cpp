#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
using namespace std;
using namespace chrono;
int sum;
mutex m;
void worker()
{
	for (auto i = 0; i < 5000000 / 2; ++i) {
		m.lock();
		sum = sum + 2;
		m.unlock();
	}

}

int main()
{
	//50000000 ¹Ý¶òÀ¸·Î 
	auto start_t = high_resolution_clock::now();

	char ch;
	std::cin >> ch;

	std::thread t1 = thread{ worker };
	std::thread t2 = thread{ worker };

	t1.join();
	t2.join();

	auto du = high_resolution_clock::now() - start_t;
	//cout << "Time " << duration_cast<milliseconds>(du).count() << " ms\n"; // 3ms
	cout << "Sum = " << sum << "\n";
}