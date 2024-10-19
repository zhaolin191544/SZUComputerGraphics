/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris Implementation with Difficulty Selection
 * ========================================================================
 *
 * - 本代码已实现以下功能：
 *   1) 绘制棋盘格和所有形状的方块（'L', 'J', 'Z', 'S', 'O', 'T', 'I'）
 *   2) 键盘左/右/下键控制方块的移动，上键旋转方块
 *   3) 随机生成方块并赋上不同的颜色
 *   4) 方块的自动向下移动
 *   5) 方块之间、方块与边界之间的碰撞检测
 *   6) 棋盘格中每一行填充满之后自动消除
 *   7) 游戏结束判断和重新开始功能
 *   8) 游戏难度选择（简单和困难）
 */

#include "Angel.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>

int starttime;          // 控制方块向下移动时间
int rotation = 0;       // 控制当前窗口中的方块旋转
glm::vec2 tile[4];      // 表示当前窗口中的方块
bool gameover = false;  // 游戏结束控制变量
int xsize;              // 窗口大小
int ysize;
double lastFallTime;
double fallInterval;    // 方块下落的时间间隔
int score = 0;          // 记录得分

// 单个网格大小
int tile_width = 33;

int difficulty;  

// 网格布大小
int board_width;
int board_height;

// 网格三角面片的顶点数量
int points_num;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
int board_line_num;

// 定义所有形状的方块和它们的旋转状态
glm::vec2 allRotationsLshape[4][4] = {
    { glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(-1,-1) },   //   "L"
    { glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0,-1), glm::vec2(1, -1) },
    { glm::vec2(1, 1), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,  0) },
    { glm::vec2(-1,1), glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1) }
};
glm::vec2 allRotationsJshape[4][4] = {
    { glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(1, 0), glm::vec2(1, -1) },
    { glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 1) },
    { glm::vec2(-1, 1), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0) },
    { glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(-1, -1) }
};
glm::vec2 allRotationsIshape[2][4] = {
    { glm::vec2(-2, 0), glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0) }, // 水平
    { glm::vec2(0, -1), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, 2) }   // 垂直
};
glm::vec2 allRotationsOshape[1][4] = {
    { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, -1), glm::vec2(1, -1) }
};
glm::vec2 allRotationsTshape[4][4] = {
    { glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, -1) },
    { glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(1, 0) },
    { glm::vec2(-1, 0), glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1) },
    { glm::vec2(0, 1), glm::vec2(0, 0), glm::vec2(0, -1), glm::vec2(-1, 0) }
};
glm::vec2 allRotationsSshape[2][4] = {
    { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, -1), glm::vec2(-1, -1) },
    { glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 0), glm::vec2(1, -1) }
};
glm::vec2 allRotationsZshape[2][4] = {
    { glm::vec2(0, 0), glm::vec2(-1, 0), glm::vec2(0, -1), glm::vec2(1, -1) },
    { glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(-1, 0), glm::vec2(-1, -1) }
};

// 绘制窗口的颜色变量
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0);
glm::vec4 white  = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 black  = glm::vec4(0.0, 0.0, 0.0, 1.0);

// 定义颜色数组
glm::vec4 colors[] = {
    glm::vec4(1.0, 0.5, 0.0, 1.0),   // 橙色
    glm::vec4(0.0, 0.5, 1.0, 1.0),   // 蓝色
    glm::vec4(0.5, 0.0, 1.0, 1.0),   // 紫色
    glm::vec4(1.0, 0.0, 0.5, 1.0),   // 粉色
    glm::vec4(0.0, 1.0, 0.5, 1.0),   // 青色
    glm::vec4(0.5, 1.0, 0.0, 1.0),   // 黄绿色
    glm::vec4(1.0, 0.5, 0.0, 1.0)    // 橙色
};

// 当前方块的位置（以棋盘格的左下角为原点的坐标系）
glm::vec2 tilepos;

// 当前方块的类型
enum ShapeType { LShape, JShape, IShape, OShape, TShape, SShape, ZShape };
ShapeType currentShape;

