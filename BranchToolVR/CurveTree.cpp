#include "CurveTree.h"

curveTree::curveTree() {
	// Junk values for super root
	root = new node();
	clear(root);
	root->nodeCurve = new Curve();
	root->depth = -1;
	root->childIndex = -1;
	curr = root;
	prev = new node();
	temp = new node();
	//std::cout << curr->children.size() << std::endl;
};
curveTree::~curveTree() {
	clear(root);
	delete root, prev, temp, curr;
	index = 0;
	out.clear();
};

Curve* curveTree::getCurr() {
	return curr->nodeCurve;
};
node* curveTree::getCurrNode() {
	return curr;
};

void curveTree::clear(node* n) {
	for (int i = 0; i < n->children.size(); i++) {
		clear(n->children[i]);
	}
	n->children.clear();
};
// Recursive helper for destructor
void curveTree::destroy(node* n) {
	if (n) {
		for (int i = 0; i < n->children.size(); i++) {
			destroy(n->children[i]);
		}
		delete n;
	}
};

void curveTree::updateCurve(Curve* C) {
	temp = curr->children.back();
	temp->nodeCurve = C;
	curr->children.back() = temp;
}

bool curveTree::empty() {
	return root->children.empty();
}

// Move "down" with push
void curveTree::pushChild(Curve* C) {
	prev = curr;
	temp = new node();
	temp->nodeCurve = C;
	temp->depth = prev->depth + 1;
	prev->children.push_back(temp);
	curr = prev->children.back();
	curr->parent = prev;
	curr->childIndex = prev->children.size() - 1;
};
// Undo curve
void curveTree::popChild() {
	if (curr == root) {
		std::cout << "curr==root" << std::endl;
		return;
	}
	temp = curr;
	curr = prev;
	prev = curr->parent;
	index = curr->childIndex;
	delete temp;
};

// Move "Up"
void curveTree::navUp() {
	if (curr == root) {
		std::cout << "curr==root" << std::endl;
		return;
	}
	curr = prev;
	prev = curr->parent;
	index = curr->childIndex;
};
// Move "Down" no push
void curveTree::navDown() {
	index = 0;
	if (curr->children.size() == 0) {
		return;
	}
	prev = curr;
	curr = curr->children[index];
};
// Move "Left"
void curveTree::navLeft() {
	if (curr == root) {
		return;
	}
	index--;
	if (index < 0) index = prev->children.size() - 1;
	curr = prev->children[index];
};
// Move "Right"
void curveTree::navRight() {
	if (curr == root) {
		return;
	}
	index++;
	if (index >= prev->children.size()) index = 0;
	curr = prev->children[index];
};

// Write serialized tree ("end-of-children" method)
void curveTree::writeTree(node* a_parent) {
	if (a_parent->children.size() == 0) {
		std::cout << "empty tree!" << std::endl;
		std::cout << "pushing spacer..." << std::endl;
		out.push_back(glm::vec3(10));
		return;
	}

	std::cout << "recursion..." << std::endl;
	for (int i = 0; i < a_parent->children.size(); i++) {
		curr = a_parent->children[i];
		for (glm::vec3 controlPoint : curr->nodeCurve->GetControlPoints()) {
			out.push_back(controlPoint);
		}
		writeTree(curr);
	}

	std::cout << "pushing spacer..." << std::endl;
	out.push_back(glm::vec3(10));
	std::cout << "node written!" << std::endl;
	return;
}
// Start serialization
std::vector<glm::vec3> curveTree::serialize() {
	//std::cout << "begin serialize..." << std::endl;
	writeTree(root);
	//std::cout << "end serialize!" << std::endl;
	return out;
}