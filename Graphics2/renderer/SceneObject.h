#pragma once
/*
Scene Objects are any object that exists within a scene.
For example these can be:
-Objects with a physical location
-Objects that can be rendered (Renderables)
-The scene itself, technically
-Grouping objects
-Something else I haven't thought of at the time of writing this
*/
#include<vector>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
using std::vector;
class Scene;

class SceneObject {
public:
	SceneObject();
	SceneObject(SceneObject& other);
	virtual ~SceneObject();
	//Sets the object's parent to obj, making it a part of that object's scene
	bool setParent(SceneObject* obj);
	//Gets the parent of the object
	SceneObject* getParent() { return parent; };
	//Gets the children of the object
	const vector<SceneObject*>& getChildren() const { return children; };
	//Checks if the child is a direct child of the object
	bool hasChild(SceneObject* child);
	//Checks if the child is a descendent of the object
	bool hasDescendent(SceneObject* child);
	//Gets the transformation matrix local to this object
	glm::mat4 getLocalMatrix();
	//Gets the transformation matrix in relation to the scene
	glm::mat4 getGlobalMatrix();
	//Sets the position of the object
	void setPosition(glm::vec3 pos);
	//Sets the scale of the object
	void setScale(glm::vec3 scale);
	//Sets the rotation of the object
	void setRotation(glm::quat rot);
private:
	Scene* scene;
	SceneObject* parent;
	vector<SceneObject*> children;
	glm::vec3 pos;
	glm::vec3 scale;
	glm::quat rot;
	glm::mat4 localMat;
	glm::mat4 globalMat;
	void updateMatrix();
};