// 当前方块的颜色
glm::vec4 tileColor;

// 布尔数组表示棋盘格的某位置是否被方块填充
std::vector<std::vector<bool>> board;

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
std::vector<glm::vec4> board_colours;

GLuint locxsize;
GLuint locysize;

GLuint vao[3];
GLuint vbo[6];

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour)
{
    // 检查坐标有效性
    if (pos.x < 0 || pos.x >= board_width || pos.y < 0 || pos.y >= board_height)
        return;

    int index = static_cast<int>(6 * (board_width * pos.y + pos.x));
    for (int i = 0; i < 6; i++)
        board_colours[index + i] = colour;

    glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

    // 计算偏移量，在适当的位置赋上颜色
    int offset = index * sizeof(glm::vec4);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

    // 每个方块包含四个格子
    for (int i = 0; i < 4; i++)
    {
        // 计算格子的坐标值
        GLfloat x = tilepos.x + tile[i].x;
        GLfloat y = tilepos.y + tile[i].y;

        glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width), .4, 1);
        glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);
        glm::vec4 p3 = glm::vec4(tile_width*2 + (x * tile_width), tile_width + (y * tile_width), .4, 1);
        glm::vec4 p4 = glm::vec4(tile_width*2 + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);

        // 每个格子包含两个三角形，所以有6个顶点坐标
        glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
        glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), newpoints);
    }
    glBindVertexArray(0);
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
bool checkvalid(glm::vec2 cellpos)
{
    int x = static_cast<int>(cellpos.x);
    int y = static_cast<int>(cellpos.y);

    if (x >= 0 && x < board_width && y >= 0 && y < board_height)
    {
        return !board[x][y]; // 返回true如果该位置未被占用
    }
    else
    {
        return false;
    }
}

// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块，
// 在游戏结束的时候判断，没有足够的空间来生成新的方块。
void newtile()
{
    // 将新方块放于棋盘格的最上行中间位置并设置默认的旋转方向
    tilepos = glm::vec2(board_width / 2, board_height - 1);
    rotation = 0;

    // 随机选择一个形状
    currentShape = static_cast<ShapeType>(rand() % 7);

    // 设置方块的形状
    switch (currentShape)
    {
        case LShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsLshape[rotation][i];
            break;
        case JShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsJshape[rotation][i];
            break;
        case IShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsIshape[rotation][i];
            break;
        case OShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsOshape[rotation][i];
            break;
        case TShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsTshape[rotation][i];
            break;
        case SShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsSshape[rotation][i];
            break;
        case ZShape:
            for (int i = 0; i < 4; i++)
                tile[i] = allRotationsZshape[rotation][i];
            break;
    }

    // 检查游戏是否结束
    for (int i = 0; i < 4; i++)
    {
        glm::vec2 pos = tile[i] + tilepos;
        if (!checkvalid(pos))
        {
            gameover = true;
            std::cout << "游戏结束！您的得分是：" << score << std::endl;
            return;
        }
    }

    updatetile();

    // 给新方块赋上颜色
    tileColor = colors[rand() % 7];

    glm::vec4 newcolours[24];
    for (int i = 0; i < 24; i++)
        newcolours[i] = tileColor;

    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
    int maxRotations = 4; // 默认4种旋转状态

    // 根据形状调整最大旋转次数
    switch (currentShape)
    {
        case IShape:
        case SShape:
        case ZShape:
            maxRotations = 2;
            break;
        case OShape:
            maxRotations = 1;
            break;
    }

    // 计算下一个旋转状态
    int nextRotation = (rotation + 1) % maxRotations;

    // 获取旋转后的新位置
    glm::vec2 newPositions[4];
    for (int i = 0; i < 4; i++)
    {
        switch (currentShape)
        {
            case LShape:
                newPositions[i] = allRotationsLshape[nextRotation][i] + tilepos;
                break;
            case JShape:
                newPositions[i] = allRotationsJshape[nextRotation][i] + tilepos;
                break;
            case IShape:
                newPositions[i] = allRotationsIshape[nextRotation][i] + tilepos;
                break;
            case OShape:
                newPositions[i] = allRotationsOshape[nextRotation][i] + tilepos;
                break;
            case TShape:
                newPositions[i] = allRotationsTshape[nextRotation][i] + tilepos;
                break;
            case SShape:
                newPositions[i] = allRotationsSshape[nextRotation][i] + tilepos;
                break;
            case ZShape:
                newPositions[i] = allRotationsZshape[nextRotation][i] + tilepos;
                break;
        }
    }
    // 检查新位置是否有效
    bool canRotate = true;
    for (int i = 0; i < 4; i++)
    {
        if (!checkvalid(newPositions[i]))
        {
            canRotate = false;
            break;
        }
    }
    // 如果可以旋转，更新方块位置
    if (canRotate)
    {
        rotation = nextRotation;
        for (int i = 0; i < 4; i++)
        {
            switch (currentShape)
            {
                case LShape:
                    tile[i] = allRotationsLshape[rotation][i];
                    break;
                case JShape:
                    tile[i] = allRotationsJshape[rotation][i];
                    break;
                case IShape:
                    tile[i] = allRotationsIshape[rotation][i];
                    break;
                case OShape:
                    tile[i] = allRotationsOshape[rotation][i];
                    break;
                case TShape:
                    tile[i] = allRotationsTshape[rotation][i];
                    break;
                case SShape:
                    tile[i] = allRotationsSshape[rotation][i];
                    break;
                case ZShape:
                    tile[i] = allRotationsZshape[rotation][i];
                    break;
            }
        }
        updatetile();
    }
}

