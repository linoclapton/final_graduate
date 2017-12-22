#include "GLSLProgram.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
void GLSLProgram::create(){
	program[id] = glCreateProgram();
}
void GLSLProgram::attachShader(GLenum type, std::string path){
	// Create the shaders
	GLuint shader = glCreateShader(type);
	// Read the Vertex Shader code from the file
	std::string shaderCode;
	std::ifstream fstream(path, std::ios::in);
	if (fstream.is_open()){
		std::string Line = "";
		while (getline(fstream, Line))
			shaderCode += "\n" + Line;
		fstream.close();
	}
	else{
		log = "Impossible to open Are you in the right directory ? Don't forget to read the FAQ !\n";
		return;
	}
	GLint Result = GL_FALSE;
	int InfoLogLength;
	// Compile Vertex Shader
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shader, 1, &sourcePointer, NULL);
	glCompileShader(shader);
	// Check Vertex Shader
	glGetShaderiv(shader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(shader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		log= &VertexShaderErrorMessage[0];
	}
	shaders.push_back(shader);
	glAttachShader(program[id], shader);
}
void GLSLProgram::glActiveTex(GLenum texture){
    glActiveTexture(texture);
}
GLint GLSLProgram::mglGetUniformLocation(const char *name){
    return glGetUniformLocation(program[id],name);
}
void GLSLProgram::mglGenFramebuffers(GLuint& index){
    glGenFramebuffers(1,&index);
}
void GLSLProgram::mglBindFramebuffer(GLuint index){
    glBindFramebuffer(GL_FRAMEBUFFER,index);
}
void GLSLProgram::mglFramebufferTexture2D(GLuint tex){
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
}
void GLSLProgram::mglGenRenderBuffers(GLuint& index){
    glGenRenderbuffers(1, &index);
}
void GLSLProgram::mglBindRenderbuffer(GLuint index){
    glBindRenderbuffer(GL_RENDERBUFFER,index);
}
void GLSLProgram::mglRenderbufferStorage(int size){
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,size,size);
}
void GLSLProgram::mglFramebufferRenderbuffer(GLuint index){
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,index);
}
void GLSLProgram::mglVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2){
    glVertexAttrib3f(index,v0,v1,v2);
}
void GLSLProgram::bindVertexArray(){
	glGenVertexArrays(1, &vertex_array);
//	glBindVertexArray(vertex_array);
}
GLuint GLSLProgram::bindBuffer(GLenum buffer_type, GLuint buffer_size, const GLvoid * data, GLenum draw_type ){
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(buffer_type, buffer);
	glBufferData(buffer_type, buffer_size, data, draw_type);
	buffers.push_back(buffer);
	return buffer;
}
void GLSLProgram::bindBufferToVertex(GLint attrib_location, GLint attrib_size, GLenum buffer_type, GLsizei buffer_per_size, const GLvoid * offset){
	glEnableVertexAttribArray(attrib_location);
	glVertexAttribPointer(attrib_location, attrib_size, buffer_type, GL_FALSE, buffer_per_size, offset);
}
void GLSLProgram::bindBufferToVertex(const char* attrib_name, GLint attrib_size, GLenum buffer_type, GLsizei buffer_per_size, const GLvoid * offset){
	GLint attrib_location = glGetAttribLocation(program[id], attrib_name);
	bindBufferToVertex(attrib_location, attrib_size, buffer_type, buffer_per_size, offset);
}
void GLSLProgram::link(){
	glLinkProgram(program[id]);
	GLint status;
	glGetProgramiv(program[id], GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		fprintf(stderr, "Failed to link shader program!\n");
		GLint logLen;
		glGetProgramiv(program[id], GL_INFO_LOG_LENGTH,
			&logLen);
		if (logLen > 0)
		{
			char * log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(program[id], logLen, &written, log);
		}
	}
}
void GLSLProgram::use(int id){
    change(id);
	glUseProgram(program[id]);
}

void GLSLProgram::printActiveAttirbs(){
	GLint maxLength, nAttribs;
	glGetProgramiv(program[id], GL_ACTIVE_ATTRIBUTES, &nAttribs);
	glGetProgramiv(program[id], GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
		&maxLength);
	GLchar * name = (GLchar *)malloc(maxLength);
	GLint written, size, location;
	GLenum type;
	printf(" Index | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nAttribs; i++) {
		glGetActiveAttrib(program[id], i, maxLength, &written,
			&size, &type, name);
		location = glGetAttribLocation(program[id], name);
		printf(" %-5d | %s\n", location, name);
	}
	free(name);
}
void GLSLProgram::printActiveUniforms(){
	GLint maxLength,nUniforms;
	glGetProgramiv(program[id], GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLength);
	glGetProgramiv(program[id], GL_ACTIVE_UNIFORMS,&nUniforms);
	GLchar * name = (GLchar *)malloc(maxLength);
	GLint written, size, location;
	GLenum type;
	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nUniforms; ++i) {
		glGetActiveUniform(program[id], i, maxLength, &written,&size, &type, name);
		location = glGetUniformLocation(program[id], name);
		printf(" %-8d | %s\n", location, name);
	}
	free(name);
}
void GLSLProgram::printLog(){
	std::cout << log << std::endl;
}


