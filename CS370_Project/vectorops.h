#include <math.h>

// Utility function for cross product
void cross(GLfloat P[], GLfloat Q[], GLfloat R[], GLfloat n[])
{
	GLfloat a[3],b[3];

	// Compute vectors from given points
	for (int i=0; i<3; i++)
	{
		a[i] = Q[i]-P[i];
		b[i] = R[i]-P[i];
	}

	// Compute cross product
	n[0] = a[1]*b[2] - a[2]*b[1];
	n[1] = a[2]*b[0] - a[0]*b[2];
	n[2] = a[0]*b[1] - a[1]*b[0];
}

// Utility function to normalize a vector
void normalize(GLfloat a[])
{
	GLfloat length = 0.0f;

	// Compute length
	for (int i=0; i<3; i++)
	{
		length += a[i]*a[i];
	}
	length = sqrt(length);

	// Normalize components
	if (length > 0.0)
	{
		for (int i=0; i<3; i++)
		{
			a[i] = a[i]/length;
		}
	}
}

// Utility function to copy a vector
void copy_vector(GLfloat *src, GLfloat *dest, int n=3)
{
	for (int i=0; i<3; i++)
	{
		dest[i] = src[i];
	}
}