// 检查棋盘格在row行有没有被填充满
void clearFullRows()
{
    int linesCleared = 0;

    for (int y = 0; y < board_height; y++)
    {
        bool rowFull = true;
        for (int x = 0; x < board_width; x++)
        {
            if (!board[x][y])
            {
                rowFull = false;
                break;
            }
        }

        if (rowFull)
        {
            linesCleared++;

            // 移除已填满的行
            for (int yy = y; yy < board_height - 1; yy++)
            {
                for (int x = 0; x < board_width; x++)
                {
                    board[x][yy] = board[x][yy + 1];
                    glm::vec4 color = board_colours[6 * (board_width * (yy + 1) + x)];
                    changecellcolour(glm::vec2(x, yy), color);
                }
            }

            // 清除顶部行
            for (int x = 0; x < board_width; x++)
            {
                board[x][board_height - 1] = false;
                changecellcolour(glm::vec2(x, board_height - 1), black);
            }

            y--; // 检查同一行，因为上面的行已经下移
        }
    }

    // 更新得分
    score += linesCleared * 100;
    if (linesCleared > 0)
    {
        std::cout << "当前得分：" << score << std::endl;
    }

    // 根据得分调整下落速度（简单模式）
    if (difficulty == 1 && score % 1000 >= 0 && fallInterval > 0.2)
    {
        fallInterval -= 0.03;
    }
}

// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
    // 每个格子
    for (int i = 0; i < 4; i++)
    {
        // 获取格子在棋盘格上的坐标
        int x = (tile[i] + tilepos).x;
        int y = (tile[i] + tilepos).y;
        // 将格子对应在棋盘格上的位置设置为填充
        board[x][y] = true;
        // 并将相应位置的颜色修改
        changecellcolour(glm::vec2(x, y), tileColor);
    }
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction)
{
    // 计算移动之后的方块的位置坐标
    glm::vec2 newtilepos[4];
    for (int i = 0; i < 4; i++)
        newtilepos[i] = tile[i] + tilepos + direction;

    // 检查移动之后的有效性
    bool canMove = true;
    for (int i = 0; i < 4; i++)
    {
        if (!checkvalid(newtilepos[i]))
        {
            canMove = false;
            break;
        }
    }

    if (canMove)
    {
        // 有效：移动该方块
        tilepos.x = tilepos.x + direction.x;
        tilepos.y = tilepos.y + direction.y;

        updatetile();

        return true;
    }

    return false;
}