void GLSLProgram::setUniform(const char *name, float x, float y, float z){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, x, y, z);
}
void GLSLProgram::setUniform(const char *name, const glm::vec3 & v){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, v);
}
void GLSLProgram::setUniform(const char *name, const glm::vec4 & v){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, v);
}
void GLSLProgram::setUniform(const char *name, const glm::mat4 & m){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, m);
}
void GLSLProgram::setUniform(const char *name, const glm::mat3 & m){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, m);
}
void GLSLProgram::setUniform(const char *name, float val){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, val);
}
void GLSLProgram::setUniform(const char *name, int val){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, val);
}
void GLSLProgram::setUniform(const char *name, bool val){
	GLint uniform_location = glGetUniformLocation(program[id], name);
	setUniform(uniform_location, val);
}
void GLSLProgram::setUniform(const char *name, int size,float* data){
	GLint uniform_location = glGetUniformLocation(program[id], name);
    glUniform3fv(uniform_location,size,data);
}
void GLSLProgram::setUniform(GLint uniform_location, float x, float y, float z){
	glUniform3f(uniform_location, x, y, z);
}
void GLSLProgram::setUniform(GLint uniform_location, const glm::vec3 & v){
	setUniform(uniform_location, v.x, v.y, v.z);
}
void GLSLProgram::setUniform(GLint uniform_location, const glm::vec4 & v){
	glUniform4f(uniform_location, v.x, v.y, v.z, v.w);
}
void GLSLProgram::setUniform(GLint uniform_location, const glm::mat4 & m){
	glUniformMatrix4fv(uniform_location, 1, GL_FALSE, &m[0][0]);
}
void GLSLProgram::setUniform(GLint uniform_location, const glm::mat3 & m){
	glUniformMatrix3fv(uniform_location, 1,GL_FALSE,&m[0][0]);
}
void GLSLProgram::setUniform(GLint uniform_location, float val){
	glUniform1f(uniform_location, val);
}
void GLSLProgram::setUniform(GLint uniform_location, int val){
	glUniform1i(uniform_location, val);
}
void GLSLProgram::setUniform(GLint uniform_location, bool val){
	glUniform1i(uniform_location, val);
}

GLuint GLSLProgram::setUniform(const char *name, GLuint buffer_size, GLvoid *pointer, GLenum draw_type){
	GLint blockIndex = glGetUniformBlockIndex(program[id], name);
	return setUniform(blockIndex, buffer_size, pointer, draw_type);
}

GLuint GLSLProgram::setUniform(GLint uniform_location, GLuint buffer_size, GLvoid *pointer, GLenum draw_type){
	GLuint uniform_buffer;
	glGenBuffers(1, &uniform_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
	glBufferData(GL_UNIFORM_BUFFER, buffer_size, pointer, draw_type);
	glBindBufferBase(GL_UNIFORM_BUFFER, uniform_location, uniform_buffer);
	buffers.push_back(uniform_buffer);
	return uniform_buffer;
}

void GLSLProgram::destroy(){
	for (GLuint buffer : buffers)
		glDeleteBuffers(1, &buffer);
	for (GLuint shader : shaders)
		glDeleteShader(shader);
	glDeleteVertexArrays(1, &vertex_array);
	glDeleteProgram(program[id]);
}
void GLSLProgram::getUniform(const char *name,float* variable)
{
	GLint uniform_location = glGetUniformLocation(program[id],name);
	getUniform(uniform_location,variable);
}

void GLSLProgram::getUniform(GLint uniform_location,float* variable){
	glGetUniformfv(program[id],uniform_location,variable);
}
void GLSLProgram::initTextShader(){
    //Shader shader("shaders/text.vs", "shaders/text.frag");
    int currentID = id;
    change(9);
    create();
    attachShader(GL_VERTEX_SHADER,"textVS.glsl");
    attachShader(GL_FRAGMENT_SHADER,"textFS.glsl");
    link();
    use(9);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(800), 0.0f, static_cast<GLfloat>(600));
    glUniformMatrix4fv(glGetUniformLocation(program[9], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
            );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    use(currentID);
}
void GLSLProgram::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color){
    int currentID = id;
    use(9);
    glUniform3f(glGetUniformLocation(program[id], "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    use(currentID);
}

void GLSLProgram::deleteShaders(){
	for(GLuint shader : shaders)
		glDeleteShader(shader);
	shaders.clear();
}

void GLSLProgram::deleteProgram(){
	glDeleteProgram(program[id]);
	program[id] = 0;
}

void GLSLProgram::change(int id){
    this->id = id;
}

void GLSLProgram::getConstant(GLenum type,GLuint index,GLint* data){
    glGetIntegeri_v(type,index,data);
}
void GLSLProgram::getConstant(GLenum type,GLint* data){
    glGetIntegerv(type,data);
}