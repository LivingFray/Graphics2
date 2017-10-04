#include <iostream>

#include "renderer\Scene.h"
#include "renderer\SceneObject.h"


int main() {
	Scene aScene;
	SceneObject parentObj;
	SceneObject childObj;
	SceneObject grandchildObj;
	std::cout << grandchildObj.setParent(&childObj) << "=1" << std::endl;
	std::cout << childObj.setParent(&parentObj) << "=1" << std::endl;
	std::cout << parentObj.setParent(&aScene) << "=1" << std::endl;

	std::cout << parentObj.hasChild(&childObj) << "=1" << std::endl;
	std::cout << parentObj.hasChild(&grandchildObj) << "=0" << std::endl;
	std::cout << parentObj.hasDescendent(&grandchildObj) << "=1" << std::endl;
	std::cout << childObj.hasDescendent(&grandchildObj) << "=1" << std::endl;
	std::cout << childObj.hasDescendent(&parentObj) << "=0" << std::endl;

	std::cout << parentObj.setParent(&grandchildObj) << "=0" << std::endl;

	int x;
	std::cin >> x;
	return 0;
}