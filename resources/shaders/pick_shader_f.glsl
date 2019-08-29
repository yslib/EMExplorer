#version 330
uniform vec4 pickColor;
out vec4 fragColor;
void main(){
    fragColor = pickColor;
}