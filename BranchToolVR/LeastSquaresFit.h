#pragma once

#include <vector>
#include <glm\glm.hpp>

struct BranchPoint;
class LeastSquaresFit
{
private:
	/** The branch points added by user */
	std::vector<BranchPoint*> branchPoints;

	/** Number of bezier pieces to fit */
	int numberOfPieces = 1;

	/** Parameters for the basis functions (t)*/
	std::vector<float>params;

	/** Basis matrix for the unknown points */
	glm::mat2 unknownBasisMatrix;

	/** Basis matrix for the known points */
	glm::mat2 knownBasisMatrix;

	/** Approximations for the two unknown data points */
	std::vector<glm::vec3> fittedControlPoints;

	/** Final points for the curve */
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

	/** Calculate parameters for the basis functions as the branch points are added */
	void CalculateParameters(int start, int end);

	/** Compute the elements for the basis matrices */
	void ComputeBasis(int start, int end);

	/** Compute the fitted coefficients and the curve points */
	void FitCurve(int start, int end);

	/** Render the  fitted curve */
	void Fit();

	/** Retrieve the curve points */
	std::vector<glm::vec3> GetCurvePoints();

	/** Retrieve the fitted coefficients for the curve */
	std::vector<glm::vec3> GetFittedControlPoints();
};

