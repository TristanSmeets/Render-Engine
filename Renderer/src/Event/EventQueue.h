#pragma once
#include "Rendererpch.h"
#include <assert.h>

template<class T>
class EventQueue
{
public:
	static void AddListener(const std::function<void(const T&)>& function);
	static void QueueEvent(const T& TEvent);
	static void Dispatch();
private:
	static const int maxQueueSize = 20;
	static const int maxListeners = 30;
	static inline std::function<void(const T&)> listeners[maxListeners];	
	static inline T events[maxQueueSize];
	static inline int eventsQueued = 0;
	static inline int activeListeners = 0;

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
