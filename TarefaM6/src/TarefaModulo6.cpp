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

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Ler Json
#include "json.hpp" 

// Custom Classes
#include "Engine/Control.hpp"
#include "Engine/Character.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Light.hpp"
#include "Engine/Mesh.hpp"
#include "Engine/Curve.hpp"

//Leitura de arquivo de configuração e inicialização de objetos
void initializeConfiguration();

// Protótipo da função de callback de teclado para controle do objeto
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
// Função de callback do teclado para controle da camera
void key_callback_camera(GLFWwindow *window, int key, int scancode, int action, int mode);
//callback character
void key_callback_character(GLFWwindow *window, int key, int scancode, int action, int mode);
// Função de callback do mouse
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Protótipos das funções
int setupShader();
GLuint loadSimpleOBJ(string filePATH, int &nVertices);
GLuint loadTexture(string filePath, int &width, int &height);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"(
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
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);
		vertexColor = color;
		texCoord = tex_coord;
		fragPos = vec3(model * vec4(position, 1.0));
		scaledNormal = vec3(model * vec4(normal, 1.0));
	})";

// Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"(
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

	uniform bool useWhiteColor;

	void main()
	{
		if (!useWhiteColor)
		{
			//vec3 finalColor = vertexColor;
			//Cor da textura
			vec3 finalColor = texture(tex_buffer, texCoord).rgb;

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
		}
		else
		{
			Color = vec4(1.0,1.0,1.0,1.0);
		}
	})";


vector<Character *> character;
Camera* cameraControl;
Light* light;
Mesh* pontosControleMesh;
Mesh* pontosCurva;
Mesh* pontos;
Curve* curva;

