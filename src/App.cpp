#include "App.h"

#include <Cool/App/RenderState.h>
#include <Cool/App/Input.h>
#include <Cool/Time/Time.h>
#include <Cool/Serialization/JsonFile.h>

App::App(Window& mainWindow)
	: m_mainWindow(mainWindow), _camera_trackball_controller(m_camera), _camera_perspective_controller(m_camera)
{
	Serialization::from_json(*this, File::root_dir() + "/last-session-cache.json");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Please note that the blending is WRONG for the alpha channel (but it doesn't matter in most cases) The correct call would be glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE) a.k.a. newAlpha = srcAlpha + dstAlpha - srcAlpha*dstAlpha
	RenderState::setExportSize(1920, 1080); // TODO remove me
}

App::~App() {
	Serialization::to_json(*this, File::root_dir() + "/last-session-cache.json", "App");
}

void App::render() {
	m_renderer.begin();	
	{
		glClearColor(m_bgColor.r, m_bgColor.g, m_bgColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (_shader_manager->is_valid()) {
			_shader_manager->setup_for_rendering(m_camera, _camera_perspective_controller.focal_length());
			m_renderer.render();
		}
	}
	m_renderer.end();
}

void App::update() {
	_shader_manager->update();
	_camera_trackball_controller.update();
	render();
	m_exporter.update(m_renderer.renderBuffer());
	Time::update();
}

void App::ImGuiWindows() {
	ImGui::Begin("Time");
	Time::imgui_timeline();
	ImGui::End();
	m_exporter.imgui_window_export_image_sequence();
	Log::ToUser::imgui_console_window();
	if (!RenderState::IsExporting()) {
		//
		_shader_manager.ImGui_windows();
		//
		m_exporter.imgui_window_export_image([this]() {render(); }, m_renderer.renderBuffer());
		//
#ifndef NDEBUG
		if (m_bShow_Debug) {
			ImGui::Begin("Debug", &m_bShow_Debug);
			ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
			ImGui::SameLine();
			bool capFramerate = m_mainWindow.isVSyncEnabled();
			if (ImGui::Checkbox("Cap framerate", &capFramerate)) {
				if (capFramerate)
					m_mainWindow.enableVSync();
				else
					m_mainWindow.disableVSync();
			}
			ImGui::Text("Rendering Size : %d %d", RenderState::Size().width(), RenderState::Size().height());
			ImGui::Text("Mouse Position in Render Area : %.0f %.0f screen coordinates", Input::MouseInScreenCoordinates().x, Input::MouseInScreenCoordinates().y);
			ImGui::Text("Mouse Position Normalized : %.2f %.2f", Input::MouseInNormalizedRatioSpace().x, Input::MouseInNormalizedRatioSpace().y);
			ImGui::Text("Camera Transform matrix :");
			glm::mat4 m = m_camera.transform_matrix();
			ImGui::Text("%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f\n%.2f %.2f %.2f %.2f",
				m[0][0], m[1][0], m[2][0], m[3][0],
				m[0][1], m[1][1], m[2][1], m[3][1],
				m[0][2], m[1][2], m[2][2], m[3][2],
				m[0][3], m[1][3], m[2][3], m[3][3]
			);
			ImGui::ColorEdit3("Background Color", glm::value_ptr(m_bgColor));
			ImGui::Checkbox("Show Demo Window", &m_bShow_ImGuiDemo);
			ImGui::End();
		}
		if (m_bShow_ImGuiDemo) // Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			ImGui::ShowDemoWindow(&m_bShow_ImGuiDemo);
#endif
	}
}

void App::ImGuiMenus() {
	if (ImGui::BeginMenu("Export")) {
		m_exporter.imgui_menu_items();
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Windows")) {
		Log::ToUser::imgui_toggle_console();
#ifndef NDEBUG
		ImGui::Separator();
		ImGui::Checkbox("Debug", &m_bShow_Debug);
#endif
		ImGui::EndMenu();
	}
}

void App::onKeyboardEvent(int key, int scancode, int action, int mods) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantTextInput) {
		if (action == GLFW_RELEASE) {
			if (Input::MatchesChar("s", key) && (mods & GLFW_MOD_CONTROL)) {
				m_exporter.open_window_export_image(true);
			}
			if (Input::MatchesChar("e", key) && (mods & GLFW_MOD_CONTROL)) {
				m_exporter.open_window_export_image_sequence(true);
			}
			if (Input::MatchesChar("z", key) && (mods & GLFW_MOD_CONTROL)) {
				ParametersHistory::get().move_backward();
			}
			if (Input::MatchesChar("y", key) && (mods & GLFW_MOD_CONTROL)) {
				ParametersHistory::get().move_forward();
			}
		}
	}
}

void App::onMouseButtonEvent(int button, int action, int mods) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
		if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (action == GLFW_PRESS) {
				_camera_trackball_controller.on_wheel_down(mods);
			}
			else if (action == GLFW_RELEASE) {
				_camera_trackball_controller.on_wheel_up();
			}
		}
	}
}

void App::onScrollEvent(double xOffset, double yOffset) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {
		_camera_trackball_controller.on_wheel_scroll(yOffset);
	}
}

void App::onMouseMoveEvent(double xpos, double ypos) {
	if (!RenderState::IsExporting() && !ImGui::GetIO().WantCaptureMouse) {

	}
}