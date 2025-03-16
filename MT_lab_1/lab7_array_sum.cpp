#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>

using namespace std;
using namespace chrono;
volatile int sum;
volatile int sum2;
volatile int sum3;
mutex m;
mutex am;
mutex bm;
mutex cm;

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
		a_sum += 2;
	}

}

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

volatile int sum_array[32];
void o2_worker(const int num_threads, const int thread_id)
{
	sum_array[thread_id] = 0;
	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		sum_array[thread_id] = sum_array[thread_id] + 2;
	}
	bm.lock();
	sum2 = sum2 + sum_array[thread_id];	
	bm.unlock();
}

struct CACHE_INT {
	alignas(64)	volatile int sum;
};



CACHE_INT sum_array2[32];

void o3_worker(const int num_threads, const int thread_id)
{
	volatile int &asum = sum_array2[thread_id].sum;
	asum = 0;

	for (auto i = 0; i < 5000000 / num_threads; ++i) {
		asum = asum + 2;
	}
	cm.lock();
	sum3 = sum3 + asum;
	cm.unlock();
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

	{

		for (int i = 1; i <= 16; i = i * 2) {
			sum2 = 0;
			std::vector<std::thread> threads;

			auto start_t = system_clock::now();

			for (int j = 0; j < i; ++j)
			{
				threads.emplace_back(o2_worker, i , j);
			}
			for (auto& th : threads)
			{
				th.join();
			}

			auto end_t = system_clock::now();
			auto ex_t = end_t - start_t;
			auto ex_ms = duration_cast<milliseconds>(ex_t).count();

			cout << i << " Thread  Array Sum = " << sum2 << " |  Time " << ex_ms << " ms\n";

		}
	}

	{

		for (int i = 1; i <= 16; i = i * 2) {
			sum3 = 0;
			std::vector<std::thread> threads;

			auto start_t = system_clock::now();

			for (int j = 0; j < i; ++j)
			{
				threads.emplace_back(o3_worker, i, j);
			}
			for (auto& th : threads)
			{
				th.join();
			}

			auto end_t = system_clock::now();
			auto ex_t = end_t - start_t;
			auto ex_ms = duration_cast<milliseconds>(ex_t).count();

			cout << i << " Thread  Array2 Sum = " << sum3 << " |  Time " << ex_ms << " ms\n";

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
//1  Thread  optimal Sum = 10000000		| Time 1 ms
//2  Thread  optimal Sum = 10000000		| Time 1 ms
//4  Thread  optimal Sum = 10000000		| Time 1 ms
//8  Thread  optimal Sum = 10000000		| Time 2 ms
//16 Thread  optimal Sum = 10000000		| Time 3 ms

//1 Thread  Array Sum = 10000000 | Time 8 ms
//2 Thread  Array Sum = 10000000 | Time 4 ms
//4 Thread  Array Sum = 10000000 | Time 3 ms
//8 Thread  Array Sum = 10000000 | Time 3 ms
//16 Thread  Array Sum = 10000000 | Time 4 ms

//1 Thread  Array2 Sum = 10000000 | Time 1 ms
//2 Thread  Array2 Sum = 10000000 | Time 1 ms
//4 Thread  Array2 Sum = 10000000 | Time 1 ms
//8 Thread  Array2 Sum = 10000000 | Time 1 ms
//16 Thread  Array2 Sum = 10000000 | Time 3 ms