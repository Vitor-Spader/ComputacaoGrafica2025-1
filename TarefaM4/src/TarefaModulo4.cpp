/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Processamento Gráfico/Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 07/03/2025
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Custom Classes
#include "Engine/Control.hpp"
#include "Engine/Character.hpp"

//Global Types
struct Mesh 
{
    GLuint VAO; 

};

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
GLuint loadSimpleOBJ(string filePATH, int &nVertices);
GLuint loadTexture(string filePath, int &width, int &height);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = R"(
	#version 450 core
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 color;
	layout (location = 2) in vec2 tex_coord;
	layout (location = 3) in vec3 normal;

	out vec3 vertexColor;
	out vec2 texCoord;
	out vec3 fragPos;
	out vec3 scaledNormal;

	uniform mat4 model;
	//uniform mat4 view;
	//uniform mat4 projection;

	void main()
	{
		gl_Position = model * vec4(position, 1.0f);
		vertexColor = color;
		texCoord = tex_coord;
		fragPos = vec3(model * vec4(position, 1.0));
		scaledNormal = vec3(model * vec4(normal, 1.0));
	})";


//Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = R"(
	#version 450 core
	in vec3 vertexColor;
	in vec2 texCoord;
	in vec3 fragPos;
	in vec3 scaledNormal;

	out vec4 Color;

	//Propriedades superfície
	uniform float ka;
	uniform float kd;
	uniform float ks;
	uniform float q;

	//Propriedades fonte de luz
	uniform vec3 lightPos;
	uniform vec3 lightColor;

	//Posição da Câmera
	uniform vec3 cameraPos;

	//pixels da textura
	uniform sampler2D tex_buffer;

	void main()
	{
		vec3 finalColor = vertexColor;
		//Cor da textura
		//vec3 finalColor = texture(tex_buffer, texCoord).rgb;

		//Cálculo da parcela de iluminação ambiente
		vec3 ambient = ka * lightColor;

		//Cálculo da parcela de iluminação difusa
		vec3 N = normalize(scaledNormal);
		vec3 L = normalize(lightPos - fragPos);
		float diff = max(dot(N,L), 0.0f);
		vec3 diffuse = kd * diff * lightColor;

		//Cálculo da Parcela de Iluminação Especular
		vec3 V = normalize(cameraPos - fragPos);
		vec3 R = normalize(reflect(-L, N));
		float spec = max(dot(R,V), 0.0f);
		spec = pow(spec, q);
		vec3 specular = ks * spec * lightColor;

		vec3 result = (ambient + diffuse) * finalColor +specular;
		
		//Color = texture(tex_buffer,texCoord);
		Color = vec4(result, 1.0f);
	})";

vector<Character*> character(1);
int currentObject = 0;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa M4 -- Vitor Henrique Spader!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();
	
	//Instancia os objetos dos personagens
	for (int x=0; x < character.size(); x++)
	{
		Control* controler = new Control(0.0f, 0.0f, 0.0f , 0.1f);
		controler->setScale(-3);
		character[x] = new Character(controler, "../assets/Modelos3D/Suzanne.obj", true);
		character[x]->initializeUniformProperties(shaderID);
	}

	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int x=0; x < character.size(); x++)
		{
			character[x]->draw(shaderID, true);

		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	for (Character* object: character)
	{
		delete object;
	}

	glfwTerminate();

	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		
		case GLFW_KEY_0:
			currentObject = 0;
			break;
		
		case GLFW_KEY_1:
			if (character.size() > 1)
				currentObject = 1;
			break;
		
		case GLFW_KEY_X:
			character[currentObject]->control->setRotateX(1);
			break;

		case GLFW_KEY_Y:
			character[currentObject]->control->setRotateY(1);
			break;

		case GLFW_KEY_Z:
			character[currentObject]->control->setRotateZ(1);
			break;

		case GLFW_KEY_RIGHT_BRACKET:
			character[currentObject]->control->setScale(1);
			break;

		case GLFW_KEY_LEFT_BRACKET:
			character[currentObject]->control->setScale(-1);
			break;
		
		case GLFW_KEY_A:
			character[currentObject]->control->setTranslateX(-1);
			break;

		case GLFW_KEY_D:
			character[currentObject]->control->setTranslateX(1);
			break;

		case GLFW_KEY_W:
			character[currentObject]->control->setTranslateZ(1);
			break;

		case GLFW_KEY_S:
			character[currentObject]->control->setTranslateZ(-1);
			break;

		case GLFW_KEY_I:
			character[currentObject]->control->setTranslateY(1);
			break;

		case GLFW_KEY_J:
			character[currentObject]->control->setTranslateY(-1);
			break;

		default:
			break;
		}
	}
}

//Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
// shader simples e único neste exemplo de código
// O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
// fragmentShader source no iniçio deste arquivo
// A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}