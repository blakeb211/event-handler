#include <chrono>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;
using namespace std::chrono_literals;

mutex g_mutex;
queue<int> events;

atomic<bool> kill_bg_thread{false};

void DedicatedBackground()
{
	for (;;)
	{
		{
			const lock_guard<mutex> lock(g_mutex);
			if (!events.empty())
			{
				auto result = events.front();
				cout << result << "\n";
				events.pop();
			}
			else
			{
				if (kill_bg_thread)
					break;
			}
		}
		std::this_thread::sleep_for(2ms);
	}
}

int main()
{
	std::thread background{DedicatedBackground};

	for (int i = 0; i < 100; i++)
	{
		{
			const lock_guard<mutex> lock(g_mutex);
			events.push(i);
		}
		std::this_thread::sleep_for(1ms);
	}
	kill_bg_thread = true;
	background.join();
	return 0;
}
