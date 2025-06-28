/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 13/08/2024
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace glm;

#include <cmath>

struct Mesh
{
    GLuint VAO;
    glm::vec3 pos;
    glm::vec3 dimensions;
    float angle;
    float vel;
    GLuint drawMode;
    GLuint nVertices;
};

struct Curve {
    std::vector<glm::vec3> controlPoints; // Pontos de controle da curva
    std::vector<glm::vec3> curvePoints;   // Pontos da curva
    glm::mat4 M;          // Matriz dos coeficientes da curva
};

////
void drawMesh(GLuint shaderID, Mesh mesh)
{
    //Usa o mesmo buffer de geometria parfa todos os sprites
    glBindVertexArray(mesh.VAO);

    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, mesh.pos);
    model = glm::rotate(model, glm::radians(mesh.angle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, mesh.dimensions);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(mesh.drawMode, 0, mesh.nVertices);

    glBindVertexArray(0);
}
////
// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int setupGeometry();
GLuint loadTexture(string filePath, int &width, int &height);

void drawGeometry(GLuint shaderID, GLuint VAO, vec3 position, vec3 dimensions, float angle, int nVertices, vec3 color= vec3(1.0,0.0,0.0), vec3 axis = (vec3(0.0, 0.0, 1.0)));
GLuint generateSphere(float radius, int latSegments, int lonSegments, int &nVertices);
 
// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 800;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar* vertexShaderSource = "#version 400\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
//...pode ter mais linhas de código aqui!
"gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

//Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar* fragmentShaderSource = "#version 400\n"
"uniform vec4 inputColor;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = inputColor;\n"
"}\n\0";

int setupQuad();
int createPointsVAO(vector<vec3> points);
void initializeBernsteinMatrix(glm::mat4 &matrix);
void generateBezierCurvePoints(Curve &curve, int numPoints);

vector<vec3> pontosControle;

