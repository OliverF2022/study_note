#include <thread>
#include <algorithm>
#include <vector>

template<typename Iterator, typename T>
struct accumulate_block
{
	void operator() (Iterator first, Iterator last, T& result)
	{
		//累加
		result = std::accumulate(first, last, result);
	}
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);
	if (!length)
		return init;

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
	//硬件线程允许数量（返回仅供参考），即核数
	unsigned long const hardware_threads = std::thread::hardware_concurrency();

	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
	//每个线程处理的块大小
	unsigned long const block_size = length / num_threads;

	std::vector<T> results(num_threads);
	std::vector<std::thread> threads(num_threads);

	Iterator block_start = first;
	for (unsigned long i = 0; i < (num_threads - 1); ++i)
	{
		Iterator block_end = block_start;
		//std::advance 将迭代器前移多个位置
		std::advance(block_end, block_size);
		threads[i] = std::thread(accumulate_block<Iterator, T>(),
			block_start, block_end, std::ref(results[i]));
		block_start = block_end;
	}
	//处理剩余数据
	accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
	//std::men_fn 把成员函数转为函数对象，使用对象进行绑定
	//std::bind包括mem_fn的功能，但不限于，是更为通用的解决方案
	std::for_each(threads.begin(), threads.end(), std::men_fn(&std::thread::join));

	return std::accumulate(results.begin(), results.end(), init);
}