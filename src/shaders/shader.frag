#version 450

layout(location = 0) out vec4 outColor; //layout(location = 0) буффер кадров с индексом 0
layout(location = 0) in vec3 fragColor; //layout(location = 0) формирует цепочку котора€ проходит через шейдеры
//≈сли layout(location = 0) out то эта паременна€ будет соответсвовать layout(location = 0) in в следующем шейдере

void main() {
  outColor = vec4(fragColor, 1.0);
}