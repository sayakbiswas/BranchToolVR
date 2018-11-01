#include "Curve.h"

struct node {
	Curve* nodeCurve;
	node* parent;
	std::vector<node*> children;
	int depth;
	int childIndex;
};

class curveTree {

	node* root;
	node* prev;
	node* temp;
	node* curr;
	int index = 0;
	std::vector<glm::vec3> out;

public:

	curveTree();
	~curveTree();

	Curve* getCurr();
	node* getCurrNode();

	void clear(node* n);
	void destroy(node* n);

	void updateCurve(Curve* C);

	bool empty();

	void pushChild(Curve* C);
	void popChild();

	void navUp();
	void navDown();
	void navLeft();
	void navRight();

	void writeTree(node* a_parent);
	std::vector<glm::vec3> serialize();
};