#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

using namespace std;
using namespace chrono;
volatile int sum;
mutex m;


void worker(const int num_threads)
{
	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		m.lock();
		sum = sum + 2;
		m.unlock();
	}

}

std::atomic <int> a_sum = 0;
void atomic_worker(const int num_threads)
{
	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		// a_sum = a_sum + 2;
		a_sum += 2;
	}

}

int main()
{
	{
		auto start_t = system_clock::now();

		for (auto i = 0; i < 5000000; ++i)
		{
			sum = sum + 2;
		}
		auto end_t = system_clock::now();
		auto ex_t = end_t - start_t;
		auto ex_ms = duration_cast<milliseconds>(ex_t).count();
		cout << "Single thread Sum = " << sum << " |  Time " << ex_ms << " ms\n";
	}

	{

		for (int i = 1; i <= 16; i = i * 2) {
			sum = 0;
			std::vector<std::thread> threads;

			auto start_t = system_clock::now();

			for (int j = 0; j < i; ++j)
			{
				threads.emplace_back(worker, i);
			}
			for (auto& th : threads)
			{
				th.join();
			}

			auto end_t = system_clock::now();
			auto ex_t = end_t - start_t;
			auto ex_ms = duration_cast<milliseconds>(ex_t).count();

			cout << i << " Thread Sum = " << sum << " |  Time " << ex_ms << " ms\n";


		}
	}

	{

		for (int i = 1; i <= 16; i = i * 2) {
			a_sum = 0;
			std::vector<std::thread> threads;

			auto start_t = system_clock::now();

			for (int j = 0; j < i; ++j)
			{
				threads.emplace_back(atomic_worker, i);
			}
			for (auto& th : threads)
			{
				th.join();
			}

			auto end_t = system_clock::now();
			auto ex_t = end_t - start_t;
			auto ex_ms = duration_cast<milliseconds>(ex_t).count();

			cout << i << " Thread  atomic Sum = " << a_sum << " |  Time " << ex_ms << " ms\n";


		}
	}


}
// a_sum = a_sum + 2;
//Single thread Sum = 10000000 | Time 10 ms
//1 Thread Sum = 10000000 | Time 66 ms
//2 Thread Sum = 10000000 | Time 73 ms
//4 Thread Sum = 10000000 | Time 94 ms
//8 Thread Sum = 10000000 | Time 152 ms
//16 Thread Sum = 10000000 | Time 304 ms
//1 Thread  atomic Sum = 10000000 | Time 36 ms
//2 Thread  atomic Sum = 7784596 | Time 77 ms
//4 Thread  atomic Sum = 5799064 | Time 79 ms
//8 Thread  atomic Sum = 3981940 | Time 88 ms
//16 Thread  atomic Sum = 2374260 | Time 103 ms

// a_sum += 2;
//Single thread Sum = 10000000 | Time 17 ms
//1 Thread Sum = 10000000 | Time 65 ms
//2 Thread Sum = 10000000 | Time 71 ms
//4 Thread Sum = 10000000 | Time 103 ms
//8 Thread Sum = 10000000 | Time 161 ms
//16 Thread Sum = 10000000 | Time 367 ms
//1 Thread  atomic Sum = 10000000 | Time 22 ms
//2 Thread  atomic Sum = 10000000 | Time 48 ms
//4 Thread  atomic Sum = 10000000 | Time 51 ms
//8 Thread  atomic Sum = 10000000 | Time 54 ms
//16 Thread  atomic Sum = 10000000 | Time 63 ms