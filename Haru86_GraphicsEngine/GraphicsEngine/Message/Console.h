#pragma once
#ifdef _DEBUG
static class Console
{
public:
	template<class Message, class... Variable>
	static void Log(Message&& message, Variable&&... variable) {
		_RPTN(_CRT_WARN, message, (variable)...);
	}
};

#endif // _DEBUG