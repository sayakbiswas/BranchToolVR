#pragma once

#include <vector>
#include <glm\glm.hpp>

struct BranchPoint;
class LeastSquaresFit
{
private:
	std::vector<BranchPoint*> branchPoints;
	
	/** Parameters for the basis functions (t)*/
	std::vector<float>params;

	/** Basis matrix for the unknown points */
	glm::mat2 unknownBasisMatrix;

	/** Basis matrix for the known points */
	glm::mat2 knownBasisMatrix;

	/** Basis values for the actual data points of the unknowns */
	std::vector<float> actualDataBasis;

	/** Coefficients for the two unknown data points */
	std::vector<glm::vec3> fittedCoefficients;

	/** Final points for the curve */
	std::vector<glm::vec3> curvePoints;

	/** Actual data in the equation */
	glm::vec2 actualDataX;
	glm::vec2 actualDataY;
	glm::vec2 actualDataZ;

public:
	LeastSquaresFit();
	LeastSquaresFit(std::vector<BranchPoint*> branchPoints);
	~LeastSquaresFit();

	/** Calculate parameters for the basis functions as the branch points are added */
	void CalculateParameters();

	/** Compute the elements for the basis matrices */
	void ComputeBasis();

	/** Compute the fitted coefficients and the curve points */
	void FitCurve();

	/** Render the  fitted curve */
	void Fit();

	/** Retrieve the curve points */
	std::vector<glm::vec3> GetCurvePoints();
};

