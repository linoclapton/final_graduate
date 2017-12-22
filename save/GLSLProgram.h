#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <string>
#include <vector>
#include <map>
#include <ft2build.h>
#include <freetype\freetype.h>
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class GLSLProgram{

private:
    int id;
	GLuint program[10];
    GLuint textProgram;
	GLuint vertex_array;
    GLuint VAO,VBO;
	std::vector<GLuint> shaders;
	std::vector<GLuint> buffers;
    std::map<GLchar, Character> Characters;
public:
	std::string log;
	GLSLProgram(){
        id = 0;
		log = "";
	}
	void attachShader(GLenum type, std::string path);
	void create();
	void link();
    void initTextShader();
	void bindVertexArray();
	GLuint bindBuffer(GLenum buffer_type, GLuint buffer_size, const GLvoid * data, GLenum draw_type);
	void bindBufferToVertex(GLint attrib_location, GLint attrib_size, GLenum buffer_type, GLsizei buffer_per_size,const GLvoid * offset);
	void bindBufferToVertex(const char * attrib_name, GLint attrib_size, GLenum buffer_type, GLsizei buffer_per_size, const GLvoid * offset);
    void change(int id);
	void use(int id);
	void destroy();

	void setUniform(const char *name, float x, float y,float z);
	void setUniform(const char *name, const glm::vec3 & v);
	void setUniform(const char *name, const glm::vec4 & v);
	void setUniform(const char *name, const glm::mat4 & m);
	void setUniform(const char *name, const glm::mat3 & m);
	void setUniform(const char *name, float val);
	void setUniform(const char *name, int val);
	void setUniform(const char *name, bool val);
    void setUniform(const char *name, int size,float* data);

	GLuint setUniform(const char *name, GLuint buffer_size, GLvoid *pointer, GLenum draw_type);

	void setUniform(GLint uniform_location, float x, float y, float z);
	void setUniform(GLint uniform_location, const glm::vec3 & v);
	void setUniform(GLint uniform_location, const glm::vec4 & v);
	void setUniform(GLint uniform_location, const glm::mat4 & m);
	void setUniform(GLint uniform_location, const glm::mat3 & m);
	void setUniform(GLint uniform_location, float val);
	void setUniform(GLint uniform_location, int val);
	void setUniform(GLint uniform_location, bool val);
	GLuint setUniform(GLint uniform_location, GLuint buffer_size, GLvoid *pointer, GLenum draw_type);

	void getUniform(GLint uniform_location,float* variable);
	void getUniform(const char *name,float* variable);
    void glActiveTex(GLenum texture);
    GLint mglGetUniformLocation(const char *name);
    void mglGenFramebuffers(GLuint& index);
    void mglBindFramebuffer(GLuint index);
    void mglFramebufferTexture2D(GLuint tex);
    void mglGenRenderBuffers(GLuint& index);
    void mglBindRenderbuffer(GLuint index);
    void mglRenderbufferStorage(int size);
    void mglFramebufferRenderbuffer(GLuint index);
    void mglVertexAttrib3f(	GLuint index, GLfloat v0, GLfloat v1, GLfloat v2);
    void getConstant(GLenum type,GLuint index,GLint* data);
    void getConstant(GLenum type,GLint* data);

    void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	void deleteShaders();
	void deleteProgram();
	void printActiveUniforms();
	void printActiveAttirbs();
	void printLog();
};
