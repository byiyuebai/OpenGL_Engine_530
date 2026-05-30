#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include <glad/glad.h>
void Renderer::SetDrawMode(DrawMode mode)
{
	switch (mode) {
	case DrawMode::Opaque:
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE); // 启用深度写入
		glDisable(GL_BLEND);
		break;
	case DrawMode::Translucent:
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE); // 禁止深度写入
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 标准 alpha 混合
		break;
	default:
		break;
	}
}
void Renderer::SetCullFace(bool onoff)
{
	if (onoff) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // 剔除背面
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}
void Renderer::Draw(const VertexArray& vao, const Shader& shader) {

	shader.use();
	vao.Bind();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//线框模式
	glDrawElements(GL_TRIANGLES, vao.GetCurrentIndexCount(), GL_UNSIGNED_INT, 0);
}
void Renderer::DrawInstanced(
	const VertexArray& vao,
	const Shader& shader,
	int instanceCount
) {
	shader.use();
	vao.Bind();

	glDrawElementsInstanced(
		GL_TRIANGLES,
		vao.GetCurrentIndexCount(),
		GL_UNSIGNED_INT,
		0,
		instanceCount
	);
}
void Renderer::DrawLine(const VertexArray& vao, const Shader& shader) {

	shader.use();
	vao.Bind();

	glDrawElements(GL_LINES, vao.GetCurrentIndexCount(), GL_UNSIGNED_INT, 0);
}
void Renderer::DrawTranslucent(const VertexArray& vao, const Shader& shader) {

	shader.use();
	vao.Bind();

	//提交绘制

}

void Renderer::Clear(unsigned int buffers) {
	glClear(buffers);
}

void Renderer::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void Renderer::setClearDepth(float d)
{
	glClearDepth(d);
}
