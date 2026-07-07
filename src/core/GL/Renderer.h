#pragma once
#include <glad/glad.h>
class VertexArray;
class ElementBuffer;
class Shader;


class Renderer {
public:

	static Renderer& Instance() {
		static Renderer instance;
		return instance;
	}
	void SetCullFace(bool onoff);
	void SetPolygonMode(GLenum face, GLenum mode);
	void Draw(const VertexArray& vao, const Shader& shader);
	void DrawInstanced(const VertexArray& vao, const Shader& shader, int instanceCount);
	void DrawLine(const VertexArray& vao, const Shader& shader);

	void Clear(unsigned int buffers);

	void setClearColor(float r, float g, float b, float a);
	void setClearDepth(float d);

private:
	Renderer() = default;
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

};