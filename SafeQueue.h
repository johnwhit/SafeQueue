/* SafeQueue.h
 * -----------
 * Thread safe queue based on Windows C++/CLI libraries. Designed
 * to be used as a producer/consumer queue.
 *
 * Created by John Whitworth on 6/30/14.
 */

#pragma once

#include <queue>
#include <windows.h>

template <typename T> class SafeQueue
{
private:
    std::queue<T> queue;
    CRITICAL_SECTION CritSection;
    CONDITION_VARIABLE ConditionVar;
    
public:
	SafeQueue(void) {
		InitializeCriticalSection(&CritSection);
		InitializeConditionVariable(&ConditionVar);
	}

	~SafeQueue(void) {
		DeleteCriticalSection(&CritSection);
	}

    void add(T item) {
        EnterCriticalSection(&CritSection);
        queue.push(item);
        LeaveCriticalSection(&CritSection);
        WakeAllConditionVariable(&ConditionVar);
    }
    
    T get() {
        EnterCriticalSection(&CritSection);
        if (queue.empty()) {
            SleepConditionVariableCS(&ConditionVar, &CritSection, INFINITE);
        }
        T item = queue.front();
        queue.pop();
		LeaveCriticalSection(&CritSection);
        return item;
    }
    
    int size() {
        EnterCriticalSection(&CritSection);
        int size = queue.size();
        LeaveCriticalSection(&CritSection);
        return size;
    }

	bool empty() {
		return (size() == 0);
	}
};