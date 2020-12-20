#include "Input.h"

#include "RenderState.h"

glm::vec2 Input::s_DPCM;
static constexpr float INCH_TO_CM = 2.54f;

void Input::Initialize() {
	SDL_GetDisplayDPI(0, nullptr, &s_DPCM[0], &s_DPCM[1]);
	s_DPCM /= INCH_TO_CM;
}

bool Input::KeyIsDown(SDL_Scancode key) {
	const Uint8* state = SDL_GetKeyboardState(NULL);
	return state[key];
}

glm::ivec2 Input::MouseInPixels() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	return glm::ivec2(x, y) - RenderState::InAppRenderArea().topLeft();
}

glm::vec2 Input::MouseInCentimeters() {
	return static_cast<glm::vec2>(MouseInPixels()) * s_DPCM;
}

glm::vec2 Input::MouseInNormalizedRatioSpace() {
	glm::vec2 pos = MouseInPixels();
	pos /= RenderState::InAppRenderArea().height();
	pos.y = 1.0f - pos.y;
	pos *= 2.0f;
	pos -= glm::vec2(RenderState::InAppRenderArea().aspectRatio(), 1.0f);
	return pos;
}