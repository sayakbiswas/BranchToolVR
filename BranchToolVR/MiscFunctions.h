#pragma once

#include <stdio.h> 
#include <stdarg.h> 
#include <string>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <pathtools.h>

#include "Constants.h"

namespace MiscFunctions 
{
	template <typename T>
	std::string to_string_with_precision(const T a_value, const int n)
	{
		std::ostringstream out;
		out << std::setprecision(n) << a_value;
		return out.str();
	}

	static inline void ray_triangle_intersection(const float *v1, const float *v2, const float *v3, const float *p, const float *l, float *cg) 
	{
		cg[0] = (v2[1] * l[2] * p[0] + l[1] * v2[0] * p[2] - v2[2] *
		l[1] * p[0] - l[2] * v2[0] * p[1] - l[0] * v2[1] * p[2] + l[0] *
		v2[2] * p[1] - v2[1] * l[2] * v3[0] - l[1] * v2[0] * v3[2] +
		v2[2] * l[1] * v3[0] + l[2] * v2[0] * v3[1] + l[0] * v2[1] * v3[2]
		- l[0] * v2[2] * v3[1] + l[1] * p[0] * v3[2] - v3[1] * l[2] * p[0]
		- l[1] * v3[0] * p[2] + p[1] * l[2] * v3[0] + l[0] * v3[1] * p[2]
		- l[0] * p[1] * v3[2]) / (v2[1] * l[2] * v1[0] - v2[1] * l[2] *
		v3[0] + l[1] * v2[0] * v1[2] + v1[1] * l[2] * v3[0] - l[2] * v2[0]
		* v1[1] + l[2] * v2[0] * v3[1] - l[2] * v1[0] * v3[1] - l[0] *
		v1[1] * v3[2] + l[0] * v1[2] * v3[1] - l[0] * v2[1] * v1[2] + l[0]
		* v2[1] * v3[2] - l[0] * v2[2] * v3[1] - l[1] * v2[0] * v3[2] +
		l[1] * v1[0] * v3[2] - v1[2] * l[1] * v3[0] + v2[2] * l[1] * v3[0]
		- v2[2] * l[1] * v1[0] + l[0] * v2[2] * v1[1]); cg[1] = -(l[0] *
		v1[1] * v3[2] - l[0] * v1[1] * p[2] - l[0] * v1[2] * v3[1] + l[0]
		* v3[1] * p[2] + l[0] * v1[2] * p[1] - l[0] * p[1] * v3[2] - v1[2]
		* l[1] * p[0] - l[1] * v3[0] * p[2] - v3[1] * l[2] * p[0] + v1[1]
		* l[2] * p[0] + l[1] * v1[0] * p[2] - v1[1] * l[2] * v3[0] + l[1]
		* p[0] * v3[2] - l[1] * v1[0] * v3[2] + l[2] * v1[0] * v3[1] - l[2
		] * v1[0] * p[1] + p[1] * l[2] * v3[0] + v1[2] * l[1] * v3[0]) /
		(v2[1] * l[2] * v1[0] - v2[1] * l[2] * v3[0] + l[1] * v2[0] *
		v1[2] + v1[1] * l[2] * v3[0] - l[2] * v2[0] * v1[1] + l[2] *
		v2[0] * v3[1] - l[2] * v1[0] * v3[1] - l[0] * v1[1] * v3[2] + l[0]
		* v1[2] * v3[1] - l[0] * v2[1] * v1[2] + l[0] * v2[1] * v3[2] -
		l[0] * v2[2] * v3[1] - l[1] * v2[0] * v3[2] + l[1] * v1[0] * v3[2]
		- v1[2] * l[1] * v3[0] + v2[2] * l[1] * v3[0] - v2[2] * l[1] *
		v1[0] + l[0] * v2[2] * v1[1]); cg[2] = (-l[2] * v2[0] * v1[1] +
		l[2] * v2[0] * p[1] - l[2] * v1[0] * p[1] + l[0] * v2[1] * p[2] +
		v1[1] * l[2] * p[0] + v2[1] * l[2] * v1[0] - v2[1] * l[2] * p[0] +
		l[1] * v2[0] * v1[2] - l[1] * v2[0] * p[2] + l[1] * v1[0] * p[2]
		- v1[2] * l[1] * p[0] + v2[2] * l[1] * p[0] - v2[2] * l[1] * v1[0]
		- l[0] * v2[1] * v1[2] - l[0] * v1[1] * p[2] + l[0] * v1[2] *
		p[1] + l[0] * v2[2] * v1[1] - l[0] * v2[2] * p[1]) / (v2[1] * l[2]
		* v1[0] - v2[1] * l[2] * v3[0] + l[1] * v2[0] * v1[2] + v1[1] *
		l[2] * v3[0] - l[2] * v2[0] * v1[1] + l[2] * v2[0] * v3[1] - l[2]
		* v1[0] * v3[1] - l[0] * v1[1] * v3[2] + l[0] * v1[2] * v3[1] -
		l[0] * v2[1] * v1[2] + l[0] * v2[1] * v3[2] - l[0] * v2[2] * v3[1]
		- l[1] * v2[0] * v3[2] + l[1] * v1[0] * v3[2] - v1[2] * l[1] *
		v3[0] + v2[2] * l[1] * v3[0] - v2[2] * l[1] * v1[0] + l[0] * v2[2]
		* v1[1]);
	}

