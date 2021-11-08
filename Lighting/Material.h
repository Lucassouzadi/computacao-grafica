#pragma once
#include <string>
#include <glm\gtc\type_ptr.hpp>

using namespace std;

class Material {
private:
	string id;
	glm::vec3 
		ka, //ambient 
		kd, //diffuse
		ks; //specular
	float ns; //shininess
	string mapKd; //texture
	unsigned int TID;
public:
	Material(string id);
	void setKa(glm::vec3 ka);
	void setKd(glm::vec3 kd);
	void setKs(glm::vec3 ks);
	void setNs(float ns);
	void setMapKd(string mapKd);
	string getId();
	glm::vec3 getKa();
	glm::vec3 getKd();
	glm::vec3 getKs();
	float getNs();
	string getMapKd();
};