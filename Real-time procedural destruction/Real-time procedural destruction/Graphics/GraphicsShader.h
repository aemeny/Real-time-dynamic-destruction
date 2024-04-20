#include <iostream>
#include <GL/glew.h>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "GraphicsModel.h"
#include "GraphicsTexture.h"
#include "Vao.h"

namespace Renderer
{

	struct Shader
	{
		Shader(std::string _vertFileName, std::string _fragFileName);
		Shader(std::string _vertFileName, std::string _fragFileName, std::string _newLightPos);
		~Shader();
		void render(std::shared_ptr<Model> _model, std::shared_ptr<Texture> _texture, bool _skyBox = false);
		void renderQuad(std::shared_ptr<Vao> _Vao, std::shared_ptr<Texture> _texture);
		void renderQuad(std::shared_ptr<Vao> _Vao);
		void renderLine(std::shared_ptr<Vao> _Vao, int _numOfLines);

		void bindShader(glm::mat4 _matrix, const GLchar* _type);
		GLuint getId() { return m_programId; };

		std::string m_newLightPos;
	private:

		void link();
		GLuint createShader(std::string _fileName, GLuint _type);
		std::string readFile(std::string _fileName);

		std::string m_fileName;
		GLuint m_programId;
		GLuint m_fragShader;
		GLuint m_vertShader;
	};

}
