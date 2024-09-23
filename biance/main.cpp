#include "Angel.h"
#include <string>

const glm::vec3 YELLOW(1.0, 0.84, 0.0); // 黄色
const int RECTANGLE_NUM_POINTS = 4;     // 矩形顶点数
const int SQUARE_NUM_POINTS = 4;        // 正方形顶点数

// 定义矩形顶点并进行旋转
void generateRotatedRectangle(glm::vec2 vertices[], glm::vec3 colors[], int startIndex, glm::vec2 center, float width, float height, float angle) {
    glm::vec2 points[RECTANGLE_NUM_POINTS] = {
        glm::vec2(-width / 2, -height / 2),  // 左下
        glm::vec2(width / 2, -height / 2),   // 右下
        glm::vec2(width / 2, height / 2),    // 右上
        glm::vec2(-width / 2, height / 2)    // 左上
    };

    for (int i = 0; i < RECTANGLE_NUM_POINTS; ++i) {
        float x_new = points[i].x * cos(angle) - points[i].y * sin(angle);
        float y_new = points[i].x * sin(angle) + points[i].y * cos(angle);
        vertices[startIndex + i] = glm::vec2(x_new, y_new) + center; // 旋转并移动到中心位置
    }

    for (int i = 0; i < RECTANGLE_NUM_POINTS; ++i) {
        colors[startIndex + i] = YELLOW;
    }
}

// 定义旋转正方形顶点
void generateRotatedSquare(glm::vec2 vertices[], glm::vec3 colors[], int startIndex, glm::vec2 center, float sideLength, float angle) {
    glm::vec2 points[SQUARE_NUM_POINTS] = {
        glm::vec2(-sideLength / 2, -sideLength / 2),  // 左下
        glm::vec2(sideLength / 2, -sideLength / 2),   // 右下
        glm::vec2(sideLength / 2, sideLength / 2),    // 右上
        glm::vec2(-sideLength / 2, sideLength / 2)    // 左上
    };

    // 旋转正方形
    for (int i = 0; i < SQUARE_NUM_POINTS; ++i) {
        float x_new = points[i].x * cos(angle) - points[i].y * sin(angle);
        float y_new = points[i].x * sin(angle) + points[i].y * cos(angle);
        vertices[startIndex + i] = glm::vec2(x_new, y_new) + center;
    }

    // 设置颜色为黄色
    for (int i = 0; i < SQUARE_NUM_POINTS; ++i) {
        colors[startIndex + i] = YELLOW;
    }
}

GLuint vao, program;
void init() {
    glm::vec2 vertices[(RECTANGLE_NUM_POINTS * 4) + (SQUARE_NUM_POINTS * 3)]; // 四个矩形 + 三个正方形的顶点
    glm::vec3 colors[(RECTANGLE_NUM_POINTS * 4) + (SQUARE_NUM_POINTS * 3)];

    // 生成四个矩形
    generateRotatedRectangle(vertices, colors, 0, glm::vec2(-0.1406, 0.413), 0.616, 0.2308, glm::radians(45.0f));
    generateRotatedRectangle(vertices, colors, 4, glm::vec2(0.209, 0.336), 0.3846, 0.2308, glm::radians(-45.0f));
    generateRotatedRectangle(vertices, colors, 8, glm::vec2(-0.1406, -0.413), 0.616, 0.2308, glm::radians(-45.0f));
    generateRotatedRectangle(vertices, colors, 12, glm::vec2(0.209, -0.336), 0.3846, 0.2308, glm::radians(45.0f));

    // 生成三个旋转的正方形，旋转45度
    generateRotatedSquare(vertices, colors, 16, glm::vec2(-0.5504, 0.0), 0.2308, glm::radians(45.0f));  // 左边
    generateRotatedSquare(vertices, colors, 20, glm::vec2(0.0, 0.0), 0.2308, glm::radians(45.0f));      // 中间
    generateRotatedSquare(vertices, colors, 24, glm::vec2(0.5504, 0.0), 0.2308, glm::radians(45.0f));   // 右边

    // 读取着色器并使用
    std::string vshader, fshader;
    vshader = "shaders/vshader.glsl";
    fshader = "shaders/fshader.glsl";
    program = InitShader(vshader.c_str(), fshader.c_str());
    glUseProgram(program);

    // 创建顶点缓存对象
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    GLuint location = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), BUFFER_OFFSET(0));

    GLuint cLocation = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(cLocation);
    glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(sizeof(vertices)));

    glClearColor(0.0, 0.0, 0.0, 1.0); // 黑色背景
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glBindVertexArray(vao);

    // 绘制四个矩形
    for (int i = 0; i < 4; ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, i * RECTANGLE_NUM_POINTS, RECTANGLE_NUM_POINTS);
    }

    // 绘制三个旋转的正方形
    for (int i = 0; i < 3; ++i) {
        glDrawArrays(GL_TRIANGLE_FAN, (16 + i * SQUARE_NUM_POINTS), SQUARE_NUM_POINTS);
    }

    glFlush();
}

int main(int argc, char **argv) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(512, 512, "2022220036_赵麟_实验一", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    init();
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Supported GLSL version is: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
