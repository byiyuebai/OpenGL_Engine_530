#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include <glad/glad.h>


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
void Renderer::SetPolygonMode(GLenum face, GLenum mode)
{
/*
face	GL_FRONT	只影响正面多边形
		GL_BACK	只影响背面多边形
		GL_FRONT_AND_BACK	最常用，影响所有多边形

mode	GL_FILL	默认值，填充整个多边形（实体渲染）
		GL_LINE	只绘制多边形的边缘线框
		GL_POINT	只绘制多边形的顶点（点云效果）
 */
	glPolygonMode(face, mode);
}
void Renderer::Draw(const VertexArray& vao, const Shader& shader) {

	shader.use();
	vao.Bind();
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
