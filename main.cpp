#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

std::mutex g_mutex;
std::queue<int> g_events;
std::atomic<bool> g_bg_thread_can_end{false};

void HandleEvents()
{
	using std::mutex, std::chrono_literals::operator""ms;
	using std::this_thread::sleep_for, std::lock_guard, std::cout;

	for (;;)
	{
		{
			const lock_guard<mutex> lock(g_mutex);
			if (!g_events.empty())
			{
				auto result = g_events.front();
				cout << result << "\n";
				g_events.pop();
			}
			else
			{
				if (g_bg_thread_can_end)
					break;
			}
		}
		sleep_for(2ms);
	}
}

int main()
{
	using std::lock_guard, std::chrono_literals::operator""ms;
	using std::thread, std::mutex, std::this_thread::sleep_for;

	thread bg_thread{HandleEvents};

	for (int i = 0; i < 100; i++)
	{
		{
			const lock_guard<mutex> lock(g_mutex);
			g_events.push(i);
		}
		sleep_for(1ms);
	}

	g_bg_thread_can_end = true;
	bg_thread.join();
	return 0;
}
