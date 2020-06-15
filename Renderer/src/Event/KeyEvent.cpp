#include "Rendererpch.h"
#include "KeyEvent.h"

KeyEvent::KeyEvent()
{
}

KeyEvent::KeyEvent(KEY key, STATE state) :
	Key(key), State(state)
{
}

KeyEvent::~KeyEvent()
{
	Key = KEY::A;
	State = STATE::PRESS;
}
