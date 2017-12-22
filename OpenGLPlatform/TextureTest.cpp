#include "TextureTest.h"
#include <soil.h>
#include <iostream>

void TextureTest::initial(){
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex_texture.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment_texture.glsl");
    glsl.link();
    glsl.use(0);
    int count;
    glsl.getConstant(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&count);
    std::cout<<count<<std::endl;
    GLuint indices[] = { // Note that we start from 0!
        0, 1, 3, // First Triangle
        1, 2, 3 // Second Triangle
    };
    /*GLfloat vertices[] = {
        // Positions // Colors // Texture Coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // Top Right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Bottom Right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom Left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // Top Left
    };*/
    GLfloat vertices[] = {
        -0.2f, -0.2f, -0.2f,  0.0f, 0.0f,
        0.2f, -0.2f, -0.2f,  1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 0.0f,

        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 1.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,

        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  1.0f, 0.0f,

        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,

        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,
        0.2f, -0.2f, -0.2f,  1.0f, 1.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        0.2f, -0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,  0.0f, 1.0f,

        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f,
        0.2f,  0.2f, -0.2f,  1.0f, 1.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,  1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,  0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,  0.0f, 1.0f
    };
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    //glm::mat4 model;
    //model = glm::translate(model , glm::vec3(0.707, -0.707, 0.0));
    //model = glm::rotate(model , 0.8f, glm::vec3(0.0, 0.0, 1.0));
    //model = glm::translate(model , glm::vec3(0.707, 0.707, 0.0));
    //model = glm::rotate(model , 0.8f, glm::vec3(0.0, 0.0, 1.0));
    //glsl.setUniform("Model",model);
    glsl.setUniform("ratio",1.0f);
    glsl.genBuffers(2,VBO);
    glsl.genBuffers(2,EBO);
    glsl.genVertexArrays(2,VAO);
    glBindVertexArray(VAO[0]);
    // 2. Copy our vertices array in a vertex buffer for
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Copy our index array in a element buffer for OpenGL
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 3. Then set the vertex attributes pointers
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    //glsl.vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    //glsl.enableVertexAttribArray(1);
    glsl.vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glsl.enableVertexAttribArray(2);
    // 4. Unbind VAO (NOT the EBO)
    glBindVertexArray(0);
    int w,h;
    unsigned char* img = SOIL_load_image("container.jpg",&w,&h,0,SOIL_LOAD_RGB);
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glsl.generateMipmap(GL_TEXTURE_2D);
    //glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(img);
    //myImg = new unsigned char[512*512*4];
    //glGetTextureSubImage(texture,0,//level 0,0,0,512,512,1,GL_RGBA,GL_UNSIGNED_BYTE,w*h*4,myImg);
    //glBindTexture(GL_TEXTURE_2D,0);
    glsl.glActiveTex (GL_TEXTURE1);
    glGenTextures(1,&texture1);
    glBindTexture(GL_TEXTURE_2D,texture1);
    w = 0;h = 0;
    img = SOIL_load_image("awesomeface.png",&w,&h,0,SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,img);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glsl.generateMipmap(GL_TEXTURE_2D);
    glsl.setUniform("ourTexture2",1);
    //glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(img);
    glsl.change(1);
    glsl.create();
    glsl.attachShader(GL_VERTEX_SHADER,"vertex_texture.glsl");
    glsl.attachShader(GL_FRAGMENT_SHADER,"fragment_texture.glsl");
    glsl.link();
    glsl.use(1);
    glsl.setUniform("ratio",1.0f);
    glBindVertexArray(VAO[1]);
    // 2. Copy our vertices array in a vertex buffer for
    glsl.bindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glsl.bufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. Copy our index array in a element buffer for OpenGL
    glsl.bindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glsl.bufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 3. Then set the vertex attributes pointers
    glsl.vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (GLvoid*)0);
    glsl.enableVertexAttribArray(0);
    //glsl.vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    //glsl.enableVertexAttribArray(1);
    glsl.vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glsl.enableVertexAttribArray(2);
    // 4. Unbind VAO (NOT the EBO)
    glBindVertexArray(0);
    glsl.setUniform("ourTexture2",0);
    glsl.setUniform("ourTexture1",1);
}

void TextureTest::render(){
    static bool test = false;
    glsl.use(2);
    glPointSize(5);
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_POINTS);
    glVertex3f(0.0f,0.0f,0.0f);
    glEnd();
    glsl.use(0);
    //glsl.glActiveTex(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,texture);
    glm::mat4 model;
    //model = glm::translate(model , glm::vec3(0.707, -0.707, 0.0));
    model = glm::mat4(1.0f);
    //model *= glm::rotate(glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //model *= glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    if(test){
        glsl.setUniform("ourTexture2",0);
        glsl.setUniform("ourTexture1",1);
    }else{
        glsl.setUniform("ourTexture2",1);
        glsl.setUniform("ourTexture1",0);
    }
    test =!test;
    glm::mat4 view;
    view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Camera camera;
    float pos[3]={0.0f,0.0f,2.0f};
    float target[3]={0.0f,0.0f,0.0f};
    float up[3]={0.0f,1.0f,0.0f};
    float* viewf = camera.lookAt(pos,target,up);
    model = glm::rotate(model , glm::radians(-55.0f), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 projection;
    //projection = glm::perspective(glm::radians(45.0f), (float)width() / height(), 0.1f, 100.0f);
    //model = glm::rotate(model , 0.8f, glm::vec3(0.0, 0.0, 1.0));
    glsl.setUniform("Model",model);
    // Note that we¡¯re translating the scene in the reverse direction of where we want to move
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glsl.setUniform("View",viewf);
    int w = 811,h = 621;
    projection = glm::perspective<float>(glm::radians(45.0f), (w+0.0)/h, 0.1f, 100.0f);
    glsl.setUniform("Projection",projection);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glsl.use(1);
    model *= glm::translate(glm::vec3(0.2,0.2,0.2));
    glsl.setUniform("Model",model);
    glsl.setUniform("View",view);
    glsl.setUniform("Projection",projection);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}