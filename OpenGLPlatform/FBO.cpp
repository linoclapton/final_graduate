#include "FBO.h"
#include <iostream>

void FBOModel::initial(){
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex_model.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment_model.glsl");
    glsl.link();
    glsl.use(0);
    model = glm::mat4(1.0f);
    model *= glm::rotate(glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model *= glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(70.0f), (float)4 / 3, 0.3f, 100.0f);
    eyeZPosition = 2.0f;
    glsl.setUniform("Kd", 0.9f, 0.5f, 0.3f);
    glsl.setUniform("Ld", 1.0f, 1.0f, 1.0f);
    glsl.setUniform("Ks", 0.9f, 0.5f, 0.3f);
    glsl.setUniform("Ls", 1.0f, 1.0f, 1.0f);
    glsl.setUniform("EyePosition", 0.0f,0.0f,eyeZPosition);
    glsl.setUniform("Ka", 0.1f, 0.05f, 0.03f);
    glsl.setUniform("La", 1.0f, 1.0f, 1.0f);
    glsl.setUniform("LightPosition", view *glm::vec4(-1.0f, -1.0f, 5.0f, 1.0f));
    glsl.setUniform("View",view);
    mv = view * model;
    glsl.setUniform("Model", model);
    glsl.setUniform("NormalMatrix",glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
    glsl.setUniform("Projection", projection);
    glsl.setUniform("hasLight",true);
    float a = 0.5;
    GLfloat vertice[] = {-a,a,a,   a,a,a,  a,-a,a,  -a,-a,a, 
                          -a,a,-a, a,a,-a, a,-a,-a, -a,-a,-a, 
                        };
    GLfloat normal[] = {-1,0,0, 0,0,1, 1,0,0, 0,0,-1, 0,1,0, 0,-1,0, };
    //GLuint  vertice_indices[] = { 1,5,4, 4,5,8, 1,4,3, 1,3,2, 2,3,7, 2,7,6, 6,7,8, 6,8,5, 1,2,5, 5,2,6, 4,8,7, 4,7,3, };
    GLuint  vertice_indices[] = { 0,4,3, 3,4,7, 0,3,2, 0,2,1, 1,2,6, 1,6,5, 5,6,7, 5,7,4, 0,1,4, 4,1,5, 3,7,6, 3,6,2,};
    //GLuint  normal_indices[] = {0,0, 1,1, 2,2, 3,3, 4,4, 5,5,};
    GLuint  normal_indices[] = {0,0,0,0,0,0, 1,1,1,1,1,1, 2,2,2,2,2,2, 3,3,3,3,3,3, 4,4,4,4,4,4, 5,5,5,5,5,5};
    glsl.genBuffers(3,VBO);
    glsl.genBuffers(2,EBO);
    glsl.genVertexArrays(2,VAO);
    glBindVertexArray(VAO[0]);
    // 2. Copy our vertices array in a vertex buffer for
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);
    // 3. Copy our index array in a element buffer for OpenGL
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertice_indices), vertice_indices, GL_STATIC_DRAW);
    // 3. Then set the vertex attributes pointers
    
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    //glsl.bufferData(GL_ARRAY_BUFFER, sizeof(normal),normal, GL_STATIC_DRAW);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertice),vertice, GL_STATIC_DRAW);
    //glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    //glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(normal_indices), normal_indices, GL_STATIC_DRAW);
    glsl.vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glsl.enableVertexAttribArray(1);
    // 4. Unbind VAO (NOT the EBO)
    glBindVertexArray(0);

    glsl.change(1);
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex_fbo.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment_fbo.glsl");
    glsl.link();
    glsl.printLog();
    glsl.use(1);
    
    GLfloat board[] = {-1.0,1.0,0.0,  1.0,1.0,0.0, 1.0,-1.0,0.0, -1.0,-1.0,0.0};
    GLuint board_indice[] = {0,2,1,0,3,2};
    glsl.setUniform("LightPosition",glm::vec4(1.0f,0.0f,0.0f,1.0f));
    glBindVertexArray(VAO[1]);
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(board),board, GL_STATIC_DRAW);
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(board_indice),board_indice, GL_STATIC_DRAW);
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    glsl.setUniform("tex",0);
    glBindVertexArray(0);

    glsl.genFramebuffers(1,&FBO);
    glsl.bindFramebuffer(FBO,1024,1024,4);
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glActiveTexture(GL_TEXTURE0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_FLOAT, NULL); 
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);
    GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,  attachments);
    //glBindTexture(GL_TEXTURE_2D, 0);

    /*GLuint rbAux;
    glGenRenderbuffers(1, &rbAux);
    glBindRenderbuffer(GL_RENDERBUFFER, rbAux);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbAux);*/

    GLuint fb;
    glGenRenderbuffers(1, &fb);
    glBindRenderbuffer(GL_RENDERBUFFER, fb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb);

}

void FBOModel::render(){
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
	glClearColor(0.2f,0.3f,0.3f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0,0,1024,1024);
    glsl.use(0);
    glBindVertexArray(VAO[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glPopAttrib();
    glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glsl.use(1);
    glBindVertexArray(VAO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void FBOModel::rotate(glm::mat4 rMat){
    model = rMat*model;
    glsl.use(0);
    glsl.setUniform("Model", model);
}