// 重新启动游戏
void restart()
{
    // 重置棋盘
    for (int x = 0; x < board_width; x++)
    {
        for (int y = 0; y < board_height; y++)
        {
            board[x][y] = false;
            changecellcolour(glm::vec2(x, y), black);
        }
    }

    // 重置游戏变量
    gameover = false;
    tilepos = glm::vec2(board_width / 2, board_height - 1);
    rotation = 0;
    score = 0;
    fallInterval = (board_width == 10) ? 0.5 : 0.3; // 根据难度重置下落速度
    lastFallTime = glfwGetTime();

    // 开始新方块
    newtile();
}

// 游戏渲染部分
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1i(locxsize, xsize);
    glUniform1i(locysize, ysize);

    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格 (width * height * 2 个三角形)
    glBindVertexArray(vao[2]);
    glDrawArrays(GL_TRIANGLES, 0, 24);   // 绘制当前方块 (8 个三角形)
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_LINES, 0, board_line_num * 2 );       // 绘制棋盘格的线

}

// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
    xsize = w;
    ysize = h;
    glViewport(0, 0, w, h);
}

// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(!gameover)
    {
        switch(key)
        {
            // 控制方块的移动方向，更改形态
            case GLFW_KEY_UP:   // 向上按键旋转方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT)
                {
                    rotate();
                    break;
                }
                else
                {
                    break;
                }
            case GLFW_KEY_DOWN: // 向下按键移动方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT){
                    if (!movetile(glm::vec2(0, -1)))
                    {
                        settile();
                        clearFullRows();
                        newtile();
                        break;
                    }
                    else
                    {
                        break;
                }
                }
            case GLFW_KEY_LEFT:  // 向左按键移动方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT){
                    movetile(glm::vec2(-1, 0));
                    break;
                }
                else
                {
                    break;
                }
            case GLFW_KEY_RIGHT: // 向右按键移动方块
                if (action == GLFW_PRESS || action == GLFW_REPEAT){
                    movetile(glm::vec2(1, 0));
                    break;
                }
                else
                {
                    break;
                }
            // 游戏设置。
            case GLFW_KEY_ESCAPE:
                if(action == GLFW_PRESS){
                    exit(EXIT_SUCCESS);
                    break;
                }
                else
                {
                    break;
                }
            case GLFW_KEY_Q:
                if(action == GLFW_PRESS){
                    exit(EXIT_SUCCESS);
                    break;
                }
                else
                {
                    break;
                }

            case GLFW_KEY_R:
                if(action == GLFW_PRESS){
                    restart();
                    break;
                }
                else
                {
                    break;
                }
        }
    }
}

