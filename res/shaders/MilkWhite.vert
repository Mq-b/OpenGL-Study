#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 mvp;
uniform mat4 model;
out vec3 vNormal;
out vec3 vPos;
void main(){
    vPos = aPos;
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = mvp * vec4(aPos, 1.0);
}