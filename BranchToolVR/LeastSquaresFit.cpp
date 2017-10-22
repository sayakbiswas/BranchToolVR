#include "LeastSquaresFit.h"
#include "DicomPointCloudObject.h"

LeastSquaresFit::LeastSquaresFit()
{
}

LeastSquaresFit::LeastSquaresFit(std::vector<BranchPoint*> branchPoints)
{
	this->branchPoints = branchPoints;
}

LeastSquaresFit::LeastSquaresFit(std::vector<BranchPoint*> branchPoints, int numberOfPieces)
{
	this->branchPoints = branchPoints;
	this->numberOfPieces = numberOfPieces;
}

LeastSquaresFit::~LeastSquaresFit()
{
}

void LeastSquaresFit::CalculateParameters()
{
	std::vector<float> segmentLengths;
	float totalLength = 0.f;

	// TODO: Neighbors are present in the data structure, not sure if they are populated, need to check
	if (branchPoints.size() > 1) 
	{
		for (int i = 1; i < branchPoints.size(); i++) 
		{
			glm::vec3 position1 = branchPoints[i - 1]->position;
			glm::vec3 position2 = branchPoints[i]->position;

			//TODO: Optimize - this does not need to be done for every point every time
			float segmentLength = glm::length(position2 - position1);

			segmentLengths.push_back(segmentLength);
			
			totalLength += segmentLength;
		}

		params.push_back(0.f);
		for (int i = 0; i < segmentLengths.size(); i++) 
		{
			params.push_back(params[i] + (segmentLengths[i] / totalLength));
		}
	}
}

void LeastSquaresFit::ComputeBasis()
{
	int size = params.size();
	float B01 = 0.f, B02 = 0.f, B11 = 0.f, B12 = 0.f, B21 = 0.f, B22 = 0.f, B31 = 0.f, B32 = 0.f;
	float b0 = 0.f, b1 = 0.f, b2 = 0.f, b3 = 0.f;
	float Yx1 = 0.f, Yx2 = 0.f, Yy1 = 0.f, Yy2 = 0.f, Yz1 = 0.f, Yz2 = 0.f;
	for (int i = 0; i < size; i++)
	{
		b0 = (1 - params[i]) * (1 - params[i]) * (1 - params[i]);
		b1 = 3 * (1 - params[i]) * (1 - params[i]) * params[i];
		b2 = 3 * (1 - params[i]) * params[i] * params[i];
		b3 = params[i] * params[i] * params[i];

		B01 += b0 * b1;
		B02 += b0 * b2;
		B11 += b1 * b1;
		B12 += b1 * b2;
		B21 += b2 * b1;
		B22 += b2 * b2;
		B31 += b3 * b1;
		B32 += b3 * b2;

		Yx1 += branchPoints[i]->position.x * b1;
		Yx2 += branchPoints[i]->position.x * b2;
		Yy1 += branchPoints[i]->position.y * b1;
		Yy2 += branchPoints[i]->position.y * b2;
		Yz1 += branchPoints[i]->position.z * b1;
		Yz2 += branchPoints[i]->position.z * b2;
	}

	unknownBasisMatrix[0][0] = B11;
	unknownBasisMatrix[0][1] = B12;
	unknownBasisMatrix[1][0] = B21;
	unknownBasisMatrix[1][1] = B22;

	knownBasisMatrix[0][0] = B01;
	knownBasisMatrix[0][1] = B02;
	knownBasisMatrix[1][0] = B31;
	knownBasisMatrix[1][1] = B32;

	actualDataBasisX[0] = Yx1;
	actualDataBasisX[1] = Yx2;
	actualDataBasisY[0] = Yy1;
	actualDataBasisY[1] = Yy2;
	actualDataBasisZ[0] = Yz1;
	actualDataBasisZ[1] = Yz2;
}

void LeastSquaresFit::FitCurve()
{
	int start = 0;
	int end = branchPoints.size() - 1;

	glm::mat2 inverseUnknownBasisMatrix = glm::inverse(unknownBasisMatrix);
	
	glm::vec2 knownPointsX = glm::vec2(branchPoints[start]->position.x, branchPoints[end]->position.x);
	glm::vec2 knownPointsY = glm::vec2(branchPoints[start]->position.y, branchPoints[end]->position.y);
	glm::vec2 knownPointsZ = glm::vec2(branchPoints[start]->position.z, branchPoints[end]->position.z);

	glm::vec2 fittedCoefficientsX = -(inverseUnknownBasisMatrix * knownBasisMatrix * knownPointsX)
		+ inverseUnknownBasisMatrix * actualDataBasisX;
	glm::vec2 fittedCoefficientsY = -(inverseUnknownBasisMatrix * knownBasisMatrix * knownPointsY)
		+ inverseUnknownBasisMatrix * actualDataBasisY;
	glm::vec2 fittedCoefficientsZ = -(inverseUnknownBasisMatrix * knownBasisMatrix * knownPointsZ)
		+ inverseUnknownBasisMatrix * actualDataBasisZ;

	fittedCoefficients.push_back(glm::vec3(fittedCoefficientsX[0], fittedCoefficientsY[0], fittedCoefficientsZ[0]));
	fittedCoefficients.push_back(glm::vec3(fittedCoefficientsX[1], fittedCoefficientsY[1], fittedCoefficientsZ[1]));

	int size = params.size();
	float b0, b1, b2, b3;

	for (float t = 0.f; t <= 1.f; t += 0.01f) 
	{
		b0 = (1 - t) * (1 - t) * (1 - t);
		b1 = 3 * (1 - t) * (1 - t) * t;
		b2 = 3 * (1 - t) * t * t;
		b3 = t * t * t;

		float curvePointsX = knownPointsX[0] * b0
			+ fittedCoefficientsX[0] * b1
			+ fittedCoefficientsX[1] * b2
			+ knownPointsX[1] * b3;

		float curvePointsY = knownPointsY[0] * b0
			+ fittedCoefficientsY[0] * b1
			+ fittedCoefficientsY[1] * b2
			+ knownPointsY[1] * b3;

		float curvePointsZ = knownPointsZ[0] * b0
			+ fittedCoefficientsZ[0] * b1
			+ fittedCoefficientsZ[1] * b2
			+ knownPointsZ[1] * b3;

		curvePoints.push_back(glm::vec3(curvePointsX, curvePointsY, curvePointsZ));
	}
}

void LeastSquaresFit::Fit()
{
	CalculateParameters();
	ComputeBasis();
	FitCurve();
}

std::vector<glm::vec3> LeastSquaresFit::GetCurvePoints()
{
	return curvePoints;
}

