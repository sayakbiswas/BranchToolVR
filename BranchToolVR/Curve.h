#pragma once
#include "AbstractBaseObject.h"
#include "fstream"

class DicomPointCloudObject;

class Curve :	public AbstractBaseObject
{

private:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> controlPoints;
	std::vector<glm::vec3> instanced_colors = { glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 0.0, 0.0) };

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

class curveTree {

	struct node {
		Curve* nodeCurve;
		node* parent;
		std::vector<node*> children;
		int depth;
		int childIndex;
	};

	node* root = new node();
	node* prev;
	node* temp;
	node* curr;
	int index = 0;
	std::vector<Curve*> out;
	std::string output;
	std::string marker = "# End of Children #\n";

public:

	curveTree() {
		// Junk values for super root
		root->nodeCurve = new Curve();
		root->depth = -1;
		root->childIndex = -1;
		curr = root;
		prev = new node();
		temp = new node();
	};

	// Recursive helper for destructor
	void destroy(node* n) {
		if (n) {
			for (int i = 0; i < n->children.size(); i++) {
				destroy(n->children[i]);
			}
			delete n;
		}
	};

	~curveTree() { delete root, prev, temp, curr; };

	Curve* getCurr() {
		return curr->nodeCurve;
	}
	node* getCurrNode() {
		return curr;
	}
	void updateCurve(Curve* C) {
		temp = curr->children.back();
		temp->nodeCurve = C;
		curr->children.back() = temp;
	}

	bool empty() {
		return root->children.empty();
	}

	// Move "down"
	void pushChild(Curve* C) {
		prev = curr;
		temp->nodeCurve = C;
		temp->depth = prev->depth + 1;
		prev->children.push_back(temp);
		curr = prev->children.back();
		curr->parent = prev;
		curr->childIndex = prev->children.size() - 1;
	};
	// Undo curve
	void popChild() {};
	// Move "Up"
	void navUp() {
		if (curr == root) {
			std::cout << "curr==root" << std::endl;
			return;
		}
		curr = prev;
		prev = curr->parent;
		index = curr->childIndex;
	};
	// Move "Down"
	void navDown() {
		index = 0;
		if (!curr->children[index]) {
			return;
		}
		prev = curr;
		curr = curr->children[index];
	};
	// Move "Left"
	void navLeft() {
		index--;
		if (index < 0) index = prev->children.size() - 1;
		curr = prev->children[index];
	};
	// Move "Right"
	void navRight() {
		index++;
		if (index >= prev->children.size()) index = 0;
		curr = prev->children[index];
	};

	// Write serialized tree ("end-of-children" method)
	void writeTree(node* _parent) {
		if (_parent->children.size() == 0)
			return;

		for (int i = 0; i < _parent->children.size(); i++) {
			curr = _parent->children[i];
			for(glm::vec3 controlPoint : curr->nodeCurve->GetControlPoints()){
				output += (std::to_string(controlPoint.x)
					+ " " + std::to_string(controlPoint.y)
					+ " " + std::to_string(-1.0 * controlPoint.z) + "\n");
			}
			writeTree(curr);
		}
		output += marker;
		return;
	}

	// Start serialization
	std::string serialize() {
		writeTree(root);
		return output;
	}

	// Level-order, still have to  do marking stuff
	void levelOrder(node * _parent) {
		for (int i = 0; i < _parent->children.size(); i++) {
			curr = _parent->children[i];
			out.push_back(curr->nodeCurve);
		}
		for (int i = 0;  i < _parent->children.size(); i++)
			levelOrder(_parent->children[i]);
	};

	// Kick-start traversal
	std::vector<Curve*> traverse() {
		levelOrder(root);
		return out;
	};
};