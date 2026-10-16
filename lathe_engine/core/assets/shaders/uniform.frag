#version 330 core
out vec4 FragColor;

uniform vec4 ourColor; //set in src code

void main(){
	FragColor = ourColor;
}