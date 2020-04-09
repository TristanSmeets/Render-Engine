#pragma once
#include "Rendererpch.h"
#include <assert.h>

template<class T>
class EventQueue
{
public:
	void AddListener(const std::function<void(const T&)>& function);
	void QueueEvent(const T& TEvent);
	void Dispatch();
private:

	static const int maxQueueSize = 20;
	static const int maxListeners = 30;
	std::function<void(const T&)> listeners[maxListeners];
	T events[maxQueueSize];
	int eventsQueued = 0;
	int activeListeners = 0;

};

template<class T>
inline void EventQueue<T>::AddListener(const std::function<void(const T&)>& function)
{
	assert(activeListeners < maxListeners);
	listeners[activeListeners++] = function;
}

template<class T>
inline void EventQueue<T>::QueueEvent(const T & TEvent)
{
	assert(eventsQueued < maxQueueSize);
	events[eventsQueued++] = TEvent;
}

template<class T>
inline void EventQueue<T>::Dispatch()
{
	for (int i = 0; i < eventsQueued; ++i)
	{
		for (int j = 0; j < activeListeners; ++j)
		{
			listeners[j](events[i]);
		}
	}
	eventsQueued = 0;
}