void init()
{
    // 初始化棋盘格，这里用画直线的方法绘制网格
    // 包含竖线 board_width+1 条
    // 包含横线 board_height+1 条
    // 一条线2个顶点坐标，并且每个顶点一个颜色值

    std::vector<glm::vec4> gridpoints(board_line_num * 2);
    std::vector<glm::vec4> gridcolours(board_line_num * 2);

    // 绘制网格线
    // 纵向线
    for (int i = 0; i < (board_width+1); i++)
    {
        gridpoints[2*i] = glm::vec4((tile_width + (tile_width * i)), tile_width, 0, 1);
        gridpoints[2*i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height+1) * tile_width, 0, 1);
    }

    // 水平线
    for (int i = 0; i < (board_height+1); i++)
    {
        gridpoints[ 2*(board_width+1) + 2*i ] = glm::vec4(tile_width, (tile_width + (tile_width * i)), 0, 1);
        gridpoints[ 2*(board_width+1) + 2*i + 1 ] = glm::vec4((board_width+1) * tile_width, (tile_width + (tile_width * i)), 0, 1);
    }

    // 将所有线赋成白色
    for (int i = 0; i < (board_line_num * 2); i++)
        gridcolours[i] = white;

    // 初始化棋盘格，并将没有被填充的格子设置成黑色
    std::vector<glm::vec4> boardpoints(points_num);
    board_colours.resize(points_num, black);

    // 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子
    for (int i = 0; i < board_height; i++)
        for (int j = 0; j < board_width; j++)
        {
            glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width), .5, 1);
            glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
            glm::vec4 p3 = glm::vec4(tile_width*2 + (j * tile_width), tile_width + (i * tile_width), .5, 1);
            glm::vec4 p4 = glm::vec4(tile_width*2 + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
            boardpoints[ 6 * ( board_width * i + j ) + 0 ] = p1;
            boardpoints[ 6 * ( board_width * i + j ) + 1 ] = p2;
            boardpoints[ 6 * ( board_width * i + j ) + 2 ] = p3;
            boardpoints[ 6 * ( board_width * i + j ) + 3 ] = p2;
            boardpoints[ 6 * ( board_width * i + j ) + 4 ] = p3;
            boardpoints[ 6 * ( board_width * i + j ) + 5 ] = p4;
        }

    // 将棋盘格所有位置的填充与否都设置为false（没有被填充）
    board.resize(board_width, std::vector<bool>(board_height, false));

    // 载入着色器
    std::string vshader, fshader;
    vshader = "shaders/vshader.glsl";
    fshader = "shaders/fshader.glsl";
    GLuint program = InitShader(vshader.c_str(), fshader.c_str());
    glUseProgram(program);

    locxsize = glGetUniformLocation(program, "xsize");
    locysize = glGetUniformLocation(program, "ysize");

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    GLuint vColor = glGetAttribLocation(program, "vColor");

    glGenVertexArrays(3, &vao[0]);
    glBindVertexArray(vao[0]);       // 棋盘格顶点

    glGenBuffers(2, vbo);

    // 棋盘格顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridpoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // 棋盘格顶点颜色
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridcolours.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColor);

    glBindVertexArray(vao[1]);       // 棋盘格每个格子

    glGenBuffers(2, &vbo[2]);

    // 棋盘格每个格子顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), boardpoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // 棋盘格每个格子顶点颜色
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), board_colours.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColor);

    glBindVertexArray(vao[2]);       // 当前方块

    glGenBuffers(2, &vbo[4]);

    // 当前方块顶点位置
    glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vPosition);

    // 当前方块顶点颜色
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColor);

    glBindVertexArray(0);

    glClearColor(0, 0, 0, 0);

    // 游戏初始化
    newtile();
    lastFallTime = glfwGetTime();
}

int main(int argc, char **argv)
{
    // 提示用户选择难度
    std::cout << "请选择游戏难度（1：简单，2：困难）：";
    std::cin >> difficulty;

    if (difficulty == 1) {
        // 简单难度
        board_width = 10;
        board_height = 20;
        xsize = 400;
        ysize = 720;
        fallInterval = 0.6; // 下落速度
    } else if (difficulty == 2) {
        // 困难难度
        board_width = 15;     // 增加宽度
        board_height = 25;    // 增加高度
        xsize = 600;          // 增加窗口宽度
        ysize = 900;          // 增加窗口高度
        fallInterval = 0.55;   // 加快下落速度
    } else {
        std::cout << "无效的选择，默认设置为简单难度。" << std::endl;
        // 默认简单难度
        board_width = 10;
        board_height = 20;
        xsize = 400;
        ysize = 720;
        fallInterval = 0.5;
    }

    // 重新计算依赖于 board_width 和 board_height 的变量
    points_num = board_height * board_width * 6;
    board_line_num = (board_width + 1) + (board_height + 1);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // 创建窗口。
    GLFWwindow* window = glfwCreateWindow(xsize, ysize, "2022220036_赵麟_期中大作业", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    srand(static_cast<unsigned int>(time(0))); // 初始化随机种子

    init();
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();

        if (!gameover && (currentTime - lastFallTime) >= fallInterval)
        {
            if (!movetile(glm::vec2(0, -1)))
            {
                settile();
                clearFullRows();
                newtile();
            }
            lastFallTime = currentTime;
        }

        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
