#include "Portal.h"
#include "glm/gtc/matrix_transform.hpp"


Portal::Portal() {
	portal = Shader("shaders/portal.vert", "shaders/portal.frag");
}


Portal::~Portal() {
}

void Portal::initPortalMap() {
	int width, height;
	glfwGetWindowSize(OpenGLSetup::window, &width, &height);
	//Generate buffers
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &renderTo);
	//Setup offscreen texture
	glBindTexture(GL_TEXTURE_2D, renderTo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	//Create framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTo, 0);
	//Create render buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//Attach render buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(portal.getProgram());
	glUniform1i(glGetUniformLocation(portal.getProgram(), "portal"), 0);
	glUseProgram(0);
}

void Portal::render(Camera* cam, GLuint depthMap, glm::mat4& LSM) {
	if (renderView && portalSurface) {
		//Set camera to be in correct relative location
		renderView->setLocalMatrix(glm::inverse(this->getGlobalMatrix()) * cam->getGlobalMatrix());
		renderView->setPosition(renderView->getPosition() * portalScale);
		//Render target camera to texture
		renderView->render(fbo);
		//Hack the framebuffer back
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//Render portal using texture
		glUseProgram(portal.getProgram());
		glm::mat4 lsm = (cam->getProjection() * cam->getView());
		glUniformMatrix4fv(glGetUniformLocation(portal.getProgram(), "lightSpaceMatrix"), 1, false, &lsm[0][0]);
		portalSurface->setLocalMatrix(this->getGlobalMatrix());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, renderTo);
		glDepthMask(GL_FALSE);
		portalSurface->renderShadow(portal.getProgram());
		glDepthMask(GL_TRUE);
		glUseProgram(0);
	}
}
