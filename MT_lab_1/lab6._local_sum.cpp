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
mutex am;

void worker(const int num_threads)
{
	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		m.lock();
		sum = sum + 2;
		m.unlock();
	}

}

// atomic 연산은 락과 언락의 기능을 비유적으로 가지고 있다.
// 그러나 작동 위치와 방식은 완전히 다르다. 락과 언락은 os단에서 atomic은 cpu단에서 작동한다.
// 또한 atomic 연산은 원자성은 보장하지만 , 데이터의 흐름은 보장하지 않는다. 

std::atomic <int> a_sum = 0;
void atomic_worker(const int num_threads)
{
	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		// a_sum = a_sum + 2; - 결국 원자적연산을 보장하지 않는다. 
		a_sum += 2;
	}

}

// 최적화된 방법
// atomic은 결국 한 번에 한스레드만 일을 하도록 보장하므로
// 멀티쓰레드에서 동시에 작동하여 그 성능을 키우려면 지역변수를 사용하여 각 다른 쓰레드를 동시에 작동시키고
// 마지막으로 더하는 연산
int o_sum = 0;
void o_worker(const int num_threads)
{
	volatile int local_sum = 0;
	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		local_sum = local_sum + 2;
	}
	am.lock();
	o_sum = o_sum + local_sum;
	am.unlock();
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


	{

		for (int i = 1; i <= 16; i = i * 2) {
			o_sum = 0;
			std::vector<std::thread> threads;

			auto start_t = system_clock::now();

			for (int j = 0; j < i; ++j)
			{
				threads.emplace_back(o_worker, i);
			}
			for (auto& th : threads)
			{
				th.join();
			}

			auto end_t = system_clock::now();
			auto ex_t = end_t - start_t;
			auto ex_ms = duration_cast<milliseconds>(ex_t).count();

			cout << i << " Thread  optimal Sum = " << a_sum << " |  Time " << ex_ms << " ms\n";

		}
	}
}

//	Single thread Sum = 10000000		| Time 16 ms 
//	1 Thread Sum = 10000000				| Time 63 ms
//	2 Thread Sum = 10000000				| Time 69 ms
//	4 Thread Sum = 10000000				| Time 95 ms
//	8 Thread Sum = 10000000				| Time 156 ms
//	16 Thread Sum = 10000000			| Time 291 ms

//	Single thread Sum = 10000000		| Time 16 ms 
//	1 Thread  atomic Sum = 10000000		| Time 20 ms
//	2 Thread  atomic Sum = 10000000		| Time 49 ms
//	4 Thread  atomic Sum = 10000000		| Time 52 ms
//	8 Thread  atomic Sum = 10000000		| Time 58 ms
//	16 Thread  atomic Sum = 10000000	| Time 62 ms
// 
//	Single thread Sum = 10000000		| Time 16 ms 
//1  Thread  optimal Sum = 10000000		| Time 1 ms
//2  Thread  optimal Sum = 10000000		| Time 1 ms
//4  Thread  optimal Sum = 10000000		| Time 1 ms
//8  Thread  optimal Sum = 10000000		| Time 2 ms
//16 Thread  optimal Sum = 10000000		| Time 3 ms