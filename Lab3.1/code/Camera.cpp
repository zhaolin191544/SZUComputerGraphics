#include "Camera.h"

Camera::Camera() { updateCamera(); };
Camera::~Camera() {};


glm::mat4 Camera::lookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{	
	// 计算相机观察矩阵
	glm::vec3 n = glm::normalize(glm::vec3(eye - at));            // 前向量
	glm::vec3 u = glm::normalize(glm::cross(glm::vec3(up), n));   // 右向量 
	glm::vec3 v = glm::cross(n, u);                               // 上向量
    glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = u.x;
	c[0][1] = u.y;
	c[0][2] = u.z;
	c[1][0] = v.x;
	c[1][1] = v.y;
	c[1][2] = v.z;
	c[2][0] = n.x;
	c[2][1] = n.y;
	c[2][2] = n.z;
	c[0][3] = -glm::dot(u, glm::vec3(eye));
	c[1][3] = -glm::dot(v, glm::vec3(eye));
	c[2][3] = -glm::dot(n, glm::vec3(eye));
	c = glm::transpose(c);
    
	//glm::mat4 c = glm::lookAt(glm::vec3(eye), glm::vec3(at), glm::vec3(up));
	return c;
}


glm::mat4 Camera::ortho(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 计算正交投影矩阵
	glm::mat4 c(1.0f);
	c[0][0] = 2.0f / (right - left);
    c[1][1] = 2.0f / (top - bottom);
	c[2][2] = -2.0f / (zFar - zNear);
	c[0][3] = -(right + left) / (right - left);
	c[1][3] = -(top + bottom) / (top - bottom);
	c[2][3] = -(zFar + zNear) / (zFar - zNear);
	c = glm::transpose(c);
    //glm::mat4 c = glm::ortho(left, right, bottom, top, zNear, zFar);
	return c;
}


glm::mat4 Camera::perspective(const GLfloat fov, const GLfloat aspect,
	const GLfloat zNear, const GLfloat zFar)
{
	// 计算透视投影矩阵
	GLfloat tanHalfFovy = tan(glm::radians(fov) / 2.0f);
	glm::mat4 c(0.0f);
	c[0][0] = 1.0f / (aspect * tanHalfFovy);
	c[1][1] = 1.0f / tanHalfFovy;
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[3][2] = -1.0f;
	c[2][3] = -(2.0f * zFar * zNear) / (zFar - zNear);
    c[3][3] = 0.0f;
	c = glm::transpose(c);
	//glm::mat4 c = glm::perspective(fov, aspect, zNear, zFar);
	return c;
}


glm::mat4 Camera::frustum(const GLfloat left, const GLfloat right,
	const GLfloat bottom, const GLfloat top,
	const GLfloat zNear, const GLfloat zFar)
{
	// 计算任意视锥体投影矩阵
	glm::mat4 c = glm::mat4(1.0f);
	c[0][0] = 2.0 * zNear / (right - left);
	c[0][2] = (right + left) / (right - left);
	c[1][1] = 2.0 * zNear / (top - bottom);
	c[1][2] = (top + bottom) / (top - bottom);
	c[2][2] = -(zFar + zNear) / (zFar - zNear);
	c[2][3] = -2.0 * zFar * zNear / (zFar - zNear);
	c[3][2] = -1.0;
	c[3][3] = 0.0;
	c = glm::transpose(c);
	return c;
}


void Camera::updateCamera()
{
	// 更新相机位置和方向
	float eyex = radius * cos(glm::radians(upAngle)) * sin(glm::radians(rotateAngle));
	float eyey = radius * sin(glm::radians(upAngle));
	float eyez = radius * cos(glm::radians(upAngle)) * cos(glm::radians(rotateAngle));

	eye = glm::vec4(eyex, eyey, eyez, 1.0);

	// 根据upAngle调整up向量方向
	if (upAngle > 90.0f && upAngle < 270.0f)
	{
		up = glm::vec4(0.0, -1.0, 0.0, 0.0);
	}
	else
	{
		up = glm::vec4(0.0, 1.0, 0.0, 0.0);
	}

	at = glm::vec4(0.0, 0.0, 0.0, 1.0);
}


void Camera::keyboard(int key, int action, int mode)
{
	// 处理键盘事件
	if (key == GLFW_KEY_X && action == GLFW_PRESS && mode == 0x0000)
	{
		rotateAngle += 5.0;
	}
	else if(key == GLFW_KEY_X && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		rotateAngle -= 5.0;
	}
	else if(key == GLFW_KEY_Y && action == GLFW_PRESS && mode == 0x0000)
	{
		upAngle += 5.0;
		if (upAngle > 180)
			upAngle = 180;
	}
	else if(key == GLFW_KEY_Y && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		upAngle -= 5.0;
		if (upAngle < -180)
			upAngle = -180;
	}
	else if(key == GLFW_KEY_R && action == GLFW_PRESS && mode == 0x0000)
	{
		radius += 0.1;
	}
	else if(key == GLFW_KEY_R && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		radius -= 0.1;
	}
	else if(key == GLFW_KEY_F && action == GLFW_PRESS && mode == 0x0000)
	{
		fov += 5.0;
	}
	else if(key == GLFW_KEY_F && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		fov -= 5.0;
	}
	else if(key == GLFW_KEY_A && action == GLFW_PRESS && mode == 0x0000)
	{
		aspect += 0.1;
	}
	else if(key == GLFW_KEY_A && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		aspect -= 0.1;
	}
	else if(key == GLFW_KEY_S && action == GLFW_PRESS && mode == 0x0000)
	{
		scale += 0.1;
	}
	else if(key == GLFW_KEY_S && action == GLFW_PRESS && mode == GLFW_MOD_SHIFT)
	{
		scale -= 0.1;
	}
	else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS && mode == 0x0000)
	{
		radius = 4.0;
		rotateAngle = 0.0;
		upAngle = 0.0;
		fov = 45.0;
		aspect = 1.0;
		scale = 1.5;
	}
}
