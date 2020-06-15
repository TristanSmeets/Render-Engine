#pragma once
#include <GLFW/glfw3.h>

struct KeyEvent
{
	enum KEY { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z };
	enum STATE { PRESS, RELEASE, REPEAT };
public:
	KeyEvent();
	KeyEvent(KEY key, STATE state);
	~KeyEvent();

	KEY Key;
	STATE State;
};