#pragma once
/*
Cameras draw the scenes
*/
#include "SceneObject.h"
#include "OpenGLSetup.h"
#include "Shader.h"
class Camera :
	public SceneObject {
public:
	Camera();
	~Camera();
	// Sets the field of view of the camera
	void setFOV(GLfloat fov);
	// Gets the field of view of the camera
	GLfloat getFOV() const { return fov; };
	// Sets whether the camera uses perspective or orthographic projection
	void setPerspective(bool perspective);
	// Gets whether the camera uses perspective projection
	bool getPerspective() const { return perspective; };
	// Sets the near clipping distance of the camera
	void setNear(GLfloat near);
	// Gets the near clipping distance of the camera
	GLfloat getNear() const { return near; };
	// Sets the far clipping distance of the camera
	void setFar(GLfloat far);
	// Gets the far clipping distance of the camera
	GLfloat getFar() const { return far; };
	// Sets the width of the camera
	void setWidth(GLfloat width);
	// Gets the width of the camera
	GLfloat getWidth() const { return width; };
	// Sets the width of the camera
	void setHeight(GLfloat height);
	// Gets the width of the camera
	GLfloat getHeight() const { return height; };
	// Gets the projection matrix associated with the camera
	glm::mat4 getProjection();
	// Gets the view matrix associated with the camera
	glm::mat4 getView();
	// Whether the camera clears the scene before drawing
	bool clearOnDraw;
	// Renders the scene from the perspective of this camera
	void render(GLuint target = 0);
	// Initialises the shadow map
	void initShadowMap();
private:
	GLfloat width;
	GLfloat height;
	GLfloat fov;
	GLfloat near;
	GLfloat far;
	bool perspective;
	glm::mat4 proj;
	bool updateFlag;
	GLuint depthMap;
	Shader shadow;
	//TODO: In-game adjusting
	unsigned int shadowMapSize = 2048;
	GLuint fbo;
};
