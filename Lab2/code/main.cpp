#include "Angel.h"

#include <vector>
#include <fstream>
#include <string>

using namespace std;

int window;
typedef struct vIndex {
    unsigned int a, b, c;
    vIndex(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;

std::vector<glm::vec3> vertices;
std::vector<vec3i> faces;

int nVertices = 0;
int nFaces = 0;
int nEdges = 0;

std::vector<glm::vec3> points;
std::vector<glm::vec3> colors;

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;
bool isAnimating = false;
char rotateAxis = 'x'; // 旋转轴，默认绕X轴旋转

GLuint program;

void read_off(const std::string filename) {
    if (filename.empty()) return;
    std::ifstream fin;
    fin.open(filename);
    if (!fin) {
        printf("failed to open file\n");
        return;
    } else {
        printf("success\n");
        vertices.clear();
        faces.clear();

        string str;
        fin >> str;
        fin >> nVertices >> nFaces >> nEdges;
        double x, y, z;
        for (int i = 0; i < nVertices; i++) {
            fin >> x >> y >> z;
            vertices.push_back(glm::vec3(x, y, z));
        }
        int a, b, c;
        int vertices_num;
        for (int i = 0; i < nFaces; i++) {
            fin >> vertices_num >> a >> b >> c;
            faces.push_back(vec3i(a, b, c));
        }
    }
    fin.close();
}

void storeFacesPoints() {
    points.clear();
    colors.clear();
    
    // 定义颜色渐变的范围
    glm::vec3 color1 = glm::vec3(1.0, 0.0, 0.0); // 红色
    glm::vec3 color2 = glm::vec3(1.0, 1.0, 0.0); // 黄色
    glm::vec3 color3 = glm::vec3(0.0, 1.0, 0.0); // 绿色
    glm::vec3 color4 = glm::vec3(0.0, 1.0, 1.0); // 蓝色
    glm::vec3 color5 = glm::vec3(1.0, 0.0, 1.0); // 紫色
    
    for (int i = 0; i < nFaces; i++) {
        // 根据每个顶点的 y 坐标生成渐变颜色
        for (int j = 0; j < 3; ++j) {
            glm::vec3 vertex = vertices[(j == 0) ? faces[i].a : (j == 1) ? faces[i].b : faces[i].c];
            points.push_back(vertex);

            // 将 y 坐标归一化为 0-1 的范围
            float t = (vertex.y + 1.0f) / 2.0f;

            // 使用线性插值来生成渐变颜色
            glm::vec3 color;
            if (t < 0.25f) {
                color = glm::mix(color1, color2, t * 4.0f);
            } else if (t < 0.5f) {
                color = glm::mix(color2, color3, (t - 0.25f) * 4.0f);
            } else if (t < 0.75f) {
                color = glm::mix(color3, color4, (t - 0.5f) * 4.0f);
            } else {
                color = glm::mix(color4, color5, (t - 0.75f) * 4.0f);
            }

            colors.push_back(color); // 为每个顶点设置颜色
        }
    }
}



void init() {
    read_off("./Models/cow.off");
    storeFacesPoints();

    GLuint vao[1];
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3) + colors.size() * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, points.size() * sizeof(glm::vec3), &points[0]);
    glBufferSubData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);

    std::string vshader = "shaders/vshader.glsl";
    std::string fshader = "shaders/fshader.glsl";
    program = InitShader(vshader.c_str(), fshader.c_str());
    glUseProgram(program);

    GLuint pLocation = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(pLocation);
    glVertexAttribPointer(pLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    GLuint cLocation = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(cLocation);
    glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(points.size() * sizeof(glm::vec3)));

    glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (isAnimating) {
        if (rotateAxis == 'x') {
            rotationX += 1.0f; // 每次更新角度（可根据需要调整速度）
        } else if (rotateAxis == 'y') {
            rotationY += 1.0f;
        } else if (rotateAxis == 'z') {
            rotationZ += 1.0f;
        }
    }

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));

    GLuint rotationLoc = glGetUniformLocation(program, "rotationMatrix");
    glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, &rotationMatrix[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, points.size());
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (action == GLFW_PRESS) {
        std::cout << "Key pressed: " << key << std::endl; // 调试输出
        if (key == GLFW_KEY_X) {
            rotateAxis = 'x';
        } else if (key == GLFW_KEY_Y) {
            rotateAxis = 'y';
        } else if (key == GLFW_KEY_Z) {
            rotateAxis = 'z';
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Left mouse button pressed" << std::endl; // 调试输出
        isAnimating = true;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        std::cout << "Right mouse button pressed" << std::endl; // 调试输出
        isAnimating = false;
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(600, 600, "2022220036_赵麟_实验2", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    init();
    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}