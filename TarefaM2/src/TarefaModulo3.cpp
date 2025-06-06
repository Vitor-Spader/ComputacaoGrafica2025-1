#include <iostream>
#include <string>
#include <assert.h>
#include <cstdio>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Control.hpp"
#include "Engine/LoadObjLight.hpp"

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;


// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;

out vec3 vNormal;
out vec4 fragPos; 
out vec4 vColor;
void main()
{
   	gl_Position = model * vec4(position.x, position.y, position.z, 1.0);
	fragPos = model * vec4(position.x, position.y, position.z, 1.0);
	vNormal = normal;
	vColor = vec4(1.0,0.0,0.0,1.0);
})";

//Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = R"(
#version 400
uniform sampler2D texBuff;
uniform vec3 light[3];
uniform vec3 camPos;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float q;
out vec4 color;
in vec4 fragPos;
in vec3 vNormal;
in vec4 vColor;
void main()
{

	vec3 lightColor = vec3(1.0,1.0,1.0);
	vec4 objectColor = vColor;

	//Coeficiente de luz ambiente
	vec3 ambient = ka * lightColor;

	//Coeficiente de reflexão difusa
	vec3 N = normalize(vNormal);

	vec3 specular = vec3(0.0,0.0,0.0);
	vec3 L;
	float diff, spec;
	vec3 diffuse = vec3(0.0,0.0,0.0);
	vec3 R, V;

	for (int x=0; x < 3; x++){
		L = normalize(light[x] - vec3(fragPos));
		diff = max(dot(N, L),0.0);
		diffuse += kd * diff * lightColor;

		//Coeficiente de reflexão especular
		R = normalize(reflect(-L,N));
		V = normalize(camPos - vec3(fragPos));
		spec = max(dot(R,V),0.0);
		spec = pow(spec,q);
		specular += (ks * spec * lightColor); 
	}


	vec3 result = (ambient + diffuse) * vec3(objectColor) + specular;
	color = vec4(result,1.0);

})";

GLfloat  rotateX=0.0, rotateY=0.0, rotateZ=0.0, dir_a=0.0, dir_d=0.0 ,dir_w=0.0, dir_s=0.0;
GLfloat dir_i=0.0, dir_k=0.0, escala=1.0f;

int imodelo=0;
glm::vec3 light[3] = {
		glm::vec3(0.9, 0.8, 0.9), // Key 0.7, 0.0, 0.9
		glm::vec3(-0.9, -0.8, 0.0), // Fill
		glm::vec3(1.0, -0.5, -5.0) // Back
	};
// Função MAIN
int main()
{
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D -- Vitor Maia!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();
	
	glUseProgram(shaderID);

	//inicializando os objetos com buffer, indices, e matriz
	loadSimpleOBJ su = loadSimpleOBJ();

	float ka = 0.2, kd =0.5, ks = 0.5, q = 20.0;

	glm::vec3 camPos = glm::vec3(0.0,0.0,-3.0);

	GLint modelLoc = glGetUniformLocation(shaderID, "model");

	glUniform1f(glGetUniformLocation(shaderID, "ka"), ka);
	glUniform1f(glGetUniformLocation(shaderID, "kd"), kd);
	glUniform1f(glGetUniformLocation(shaderID, "ks"), ks);
	glUniform1f(glGetUniformLocation(shaderID, "q"), q);
	for (int x = 0; x < 3; x++)
		glUniform3f(glGetUniformLocation(shaderID, ("light[" + std::to_string(x) + "]").c_str()), light[x].x,light[x].y,light[x].z);

	glUniform3f(glGetUniformLocation(shaderID, "camPos"), camPos.x,camPos.y,camPos.z);
	
	glEnable(GL_DEPTH_TEST);

	su.setScale(0.5f);
	su.setRotation(180.0f, 'y');

	//parametros texturas
	
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(0);

		su.setScale(escala);

		su.setRotation(rotateX, 'x');
		su.setRotation(rotateY, 'y');
		su.setRotation(rotateZ, 'z');

		su.setTransf(dir_a,dir_d,dir_s,dir_w,dir_i,dir_k);

		dir_a=dir_d=dir_s=dir_w=dir_i=dir_k = 0;
		rotateZ=rotateX=rotateY = 0;
		escala = 1;
		
		su.draw(modelLoc);							

		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		
		rotateX += 1.7f;		
	}

	if (key == GLFW_KEY_Y && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		
		rotateY += 1.7f;		
	}

	if (key == GLFW_KEY_Z && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		rotateZ += 1.7f;
	}
	
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		
		dir_a -= 0.02f;
	}

	if (key == GLFW_KEY_D && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		dir_d += 0.02f;
	}
	
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		dir_w += 0.02f;		
	}
	
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		dir_s -= 0.02f;	
	}

	if (key == GLFW_KEY_I && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		dir_i += 0.02f;		
	}
	
	if (key == GLFW_KEY_K && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		dir_k -= 0.02f;
	}
	
	if (key == GLFW_KEY_LEFT_BRACKET && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		escala += 0.02f;		
	}
	
	if (key == GLFW_KEY_RIGHT_BRACKET && (action == GLFW_PRESS || GLFW_REPEAT))
	{
		escala -= 0.02f;		
	}
	
	if (key == GLFW_KEY_F && action == GLFW_RELEASE)
	{
		imodelo =  (1+imodelo) % 2;		
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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
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
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
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
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
 
	GLfloat vertices[] = {

		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b
		-0.5, -0.5, 0.5, 1.0, 0.0, 0.0,   // 2 inferior - +z 
		 0.5,  0.5, 0.5, 0.0, 1.0, 0.0,   // 1 superior - +z
		 0.5, -0.5, 0.5, 1.0, 0.0, 1.0,   // 2 superior - +z
		-0.5,  0.5, 0.5, 1.0, 1.0, 0.0,   // 1 inferior - +z

		-0.5, -0.5,  -0.5, 1.0, 0.0, 0.2,   // 2 inferior - z
		 0.5,  0.5,  -0.5, 0.0, 1.0, 0.2,   // 1 superior - z
		 0.5, -0.5,  -0.5, 0.2, 0.0, 1.0,   // 2 superior - z
		-0.5,  0.5,  -0.5, 1.0, 0.5, 1.0,   // 1 inferior - z


	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	unsigned int indices[] = {
								0, 1, 2, 
								3, 1, 0,

								2, 6, 4,
								0, 2, 4,

								7, 6, 5,
								4, 6, 7,

								1, 5, 7,
								3, 1, 7,

								1, 5, 2,
								2, 5, 6,

								3, 7, 0,
								0, 7, 4
									
		
	};

	
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	
	return VAO;
}

