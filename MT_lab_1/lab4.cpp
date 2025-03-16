#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>

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
		
		for (int i = 1; i <= 64; i = i * 2) {
			sum = 0;
			std::vector<std::thread> threads;

			auto start_t = system_clock::now();
		
			for (int j = 0; j < i; ++j)
			{
				threads.emplace_back(worker, i);
			}
			for(auto & th : threads)
			{
				th.join();
			}

			auto end_t = system_clock::now();
			auto ex_t = end_t - start_t;
			auto ex_ms = duration_cast<milliseconds>(ex_t).count();

			cout << i << " Thread Sum = " << sum << " |  Time " << ex_ms << " ms\n";
			

		}
	}

}
// 디버그
//Single thread Sum = 10000000	| Time 9 ms
//1 Thread Sum = 10000000		| Time 159 ms
//2 Thread Sum = 10000000		| Time 160 ms
//4 Thread Sum = 10000000		| Time 188 ms
//8 Thread Sum = 10000000		| Time 280 ms
//16 Thread Sum = 10000000		| Time 552 ms

// 릴리즈
//Single thread Sum = 10000000	| Time 0 ms - 릴리즈는 ai가 깡으로 1억 넣음 
//1 Thread Sum = 10000000		| Time 87 ms
//2 Thread Sum = 10000000		| Time 87 ms
//4 Thread Sum = 10000000		| Time 107 ms
//8 Thread Sum = 10000000		| Time 191 ms
//16 Thread Sum = 10000000		| Time 391 ms

// 릴리즈
//Single thread Sum = 10000000	| Time 15 ms - volatile - 최적화하지마 
//1 Thread Sum = 10000000		| Time 63 ms
//2 Thread Sum = 10000000		| Time 73 ms
//4 Thread Sum = 10000000		| Time 91 ms
//8 Thread Sum = 10000000		| Time 185 ms
//16 Thread Sum = 10000000		| Time 365 ms