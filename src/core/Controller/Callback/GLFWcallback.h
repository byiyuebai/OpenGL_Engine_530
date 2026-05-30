#include <GLFW/glfw3.h>
#include "input_types.h"
class GLFWcallback {
public:
	GLFWcallback(GLFWwindow* window);
	~GLFWcallback();
private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		
	static KeyCode GLFWkey_to_mytype(int key);
	static MouseButton GLFWmouse_button_to_mytype(int button);
	static InputAction GLFWaction_to_mytype(int action);


	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};