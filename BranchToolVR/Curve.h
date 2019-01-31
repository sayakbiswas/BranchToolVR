#pragma once
#include "AbstractBaseObject.h"
#include <limits>

class DicomPointCloudObject;

class Curve : public AbstractBaseObject
{

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> instanced_colors = { glm::vec3(0.0, 1.0, 1.0), glm::vec3(1.0, 0.0, 0.0) };

	GLuint normals_buffer;
	GLuint instanced_positions_buffer;
	GLuint instanced_colors_buffer;

	DicomPointCloudObject* pointCloud;
	void Load();
	int Type();

public:
	Curve();
	~Curve();
	Curve(std::vector<glm::vec3> curvePoints);
	void SetPositions(std::vector<glm::vec3> curvePoints);
	std::vector<glm::vec3> GetPositions();
	void SetNormals(std::vector<glm::vec3> normals);
	std::vector<glm::vec3> GetNormals();
	void RenderCurve();
	void RenderCurveHighlight();
	void SetDicomPointCloudObject(DicomPointCloudObject* pointCloud);
	void SetControlPoints(std::vector<glm::vec3> controlPoints);
	std::vector<glm::vec3> GetControlPoints();
};
//
//class curveTree {
//
//	struct node {
//		Curve* nodeCurve;
//		node* parent;
//		std::vector<node*> children;
//		int depth;
//		int childIndex;
//	};
//
//	node* root;
//	node* prev;
//	node* temp;
//	node* curr;
//	int index = 0;
//	std::vector<glm::vec3> out;
//
//public:
//
//	curveTree() {
//		// Junk values for super root
//		root = new node();
//		clear(root);
//		root->nodeCurve = new Curve();
//		root->depth = -1;
//		root->childIndex = -1;
//		curr = root;
//		prev = new node();
//		temp = new node();
//		//std::cout << curr->children.size() << std::endl;
//	};
//
//	void clear(node* n) {
//		for (int i = 0; i < n->children.size(); i++) {
//			clear(n->children[i]);
//		}
//		n->children.clear();
//	};
//
//	// Recursive helper for destructor
//	void destroy(node* n) {
//		if (n) {
//			for (int i = 0; i < n->children.size(); i++) {
//				destroy(n->children[i]);
//			}
//			delete n;
//		}
//	};
//
//	~curveTree() {
//		clear(root); 
//		delete root, prev, temp, curr;
//		index = 0; 
//		out.clear();
//	};
//
//	Curve* getCurr() {
//		return curr->nodeCurve;
//	}
//	node* getCurrNode() {
//		return curr;
//	}
//	void updateCurve(Curve* C) {
//		temp = curr->children.back();
//		temp->nodeCurve = C;
//		curr->children.back() = temp;
//	}
//
//	bool empty() {
//		return root->children.empty();
//	}
//
//	// Move "down" with push
//	void pushChild(Curve* C) {
//		prev = curr;
//		temp = new node();
//		temp->nodeCurve = C;
//		temp->depth = prev->depth + 1;
//		prev->children.push_back(temp);
//		curr = prev->children.back();
//		curr->parent = prev;
//		curr->childIndex = prev->children.size() - 1;
//	};
//	// Undo curve
//	void popChild() {};
//	// Move "Up"
//	void navUp() {
//		if (curr == root) {
//			std::cout << "curr==root" << std::endl;
//			return;
//		}
//		curr = prev;
//		prev = curr->parent;
//		index = curr->childIndex;
//	};
//	// Move "Down" no push
//	void navDown() {
//		index = 0;
//		if (curr->children.size() == 0) {
//			return;
//		}
//		prev = curr;
//		curr = curr->children[index];
//	};
//	// Move "Left"
//	void navLeft() {
//		index--;
//		if (index < 0) index = prev->children.size() - 1;
//		curr = prev->children[index];
//	};
//	// Move "Right"
//	void navRight() {
//		index++;
//		if (index >= prev->children.size()) index = 0;
//		curr = prev->children[index];
//	};
//
//	// Write serialized tree ("end-of-children" method)
//	void writeTree(node* a_parent) {
//		if (a_parent->children.size() == 0) {
//			std::cout << "empty tree!" << std::endl;
//			std::cout << "pushing spacer..." << std::endl;
//			out.push_back(glm::vec3(10));
//			return;
//		}
//
//		std::cout << "recursion..." << std::endl;
//		for (int i = 0; i < a_parent->children.size(); i++) {
//			curr = a_parent->children[i];
//			for (glm::vec3 controlPoint : curr->nodeCurve->GetControlPoints()) {
//				out.push_back(controlPoint);
//			}
//			writeTree(curr);
//		}
//
//		std::cout << "pushing spacer..." << std::endl;
//		out.push_back(glm::vec3(10));
//		std::cout << "node written!" << std::endl;
//		return;
//	}
//
//	// Start serialization
//	std::vector<glm::vec3> serialize() {
//		//std::cout << "begin serialize..." << std::endl;
//		writeTree(root);
//		//std::cout << "end serialize!" << std::endl;
//		return out;
//	}
//};