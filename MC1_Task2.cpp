//============================================================================
// Name        : MC1_Task2.cpp
// Author      : Alexander Lemutov
// Version     :
// Copyright   : Free copyright
// Description : MC1, Task 2
//============================================================================

#include <iostream>
#include <random>
#include <semaphore>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>


const int work_piece = 10;
const int machines = 3;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> work_time(1, 2);

std::counting_semaphore<machines> sem_machines(machines);
std::counting_semaphore<work_piece> sem_work_pieces(work_piece);

std::mutex output_mutex;

void edit(int id){
	sem_work_pieces.acquire();

	{
		std::lock_guard<std::mutex> lock(output_mutex);
		std::cout << "Заготовка " << id << " взята со склада. " << std::endl;
	}

	sem_machines.acquire();

	{
		std::lock_guard<std::mutex> lock(output_mutex);
		std::cout << "Заготовка " << id << " обрабатывается. " << std::endl;
	}

	std::this_thread::sleep_for(std::chrono::seconds(work_time(gen)));

	{
		std::lock_guard<std::mutex> lock(output_mutex);
		std::cout << "Заготовка " << id << " обработана." << std::endl;
	}

	sem_machines.release();
	sem_work_pieces.release();

	{
		std::lock_guard<std::mutex> lock(output_mutex);
		std::cout << "Заготовка " << id << " возвращена на склад." << std::endl;
	}
}

/*
 * Блок выше имитирует обработку заготовки. Сначала захватывается ресурс "заготовка" (т.к. их 10, то на них очереди
 * не будет, их всегда достаточно), затем выводится сообщение о взятии заготовки со склада (с захватом мьютекса на
 * вывод, иначе появляется лишняя конкатенация строк), затем аналогично захватывается ресурс "машина" и выводится
 * сообщение о начале обработки. После этого поток останавливается на 1-2 сек, затем выводится сообщение о завершении
 * обработки (с захватом мьютекса вывода), освобождаются семафоры на машины и заготовки, выводится сообщение
 * о возвращении заготовки на склад (с захватом мьютекса вывода)
 */

int main() {
	std::vector<std::thread> threads_work_pieces;

	for (int i = 0; i < work_piece; ++i) {
		threads_work_pieces.emplace_back(edit, i+1);
	}

	for (auto &t: threads_work_pieces) t.join();

	return 0;
}

/*
 * Блок выше имитирует 10 заготовок, которые будут обрабатываться на трёх машинах. Создается вектор потоков
 * (10 потоков), которые последовательно запускаются (им передается функция, имитирующая обработку заготовки),
 * затем ожидается завершение всех потоков.
 */