	static inline void thread_sleep(unsigned long nMilliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(nMilliseconds));
	}

	static inline void dprintf(const char *fmt, ...)
	{
		va_list args;
		char buffer[2048];

		va_start(args, fmt);
		vsprintf_s(buffer, fmt, args);
		va_end(args);

		printf("%s", buffer);
	}

	static inline void println_vec3(glm::vec3 v)
	{
		std::cout << v.x << " " << v.y << " " << v.z << std::endl;
	}

	static inline void println_mat4(glm::mat4 m) 
	{
		std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl;
		std::cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl;
		std::cout << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl;
		std::cout << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl << std::endl;
	}

	static inline float XZplaneAngleY(glm::vec3 a)
	{
		glm::vec3 xz = glm::normalize(glm::vec3(a.x, 0.0f, a.z));

		float xzDotXaxis = acos(glm::dot(xz, Constants::X_AXIS));

		if (xz.x > 0 && xz.z < 0) 
		{
			// quadrant 0
			return xzDotXaxis;
		}
		else if (xz.x < 0 && xz.z < 0) 
		{
			// quad 1
			return xzDotXaxis;
		}
		else if (xz.x < 0 && xz.z > 0) 
		{
			// quad 2
			return TWOPI - xzDotXaxis;
		}
		else if (xz.x > 0 && xz.z > 0) 
		{
			// quad 3
			return TWOPI - xzDotXaxis;
		}
		else
		{
			// inbetween quadrants
			if (xz.x == 0.0f && xz.z < 0)
			{
				return xzDotXaxis;
			}
			else if (xz.x == 0.0f && xz.z > 0)
			{
				return TWOPI - xzDotXaxis;
			}
			else if (xz.z == 0.0f && xz.x < 0.0f)
			{
				return PI;
			}
			else if (xz.z == 0.0f && xz.x > 0.0f)
			{
				return 0.0f;
			}
		}
	}

	static inline float XZplaneAngleZ(glm::vec3 a, glm::vec3 b)
	{
		glm::vec3 z = glm::normalize(a);
		glm::vec3 zflat = glm::normalize(glm::vec3(a.x, 0.0f, a.z));
		float zDotXaxis = acos(glm::dot(z, zflat));
		float fac = 1.0f;

		if (b.x < 0.0f) 
		{
			fac = -1.0f;
		}

		if (z.y > 0.0f)
		{
			return fac*zDotXaxis;
		}
		else
		{
			return fac*-zDotXaxis;
		}
	}

	static inline glm::mat4 RotateTo(glm::vec3 a, glm::vec3 b) 
	{
		if (a == Constants::ZERO_VEC3 || b == Constants::ZERO_VEC3) 
		{
			return glm::mat4(1.0f);
		}

		return glm::rotate(glm::mat4(1.0f), XZplaneAngleY(b) - XZplaneAngleY(a), Constants::Y_AXIS) 
			*glm::rotate(glm::mat4(1.0f), XZplaneAngleZ(b, a) - XZplaneAngleZ(a, a), Constants::Z_AXIS) 
			*glm::scale(glm::mat4(1.0f), glm::vec3(glm::length(b) / glm::length(a)));
	}

	static inline std::string relativeToAbsolutePath(std::string _relative)
	{
		std::string sExecutableDirectory = Path_StripFilename(Path_GetExecutablePath());
		return Path_MakeAbsolute(_relative, sExecutableDirectory);
	}
};		
