// 读取当前绑定的 EBO 索引数据，返回索引数组
inline std::vector<unsigned int> readIndexBufferData()
{
	GLint ebo;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);
	if (ebo == 0) return {};

	GLint size = 0;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	std::vector<unsigned int> indices(size / sizeof(unsigned int));
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices.data());

	return indices;
}
// 读取当前绑定的 VBO 顶点数据
inline std::vector<Vertex> readVertexBufferData()
{
	GLint vbo;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbo);
	if (vbo == 0) return {};

	GLint size = 0;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	std::vector<Vertex> vertices(size / sizeof(Vertex));
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices.data());

	return vertices;
}
inline void debugPrintVAOData()
{
	std::wcout << L"===== [VAO 数据回读调试] =====" << std::endl;

	auto indices = readIndexBufferData();
	auto vertices = readVertexBufferData();

	std::wcout << L"顶点数量: " << vertices.size() << std::endl;
	std::wcout << L"索引数量: " << indices.size() << std::endl;

	if (!vertices.empty()) {
		std::wcout << L"前 5 个顶点：" << std::endl;
		for (int i = 0; i < std::min(5, (int)vertices.size()); i++) {
			auto& v = vertices[i];
			wprintf(
				L"[%d] Pos(%.2f, %.2f, %.2f) UV(%.2f, %.2f)\n",
				i,
				v.Position.x, v.Position.y, v.Position.z,
				v.TexCoords.x, v.TexCoords.y
			);
		}
	}

	if (!indices.empty()) {
		std::wcout << L"前 12 个索引：";
		for (int i = 0; i < std::min(12, (int)indices.size()); i++) {
			std::wcout << indices[i] << L" ";
		}
		std::wcout << std::endl;
	}
	std::wcout << L"==============================" << std::endl;
}