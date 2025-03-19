//============================================================================
// Name        : MC1.cpp
// Author      : Alexander Lemutov
// Version     :
// Copyright   : Free copyright
// Description : MC1 Task1
//============================================================================

#include <iostream>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <random>
#include <thread>
#include <vector>

int workers = 5;
int machines = 3;

std::mutex mtx;
std::atomic<int> available_machines(machines);
std::condition_variable cv;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> work_time(1, 3);

void work(int id){
	std::unique_lock<std::mutex> lock(mtx);
	cv.wait(lock, [] {return available_machines.load() > 0; });
	available_machines--;
	std::cout << "Рабочий " << id << " начал работать на станке. Доступно свободных станков: " << available_machines.load() << std::endl;
	lock.unlock();

	std::this_thread::sleep_for(std::chrono::seconds(work_time(gen)));

	lock.lock();
	available_machines++;
	std::cout << "Рабочий " << id << " закончил работать на станке. Доступно свободных станков: " << available_machines.load() << std::endl;
	lock.unlock();
	cv.notify_one();
}
 /*
  * Блок выше имитиурет работу одного из рабочих на станке. Когда один из потоков попадает в функцию work,
  * он вызывает захват мьютекса с помощью unique_lock, причем захват выполнится, только когда количество свободных
  * станков (available_machines) будет строго больше нуля. При захвате мьютекса этот поток уменьшает кол-во
  * свободных станков, выводит информацию в консоль и разблокирует мьютекс. Затем, когда рабочий завершит работу за
  * станком, мьютекс снова захватывается и кол-во свободных станков увеличивается на 1, выводится информация об этом,
  * мьютекс разблокируется, условной переменной отправляется уведомление (после чего мьютекс может быть захвачен
  * другим потоком).
  */
int main() {
	std::vector<std::thread> threads_workers;

	for (int i = 0; i < workers; ++i) {
		threads_workers.emplace_back(work, i+1);
	}

	for (auto &t: threads_workers) t.join();

	return 0;
}

/*
 * Блок выше содержит вектор из потоков, имитирующих рабочих, цикл, в котором запускаются эти потоки, и ожидание
 * завершения работы всех потоков.
 */
