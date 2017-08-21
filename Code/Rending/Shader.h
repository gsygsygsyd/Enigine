#pragma once

#include <GLEW/glew.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <GLM/glm.hpp>
#include "Util/Gloabl.h"

bool GGetShaderType(const std::string& file, GLuint& shadertype)
{

}

enum 
{
	LOAD_SHADER_FAIL,
	CREATE_SHADER_FAIL,
	COMPILE_SHADER_FAIL,
	COMPILE_PROGRAM_FAIL,
	LINK_PRAGRAM_FAIL,
	NO_ERROR
};

struct FShader
{
	FShader(const std::string& file)
	{
		Error = LOAD_SHADER_FAIL;

		Error = LoadProgramFile(file);
	}

	FShader(const char* source, int type)
	{
		Error = LOAD_SHADER_FAIL;

		Error = LoadFromStr(source, type);
	}

	
	int Error;

	int LoadFromStr(const char* source, int shadertype)
	{
		//创建shader
		ShaderType = shadertype;
		ShaderID = glCreateShader(ShaderType);
		if (0 == ShaderID)
		{
			return CREATE_SHADER_FAIL;
		}

		//写入shader代码
		
		glShaderSource(ShaderID, 1, &source, NULL);

		//编译shader
		glCompileShader(ShaderID);

		//查看错误
		GLint compile_status = 0;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &compile_status); // 检查编译状态
		if (GL_FALSE == compile_status) // 获取错误报告
		{
			GLint log_length = 0;
			glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetShaderInfoLog(ShaderID, log_length, &log_length, log);

			CompileResult.clear();
			CompileResult.assign(log);

			delete[]log;

			return COMPILE_SHADER_FAIL;
		}

		return NO_ERROR;
	}

	int LoadProgramFile(const std::string& file)
	{
		//加载文件
		std::string source;
		std::ifstream in_stream(file);
		if (!in_stream)
		{
			 return LOAD_SHADER_FAIL;
		}
		source.assign(std::istreambuf_iterator<char>(in_stream), std::istreambuf_iterator<char>());
		if (source.empty())
		{
			return LOAD_SHADER_FAIL;
		}

		if (!GGetShaderType(file, ShaderType))
			return LOAD_SHADER_FAIL;

		//创建shader
		ShaderID = glCreateShader(ShaderType);
		if (0 == ShaderID)
		{
			return CREATE_SHADER_FAIL;
		}

		//写入shader代码
		const char* source_data = source.data();
		glShaderSource(ShaderID, 1, &source_data, NULL);

		//编译shader
		glCompileShader(ShaderID);

		//查看错误
		GLint compile_status = 0;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &compile_status); // 检查编译状态
		if (GL_FALSE == compile_status) // 获取错误报告
		{
			GLint log_length = 0;
			glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetShaderInfoLog(ShaderID, log_length, &log_length, log);

			CompileResult.clear();
			CompileResult.assign(log);

			delete[]log;

			return COMPILE_SHADER_FAIL;
		}

		return NO_ERROR;
	}

	//着色器类型
	enum EShaderType
	{
		VERTEX = GL_VERTEX_SHADER,
		GEOMETRY = GL_GEOMETRY_SHADER,
		FRAGEMENT = GL_FRAGMENT_SHADER,
		TESS_CONTROL = GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
		COMPUTE = GL_COMPUTE_SHADER
	};

	GLuint ShaderID;
	GLuint ShaderType;
	std::string CompileResult;
	

	~FShader()
	{
		glDeleteShader(ShaderID);
	}
};

MAKE_SMART_PTR(FShader)

class FShaderProgram
{
public:
	FShaderProgram()
	{
		ProgramID = -1;
		Linked = false;
		ProgramID = glCreateProgram();
	}

	int AddShader(FShaderPtr shader)
	{
		if (shader->Error != NO_ERROR)
		{
			return LOAD_SHADER_FAIL;
		}
		Shaders.push_back(shader);
		return NO_ERROR;
	}

	int AddShader(const std::string& shaderfile)
	{
		FShaderPtr shader = std::make_shared<FShader>(shaderfile);
		return AddShader(shader);
	}

	int AddShaderSource(const std::string& source, int shadertype)
	{
		FShaderPtr shader = std::make_shared<FShader>(source, shadertype);
		return AddShader(shader);
	}

	int Link()
	{
		for (auto it : Shaders)
		{
			glAttachShader(ProgramID, it->ShaderID);
		}
		glLinkProgram(ProgramID);

		GLint linkStatus;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &linkStatus);
		if (GL_FALSE == GL_FALSE)
		{
			GLint log_length = 0;
			glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &log_length);
			char* log = new char[log_length];
			glGetProgramInfoLog(ProgramID, log_length, &log_length, log);

			CompileStatus.clear();
			CompileStatus.assign(log);

			delete[]log;

			return COMPILE_PROGRAM_FAIL;
		}

		return NO_ERROR;
	}

	int Use()
	{
		if (Linked)
		{
			glUseProgram(ProgramID);
			return NO_ERROR;
		}

		return LINK_PRAGRAM_FAIL;
	}

	int BindAttribLoaction(GLuint location, const char* name)
	{
		glBindAttribLocation(ProgramID, location, name);
	}
	void BindFragDataLocation(GLuint location, const char* name)
	{
		glBindFragDataLocation(ProgramID, location, name);
	}

	void SetUniform(const char* name, float x, float y, float z)
	{
		GLint loc = GetUniformLocation(name);
		glUniform3f(loc, x, y, z);
	}

	void SetUniform(const char* name, const glm::vec3& v)
	{
		SetUniform(name, v.x, v.y, v.z);
	}

	void SetUniform(const char* name, const glm::vec4& v)
	{
		GLint loc = GetUniformLocation(name);
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	}

	void SetUniform(const char* name, const glm::mat3& m)
	{
		GLint loc = GetUniformLocation(name);
		glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
	}

	void SetUniform(const char* name, const glm::mat4& m)
	{
		GLint loc = GetUniformLocation(name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
	}

	void SetUniform(const char* name, float val)
	{
		GLint loc = GetUniformLocation(name);
		glUniform1f(loc, val);
	}
	
	void SetUniform(const char* name, int val)
	{
		GLint loc = GetUniformLocation(name);
		glUniform1i(loc, val);
	}

	void SetUniform(const char* name, bool val)
	{
		int loc = GetUniformLocation(name);
		glUniform1i(loc, val);
	}

	int GetUniformLocation(const char* name)
	{
		auto itor = UniformLocations.find(name);
		if (itor == UniformLocations.end())
		{
			UniformLocations[name] = glGetUniformLocation(ProgramID, name);
		}

		return UniformLocations[name];
	}

	
	int Error;
	GLuint ProgramID;
	bool Linked;
	std::string CompileStatus;

	~FShaderProgram()
	{
		for (auto it : Shaders)
		{
			glDetachShader(ProgramID, it->ShaderID);
		}
		Shaders.clear();
		glDeleteProgram(ProgramID);
	}
private:
	std::vector<FShaderPtr> Shaders;
	std::unordered_map<std::string, int> UniformLocations;

};

MAKE_SMART_PTR(FShaderProgram)