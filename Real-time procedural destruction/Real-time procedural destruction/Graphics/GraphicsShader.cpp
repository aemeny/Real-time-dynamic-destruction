#include "GraphicsShader.h"

namespace Renderer
{
	/* Constructor for the shader taking in a vertex and fragment shader file location */
	Shader::Shader(std::string _vertFileName, std::string _fragFileName) :
		m_programId(glCreateProgram()), m_newLightPos("vec3 lightPos = vec3(0, 0, 0);")
	{
		//calling create shader on each fragment and vertex shader giving it the correct type
		m_vertShader = createShader(_vertFileName, GL_VERTEX_SHADER);
		m_fragShader = createShader(_fragFileName, GL_FRAGMENT_SHADER);
		//linking attribute locations
		link();
	}

	/* Constructor for the shader taking in a vertex and fragment shader file location
	   And sets point light position */
	Shader::Shader(std::string _vertFileName, std::string _fragFileName, std::string _newLightPos) :
		m_programId(glCreateProgram()), m_newLightPos(_newLightPos)
	{
		//calling create shader on each fragment and vertex shader giving it the correct type
		m_vertShader = createShader(_vertFileName, GL_VERTEX_SHADER);
		m_fragShader = createShader(_fragFileName, GL_FRAGMENT_SHADER);
		//linking attribute locations
		link();
	}

	/* shader deconstructor */
	Shader::~Shader()
	{
		// Detach and destroy the shader objects. These are no longer needed
		// because we now have a complete shader program.
		glDetachShader(m_vertShader, m_programId);
		glDetachShader(m_fragShader, m_programId);
		glDeleteShader(m_vertShader);
		glDeleteShader(m_fragShader);
	}

	/* using polymorphism render function to take two different lots of arguements, one for taking a model and a texture and one that takes a quad(Vao) and a texture*/
	void Shader::render(std::shared_ptr<Model> _model, std::shared_ptr<Texture> _texture, bool _skyBox)
	{
		// Instruct OpenGL to use our shader program and our VAO
		glUseProgram(m_programId);

		//glBindVertexArray(vaoId);
		glBindVertexArray(_model->getModelId());

		//Bind Texture
		glBindTexture(GL_TEXTURE_2D, _texture->getId());

		if (!_skyBox)
		{
			//BackFace Culling
			glEnable(GL_CULL_FACE);
		}

		//Depth Testing
		glEnable(GL_DEPTH_TEST);

		// Draw 3 vertices (a triangle)
		glDrawArrays(GL_TRIANGLES, 0, _model->getVertices());

		// Reset the state
		glDisable(GL_DEPTH_TEST);
		if (!_skyBox)
		{
			glDisable(GL_CULL_FACE);
		}
		glBindVertexArray(0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Shader::renderQuad(std::shared_ptr<Vao> _Vao, std::shared_ptr<Texture> _texture)
	{
		// Instruct OpenGL to use our shader program and our VAO
		glUseProgram(m_programId);

		//glBindVertexArray(vaoId);
		glBindVertexArray(_Vao->getId());

		//Bind Texture
		glBindTexture(GL_TEXTURE_2D, _texture->getId());

		//Depth Testing
		glEnable(GL_DEPTH_TEST);

		//Alpha Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw 3 vertices (a triangle)
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Reset the state
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Shader::renderQuad(std::shared_ptr<Vao> _Vao)
	{
		// Instruct OpenGL to use our shader program and our VAO
		glUseProgram(m_programId);

		//glBindVertexArray(vaoId);
		glBindVertexArray(_Vao->getId());

		//Depth Testing
		glEnable(GL_DEPTH_TEST);

		//Alpha Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw 3 vertices (a triangle)
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Reset the state
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	void Shader::renderLine(std::shared_ptr<Vao> _Vao, int _numOfLines)
	{
		// Instruct OpenGL to use our shader program and our VAO
		glUseProgram(m_programId);

		glBindVertexArray(_Vao->getId());

		//Depth Testing
		glEnable(GL_DEPTH_TEST);

		//Alpha Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw 2 vertices (a line)
		glDrawArrays(GL_LINES, 0, _numOfLines * 2);
		
		// Reset the state
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	/* link attribute locations and link to program */
	void Shader::link()
	{
		// Ensure the VAO "position"  
		glBindAttribLocation(m_programId, 0, "a_Position");
		glBindAttribLocation(m_programId, 1, "a_TexCoord");
		glBindAttribLocation(m_programId, 2, "a_Normal");

		// Perform the link and check for failure
		GLint success = 1;
		glLinkProgram(m_programId);
		glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
		if (!success)
		{
			throw std::exception();
		}
	}

	/* bind matrix given which matrix and a type */
	void Shader::bindShader(glm::mat4 _matrix, const char* _type)
	{
		//Bind matrix
		glUseProgram(getId());
		glProgramUniformMatrix4fv(m_programId, glGetUniformLocation(m_programId, _type), 1, GL_FALSE, glm::value_ptr(_matrix));
		glUseProgram(0);
	}

	/* using fstream to open and read the given shader files and convert them into a string to be given to open gl */
	std::string Shader::readFile(std::string _fileName)
	{
		std::string line;
		std::string fileString = "";
		std::ifstream file(_fileName);
		if (file.is_open())
		{
			while (std::getline(file, line))
			{
				if (line == "vec3 lightPos = vec3(0, 0, 0);")
				{
					fileString += m_newLightPos + '\n';
				}
				else
				{
					fileString += line + '\n';
				}
			}
			file.close();
		}
		else throw std::runtime_error("Unable To Open Shader File");

		return fileString;
	}

	/* creating a shader given the file location and type */
	GLuint Shader::createShader(std::string _fileName, GLuint _type)
	{
		//open file and return its insides as a string
		std::string fileString = readFile(_fileName);
		const GLchar* ShaderSrc = fileString.c_str();

		// Create a new shader, attach source code, compile it and
		// check for errors.
		GLuint shaderId = glCreateShader(_type);
		glShaderSource(shaderId, 1, &ShaderSrc, NULL);
		glCompileShader(shaderId);

		GLint success = 0;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
		if (!success) //Print error in exact location for easier debugging
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH,
				&maxLength);
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shaderId, maxLength,
				&maxLength, &errorLog[0]);
			std::cout << &errorLog.at(0) << std::endl;
			throw std::exception();
		}

		//Attach Shader
		glAttachShader(m_programId, shaderId);

		return shaderId;
	}

}