enum TipoObjeto
{
    CHARACTER,
    CAMERA
    //LUZ ainda não implementado
};
struct{
	TipoObjeto type = CHARACTER;
	int index = 0;
} currentObject;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();
	
	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Tarefa M6 -- Vitor Henrique Spader!", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	
	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
	}
	
	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte *version = glGetString(GL_VERSION);	/* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
	
	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	
	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();
	
	GLuint modelLoc;
	
	glUseProgram(shaderID);
	
	initializeConfiguration();

	cameraControl->setShaderID(shaderID);
	cameraControl->setWindowSize(WIDTH, HEIGHT);
	cameraControl->updateUniformProperties();

	// Seta uniforms
	for (int x = 0; x < character.size(); x++)
	{
		// Control *controler = new Control(0.0f, 0.0f, 0.0f, 0.1f);
		// controler->setScale(-3);
		// character[x] = new Character(controler, "../assets/Modelos3D/Suzanne.obj", true);
		character[x]->initializeUniformProperties(shaderID);
		//character[x]->setModelLoc(modelLoc);
	}

	light->updateUniformData(shaderID);

	vector<glm::vec3> pontosControle;
	pontosControle.push_back(glm::vec3(-1.0, -1.0, 0.0));
	pontosControle.push_back(glm::vec3(-0.5, -0.5, 0.5));
	pontosControle.push_back(glm::vec3(0.0, 0.0, 1.0));
	pontosControle.push_back(glm::vec3(0.5, 0.5, 0.5));
	pontosControle.push_back(glm::vec3(1.0, 1.0, 0.0));
	pontosControle.push_back(glm::vec3(0.5, 0.5, -0.5));
	pontosControle.push_back(glm::vec3(0.0, 0.0, -1.0));
	pontosControle.push_back(glm::vec3(-0.5, -0.5, -0.5));
	pontosControle.push_back(glm::vec3(-1.0, -1.0, 0.0));
	pontosControle.push_back(glm::vec3(-1.0, -1.0, 0.0));

	//Utilizado somente para criação dos pontos de curva
	pontosControleMesh = new Mesh(
		pontosControle,
		glm::vec3(0.0, 0.0, 0.0),//pos
		glm::vec3(1.0, 1.0, 1.0),//dimensions
		0.0,//angle
		0.0,//vel
		GL_POINTS //draw mode
	);

	curva = new Curve(pontosControle, 30);

	pontosCurva = new Mesh(
		curva->getPoints(),//points 
		glm::vec3(0.0, 0.0, 0.0),//pos
		glm::vec3(1.0, 1.0, 1.0), // dimensions
		0.0, //angle
		0.0, //vel
		GL_LINE_LOOP // draw mode
	);

	glEnable(GL_DEPTH_TEST);

	int i = 0;
	float delta_time, curr_time, currentAngle=0.0f, prev_time=glfwGetTime(), FPS = 24.0;

	//Habilita rotação do eixo y
	character[1]->control->setRotateY(1);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cameraControl->updateUniformProperties();
		
		// glPointSize(1);
		character[0]->draw(shaderID, true);

		// glPointSize(50);
		glLineWidth(10);
		glUniform1i(glGetUniformLocation(shaderID, "useWhiteColor"), true);
		pontosCurva->draw(shaderID);
		glUniform1i(glGetUniformLocation(shaderID, "useWhiteColor"), false);

		glm::vec3 position = curva->getPoints()[i];
		character[1]->control->setPosition(position);
		character[1]->draw(shaderID, true);

		curr_time = glfwGetTime();
		delta_time = curr_time - prev_time;
		if (delta_time > 1.0/FPS)
		{	
			i = (i + 1) % curva->getPoints().size();
			prev_time = curr_time;
			//implementar direção
			glm::vec3 nextPos = curva->getPoints()[i];
			glm::vec3 dir = glm::normalize(nextPos - position);
			// Novo ângulo
			float targetAngle = atan2(dir.x, dir.z);

			// Corrige interpolação circular: evita salto de 360° → 0°
			float deltaAngle = targetAngle - currentAngle;
			if (deltaAngle > glm::pi<float>())
				deltaAngle -= glm::two_pi<float>();
			else if (deltaAngle < -glm::pi<float>())
				deltaAngle += glm::two_pi<float>();

			// Interpolação suave do ângulo
			currentAngle += deltaAngle * 0.1f;
			
			currentAngle = targetAngle;
			// Atualiza o personagem
			character[1]->control->setAngle(currentAngle);

			character[1]->draw(shaderID, true);
		}

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	for (Character *object : character)
	{
		delete object;
	}

	glfwTerminate();

	return 0;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	cameraControl->calculateCameraPositionByMouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraControl->calculateFov(yoffset);
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback_camera(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_A:
			cameraControl->moveLeft();
			break;

		case GLFW_KEY_D:
			cameraControl->moveRight();
			break;

		case GLFW_KEY_W:
			cameraControl->moveUp();
			break;

		case GLFW_KEY_S:
			cameraControl->moveDown();
			break;

		default:
			break;
		}
	}
}
// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		switch (currentObject.type)
		{
		case CHARACTER:
			currentObject.type = CAMERA;
			break;

		case CAMERA:
			currentObject.type = CHARACTER;
			break;
		
		default:
			break;
		}
	}

	if (action == GLFW_PRESS)
	{
		switch (currentObject.type)
		{
		case CAMERA:
			key_callback_camera(window, key, scancode, action, mode);
			break;
		default:
			key_callback_character(window, key, scancode, action, mode);
			break;
		}
	}
}

void key_callback_character(GLFWwindow *window, int key, int scancode, int action, int mode)
{

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;

		case GLFW_KEY_0:
			currentObject.index = 0;
			break;

		case GLFW_KEY_1:
			if (character.size() > 1)
				currentObject.index = 1;
			break;

		case GLFW_KEY_X:
			character[currentObject.index]->control->setRotateX(1);
			break;

		case GLFW_KEY_Y:
			character[currentObject.index]->control->setRotateY(1);
			break;

		case GLFW_KEY_Z:
			character[currentObject.index]->control->setRotateZ(1);
			break;

		case GLFW_KEY_RIGHT_BRACKET:
			character[currentObject.index]->control->setScale(1);
			break;

		case GLFW_KEY_LEFT_BRACKET:
			character[currentObject.index]->control->setScale(-1);
			break;

		case GLFW_KEY_A:
			character[currentObject.index]->control->setTranslateX(-1);
			break;

		case GLFW_KEY_D:
			character[currentObject.index]->control->setTranslateX(1);
			break;

		case GLFW_KEY_W:
			character[currentObject.index]->control->setTranslateZ(1);
			break;

		case GLFW_KEY_S:
			character[currentObject.index]->control->setTranslateZ(-1);
			break;

		case GLFW_KEY_I:
			character[currentObject.index]->control->setTranslateY(1);
			break;

		case GLFW_KEY_J:
			character[currentObject.index]->control->setTranslateY(-1);
			break;

		default:
			break;
		}
	}
}

// Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
//  shader simples e único neste exemplo de código
//  O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
//  fragmentShader source no iniçio deste arquivo
//  A função retorna o identificador do programa de shader
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
		cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
			 << infoLog << endl;
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
		cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
			 << infoLog << endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
			 << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

//Inicializa todos os objetos globais com base no arquivo de configurações
void initializeConfiguration()
{
	std::ifstream arquivo("../config/config.json");
    nlohmann::json config;
    arquivo >> config;

	//Inicializa Camera
	glm::vec3 position = glm::vec3(
		config["camera"]["position"][0].get<float>(), 
		config["camera"]["position"][1].get<float>(), 
		config["camera"]["position"][2].get<float>()
	);

	glm::vec3 front = glm::vec3(
		config["camera"]["front"][0].get<float>(), 
		config["camera"]["front"][1].get<float>(), 
		config["camera"]["front"][2].get<float>()
	);

	glm::vec3 up = glm::vec3(
		config["camera"]["up"][0].get<float>(), 
		config["camera"]["up"][1].get<float>(), 
		config["camera"]["up"][2].get<float>()
	);

	float moveSpeed = config["camera"]["moveSpeed"];

	cameraControl = new Camera(position, front, up, moveSpeed);

	// Inicializa Objeto
	for (const auto& object: config["object"])
	{
		const auto& pos = object["initial"]["position"];

		float x = pos[0].get<float>();
		float y = pos[1].get<float>();
		float z = pos[2].get<float>();
		float moveFactor = object["initial"]["moveFactor"].get<float>();

		Control* control = new Control(x, y, z, moveFactor);
		control->setScale(object["initial"]["scale"].get<float>());
		string filePath = object["objFilePath"].get<string>(); 
		bool hasTexture = object["hasTexture"].get<bool>();
		
		Character* tempCharacter = new Character(control, filePath, hasTexture);

		character.push_back(tempCharacter);
	}

	light = new Light(
		config["light"]["ka"].get<float>(),
		config["light"]["kd"].get<float>(),
		config["light"]["ks"].get<float>(),
		config["light"]["q"].get<float>()
	);
	for (const auto& position: config["light"]["position"])
	{
		light->addLightPosition(
			glm::vec3(
				position[0].get<float>(),
				position[1].get<float>(),
				position[2].get<float>()
			)
		);
	}
}