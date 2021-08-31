#pragma once
#include <glad/glad.h>
#include <glm.hpp>

namespace GLSLShader
{
	enum GLSLShaderType
	{
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};
}

/* */
class GLSLProgram
{
public:
	/* Parameters to set a subroutine.
	 * @param std::string Name name of the subroutine.
	 * @param GLenum Shader.
	 */
	struct SubroutineParameters
	{
		std::string Name;
		GLenum Shader;

		SubroutineParameters(const std::string& name = "Default", GLenum shaderType = GL_FRAGMENT_SHADER);
		bool operator==(SubroutineParameters& parameters);
	};

public:
	GLSLProgram();
	~GLSLProgram();

	/* Make sure GLSL programs can not be copied. */
	GLSLProgram(const GLSLProgram&) = delete;
	/* Make sure GLSLProgram can not be copied. */
	GLSLProgram& operator= (const GLSLProgram&) = delete;

	/* Tries to get the shader type from the filename extension.
	 * Calls CompileShader(filename, shaderType) if the extension is found.
	 * @param std::string& filename.
	 * @exception GLSLProgramException Unrecognized extension.
	 */
	void CompileShader(const std::string& filename);

	/* Tries to open, create a program, and read the shader source code.
	 * Calls CompileShader(sourceCode, shaderType, filename) if successfully opened the file, created the GLSLProgram and read the shader source code.
	 * @param std::string& filename
	 * @param GLSLShaderType type
	 * @exception GLSLProgramException Shader: <filename> not found.
	 * @exception GLSLProgramException Unable to create program.
	 * @exception GLSLProgramException Unable to open: <filename>.
	 */
	void CompileShader(const std::string& filename, GLSLShader::GLSLShaderType type);

	/* Creates a program if it does not exist.
	 * Creates the shader from the source code and compiles it.
	 * Attaches the shader to the program after successful compilation.
	 * @param const char* source The shader source code.
	 * @param GLSLShaderType type
	 * @param std::string& filename <optional> Filename gets used if an exception needs to be thrown.
	 * @exception GLSLProgramException <Filename>: shader compilation failed. <Log message>
	 */
	void CompileShader(const char* source, GLSLShader::GLSLShaderType type, const std::string& filename = std::string());

	/* Tries to link the shaders in the program.
	 * CallsFindUniformLocations and DetachAndDeleteShaderObjects after successfully linking the shaders.
	 * @exception GLSLProgramException Program has not been compiled.
	 * @exception GLSLProgramException Program link failed: <Log message>.
	 */
	void Link();

	/* Uses the program */
	void Use() const;

	/* Checks if the program is linked and validates it.
	 * @exception GLSLProgramException Program is not linked.
	 * @exception GLSLProgramException Program failed to validate: <Log message>
	 */
	void Validate() const;

	/* returns the handle for the program.
	 * @return GLuint programHandle
	 */
	const GLuint GetHandle() const;

	/* Returns if the program is linked.
	 * @return bool linked
	 */
	const bool IsLinked() const;

	/* Sets the specified uniform value.
	 * @param std::string& name Name of the uniform.
	 * @param bool value
	 */
	void SetUniform(const std::string& name, bool value);

	/* Sets the specified uniform value.
	 * @param std::string& name Name of the uniform.
	 * @param int value
	 */
	void SetUniform(const std::string& name, int value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param unsigned int value
	*/
	void SetUniform(const std::string& name, unsigned int value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param float value
	*/
	void SetUniform(const std::string& name, float value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param glm::vec2 value
	*/
	void SetUniform(const std::string& name, glm::vec2 value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param glm::vec3 value
	*/
	void SetUniform(const std::string& name, glm::vec3 value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param glm::vec4 value
	*/
	void SetUniform(const std::string& name, glm::vec4 value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param glm::mat3 value
	*/
	void SetUniform(const std::string& name, glm::mat3 value);

	/* Sets the specified uniform value.
	* @param std::string& name Name of the uniform.
	* @param glm::mat4 value
	*/
	void SetUniform(const std::string& name, glm::mat4 value);

	/* Sets the subroutine in the program mentioned in the parameters.
	 * @param SubroutineParameters& parameters See SubroutineParameters
	 */
	void SetSubroutine(const SubroutineParameters& parameters);

	/* Prints all the active uniforms in the program. */
	void PrintActiveUniforms();

	/* Prints all active uniform blocks in the program. */
	void PrintActiveUniformBlocks();

	/* Prints all active attributes. */
	void PrintActiveAttributes();

private:
	std::unordered_map<std::string, GLint> uniformLocationCache;
	std::unordered_map<std::string, GLuint> subroutineIndexCache;
	GLuint programHandle = 0;
	bool linked = false;

	/* Detaches and deletes all the attached shaders from the program */
	void DetachAndDeleteShaderObjects();

	/* Checks if the file exists */
	bool FileExists(const std::string& filename);

	/* Gets the extension from the filename, if it any exists.
	 * @return std::string extension
	 */
	std::string GetExtension(const std::string& filename);

	/* returns the uniform location from the cache if it exists.
	 * Else adds it to the cache and returns it.
	 * @param std::string& name Name of the uniform.
	 * @return const GLint& uniformLocation
	 */
	const GLint& GetUniformLocation(const std::string& name);

	/* Gets the subroutine index from the cache if it exists.
	 * Else adds it to the cache and returns it.
	 * @param const SubroutineParameters* parameters
	 * @return const GLuint& subroutineIndex
	 */
	const GLuint& GetSubroutineIndexFromCache(const SubroutineParameters& parameters);

	/* Finds all the uniform locations and adds them to the cache */
	void FindUniformLocations();

	/* Turns a GLenum into a string
	 * @param GLenum type
	 * @return std::string enumType
	 */
	std::string GetTypeString(GLenum type);
};
