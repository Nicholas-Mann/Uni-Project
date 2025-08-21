#pragma once

#include <glm/glm.hpp>
#include <math.h>
#include <iostream>

class MathsFunction
{
public:
	static inline double randomPos(double x, double y, double seed) {
		// This function returns a number between -1 , 1
		// It uses a sin and cos function to generate a random number based on the x and y position
		// The seed is used to change the random number

		return (
			sin((x * x * x * 12.9898 + y * y * y * 78.233 + seed * 545.7145) * 1455.546) +
			cos((x * x * x * 12.9898 + y * y * y * 78.233 + seed * 545.7145) * 5640.465)
			) / 2;
	}
};

