struct ButtonState {
	bool down;
	bool changed;
};

enum {
	ButtonUp,
	ButtonDown,
	ButtonLeft,
	ButtonRight,
	ButtonEsc,
};

struct Input {
	ButtonState buttons[5];
};