int i = 0;
float prev_time = glfwGetTime();
float curr_time = glfwGetTime();
float delta_time = 0.0;
float FPS = 24.0;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Ola esfera iluminada!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
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

	int nVertices;

	Mesh mesh;
	mesh.VAO = setupQuad();
	mesh.drawMode = GL_TRIANGLE_STRIP;
	mesh.pos = vec3(0.0,0.0,0.0);
	mesh.angle = 0.0;
	mesh.dimensions = vec3(1.0, 1.0, 1.0);
	mesh.nVertices = 4;

	pontosControle.push_back(vec3(-0.5, -0.5, 0.01));
	pontosControle.push_back(vec3(-0.25, -0.15, 0.01));
	pontosControle.push_back(vec3(0.0, 0.0, 0.01));
	pontosControle.push_back(vec3(0.25, 0.15, 0.01));
	pontosControle.push_back(vec3(0.25, 0.3, 0.01));
	pontosControle.push_back(vec3(0.15, 0.3, 0.01));
	pontosControle.push_back(vec3(-0.25, 0.25, 0.01));

	Mesh pontosControleMesh;
	pontosControleMesh.VAO = createPointsVAO(pontosControle);
	pontosControleMesh.drawMode = GL_POINTS;
	pontosControleMesh.pos = vec3(0.0,0.0,0.0);
	pontosControleMesh.angle = 0.0;
	pontosControleMesh.dimensions = vec3(1.0, 1.0, 1.0);
	pontosControleMesh.nVertices = pontosControle.size();

	Curve curva;
	curva.controlPoints = pontosControle;
	initializeBernsteinMatrix(curva.M);
	generateBezierCurvePoints(curva, 10);

	Mesh pontosCurvaMesh;
	pontosCurvaMesh.VAO = createPointsVAO(curva.curvePoints);
	pontosCurvaMesh.drawMode = GL_POINTS;
	pontosCurvaMesh.pos = vec3(0.0,0.0,0.0);
	pontosCurvaMesh.angle = 0.0;
	pontosCurvaMesh.dimensions = vec3(1.0, 1.0, 1.0);
	pontosCurvaMesh.nVertices = curva.curvePoints.size();

	vector<vec3> pontoExemplo;
	pontoExemplo.push_back(vec3(0.0, 0.0, 0.01));
	Mesh ponto;
	ponto.VAO = createPointsVAO(pontoExemplo);
	ponto.drawMode = GL_POINTS;
	ponto.pos = vec3(0.0,0.0,0.0);
	ponto.angle = 0.0;
	ponto.dimensions = vec3(1.0, 1.0, 1.0);
	ponto.nVertices = pontoExemplo.size();

	glUseProgram(shaderID);

	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	mat4 projection = ortho(-1.0, 1.0, -1.0, 1.0, -5.0, 5.0);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_PROGRAM_POINT_SIZE);
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		//glLineWidth(10);
		glPointSize(20);

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f,1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniform4f(colorLoc, 0.0f, 0.0f, 1.0, 1.0f);
		drawMesh(shaderID, mesh);

		glUniform4f(colorLoc, 1.0f, 1.0f, 0.0, 1.0f);
		drawMesh(shaderID, pontosControleMesh);

		glPointSize(10);
		glUniform4f(colorLoc, 1.0f, 0.0f, 1.0, 0.0f);
		drawMesh(shaderID, pontosCurvaMesh);

		glPointSize(25);
		glUniform4f(colorLoc, 0.0f, 1.0f, 0.0, 0.0f);

		ponto.pos = curva.curvePoints[i];

		curr_time = glfwGetTime();
		delta_time = curr_time - prev_time;
		if (delta_time > 1.0/FPS)
		{
			i = (i + 1) % curva.curvePoints.size();
			prev_time = curr_time;
			//implementar direção
			//vec3 nextPos = curva.curvePoints[index];
			//vec3 dir = normalize(nextPos - position);
			//angle = atan2(dir.y, dir.x);
		}
		drawMesh(shaderID, ponto);
	
		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &mesh.VAO);
	glDeleteVertexArrays(1, &pontosControleMesh.VAO);
	glDeleteVertexArrays(1, &pontosCurvaMesh.VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
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
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
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
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
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
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
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
		// x    y    z   s    t    nx  ny    nz
		// T0
		-0.5, -0.5, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,    // v0
		 0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0,   // v1
		 0.0,  0.5, 0.0, 0.5, 1.0, 0.0, 0.0, -1.0 	  // v2
	};

	GLuint VBO, VAO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);
	// Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando:
	//  Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	//  Numero de valores que o atributo tem (por ex, 3 coordenadas xyz)
	//  Tipo do dado
	//  Se está normalizado (entre zero e um)
	//  Tamanho em bytes
	//  Deslocamento a partir do byte zero

	//Atributo posição - coord x, y, z - 3 valores
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	//Atributo coordenada de textura - coord s, t - 2 valores
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Atributo componentes vetor normal
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(5* sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

GLuint loadTexture(string filePath, int &width, int &height)
{
	GLuint texID; // id da textura a ser carregada

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Ajuste dos parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Carregamento da imagem usando a função stbi_load da biblioteca stb_image
	int nrChannels;

	unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) // jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else // assume que é 4 canais png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << filePath << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void drawGeometry(GLuint shaderID, GLuint VAO, vec3 position, vec3 dimensions, float angle, int nVertices, vec3 color, vec3 axis)
{
	// Matriz de modelo: transformações na geometria (objeto)
	mat4 model = mat4(1); // matriz identidade
	// Translação
	model = translate(model, position);
	// Rotação
	model = rotate(model, radians(angle), axis);
	// Escala
	model = scale(model, dimensions);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

	//glUniform4f(glGetUniformLocation(shaderID, "inputColor"), color.r, color.g, color.b, 1.0f); // enviando cor para variável uniform inputColor
																								//  Chamada de desenho - drawcall
																								//  Poligono Preenchido - GL_TRIANGLES
	glDrawArrays(GL_TRIANGLES, 0, nVertices);
}

GLuint generateSphere(float radius, int latSegments, int lonSegments, int &nVertices) {
    vector<GLfloat> vBuffer; // Posição + Cor + Normal + UV

    vec3 color = vec3(1.0f, 0.0f, 0.0f); // Laranja

    auto calcPosUVNormal = [&](int lat, int lon, vec3& pos, vec2& uv, vec3& normal) {
        float theta = lat * pi<float>() / latSegments;
        float phi = lon * 2.0f * pi<float>() / lonSegments;

        pos = vec3(
            radius * cos(phi) * sin(theta),
            radius * cos(theta),
            radius * sin(phi) * sin(theta)
        );

        uv = vec2(
            phi / (2.0f * pi<float>()),  // u
            theta / pi<float>()          // v
        );

        // Normal é a posição normalizada (posição/radius)
        normal = normalize(pos);
    };

    for (int i = 0; i < latSegments; ++i) {
        for (int j = 0; j < lonSegments; ++j) {
            vec3 v0, v1, v2, v3;
            vec2 uv0, uv1, uv2, uv3;
            vec3 n0, n1, n2, n3;

            calcPosUVNormal(i, j, v0, uv0, n0);
            calcPosUVNormal(i + 1, j, v1, uv1, n1);
            calcPosUVNormal(i, j + 1, v2, uv2, n2);
            calcPosUVNormal(i + 1, j + 1, v3, uv3, n3);

            // Primeiro triângulo
            vBuffer.insert(vBuffer.end(), { v0.x, v0.y, v0.z, color.r, color.g, color.b, n0.x, n0.y, n0.z, uv0.x, uv0.y });
            vBuffer.insert(vBuffer.end(), { v1.x, v1.y, v1.z, color.r, color.g, color.b, n1.x, n1.y, n1.z, uv1.x, uv1.y });
            vBuffer.insert(vBuffer.end(), { v2.x, v2.y, v2.z, color.r, color.g, color.b, n2.x, n2.y, n2.z, uv2.x, uv2.y });

            // Segundo triângulo
            vBuffer.insert(vBuffer.end(), { v1.x, v1.y, v1.z, color.r, color.g, color.b, n1.x, n1.y, n1.z, uv1.x, uv1.y });
            vBuffer.insert(vBuffer.end(), { v3.x, v3.y, v3.z, color.r, color.g, color.b, n3.x, n3.y, n3.z, uv3.x, uv3.y });
            vBuffer.insert(vBuffer.end(), { v2.x, v2.y, v2.z, color.r, color.g, color.b, n2.x, n2.y, n2.z, uv2.x, uv2.y });
        }
    }

    // Criar VAO e VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);

    // Layout da posição (location 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(0));
glEnableVertexAttribArray(0);

// Layout da cor (location 1)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
glEnableVertexAttribArray(1);

// Layout da normal (location 2)
glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
glEnableVertexAttribArray(2);

// Layout da UV (location 3)
glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    nVertices = vBuffer.size() / 11; // Cada vértice agora tem 11 floats!

    return VAO;
}

int setupQuad()
{
	GLfloat vertices[] = {
		// -0.5, 0.0, 0.5,
		// -0.5, 0.0, -0.5,
		// 0.5, 0.0, 0.5,
		// 0.5, 0.0, -0.5

		-0.5, 0.5, 0.0,
		-0.5, -0.5, 0.0,
		0.5, 0.5, 0.0,
		0.5, -0.5, 0.0
	};

	GLuint VBO,VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return VAO;
}

int createPointsVAO(vector<vec3> points)
{
	GLuint VBO, VAO;
	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(vec3), points.data(), GL_STATIC_DRAW);

	// Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void initializeBernsteinMatrix(glm::mat4 &matrix)
{
    matrix[0] = glm::vec4(-1.0f, 3.0f, -3.0f, 1.0f); // Primeira coluna
    matrix[1] = glm::vec4(3.0f, -6.0f, 3.0f, 0.0f);  // Segunda coluna
    matrix[2] = glm::vec4(-3.0f, 3.0f, 0.0f, 0.0f);  // Terceira coluna
    matrix[3] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);    // Quarta coluna

}


void generateBezierCurvePoints(Curve &curve, int numPoints) 
{
    curve.curvePoints.clear(); // Limpa quaisquer pontos antigos da curva

    initializeBernsteinMatrix(curve.M);
    // Calcular os pontos ao longo da curva com base em Bernstein
     // Loop sobre os pontos de controle em grupos de 4

    float piece = 1.0/ (float) numPoints;
    float t;
    for (int i = 0; i < curve.controlPoints.size() - 3; i+=3) {
        
        // Gera pontos para o segmento atual
        for (int j = 0; j < numPoints;j++) {
            t = j * piece;
            
            // Vetor t para o polinômio de Bernstein
            glm::vec4 T(t * t * t, t * t, t, 1);
            
            glm::vec3 P0 = curve.controlPoints[i];
			glm::vec3 P1 = curve.controlPoints[i + 1];
			glm::vec3 P2 = curve.controlPoints[i + 2];
			glm::vec3 P3 = curve.controlPoints[i + 3];

			glm::mat4x3 G(P0, P1, P2, P3);

            // Calcula o ponto da curva multiplicando tVector, a matriz de Bernstein e os pontos de controle
            glm::vec3 point = G * curve.M * T;
            
            curve.curvePoints.push_back(point);
        }
    }
}