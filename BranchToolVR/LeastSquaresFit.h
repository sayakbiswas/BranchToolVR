#pragma once

#include <vector>
#include <glm\glm.hpp>

struct BranchPoint;
class LeastSquaresFit
{
private:
	std::vector<BranchPoint*> branchPoints;
	int numberOfPieces = 1;
	/** Parameters for the basis functions (t)*/
	std::vector<float>params;
	glm::mat2 unknownBasisMatrix;
	glm::mat2 knownBasisMatrix;
	/** Approximations for the two unknown data points */
	std::vector<glm::vec3> fittedControlPoints;
	std::vector<glm::vec3> curvePoints;

	/** Sum of actual data points in the equation multiplied with the basis */
	glm::vec2 actualDataBasisX;
	glm::vec2 actualDataBasisY;
	glm::vec2 actualDataBasisZ;

public:
	LeastSquaresFit();
	LeastSquaresFit(std::vector<BranchPoint*> branchPoints);
	LeastSquaresFit(std::vector<BranchPoint*> branchPoints, int numberOfPieces);
	~LeastSquaresFit();

	void CalculateParameters(int start, int end);
	void ComputeBasis(int start, int end);
	void FitCurve(int start, int end);
	/** Render the  fitted curve */
	void Fit();
	std::vector<glm::vec3> GetCurvePoints();
	std::vector<glm::vec3> GetFittedControlPoints();
};

