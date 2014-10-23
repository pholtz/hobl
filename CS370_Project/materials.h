typedef struct MaterialType {
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} MaterialType;

// Material properties for brass
MaterialType brass = {  { 0.33f, 0.22f, 0.03f, 1.0f },
						{ 0.78f, 0.57f, 0.11f, 1.0f },
						{ 0.99f, 0.91f, 0.81f, 1.0f },
											  27.8f };

// Material properties for glass
MaterialType glass = { 
	{ 0.0f, 0.0f, 0.0f, 0.15f }, 
	{ 0.55f, 0.55f, 0.55f, 0.15f }, 
	{ 0.70f, 0.70f, 0.70f, 0.15f },
	100.0f
};

//Taken from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
MaterialType emerald = {
	{ 0.0215f, 0.1745f, 0.0215f, 1.0f },
	{ 0.07568f, 0.61424f, 0.07568f, 1.0f },
	{ 0.433f, 0.527811f, 0.433f, 1.0f },
	0.1 };

//Taken from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
MaterialType yellow_rubber = {
	{ 0.05f, 0.05f, 0.0f, 1.0f },
	{ 0.5f, 0.5f, 0.4f, 1.0f },
	{ 0.7f, 0.7f, 0.04f, 1.0f },
	10.0f
};

//Taken from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
MaterialType white_rubber = {
	{ 0.05f, 0.05f, 0.05f, 1.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f },
	{ 0.7f, 0.7f, 0.7f, 1.0f },
	10.0f
};

//Taken from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
MaterialType chrome = {
	{ 0.25f, 0.25f, 0.25f, 1.0f },
	{ 0.4f, 0.4f, 0.4f, 1.0f },
	{ 0.774597f, 0.774597f, 0.774597f, 1.0f },
	76.8f
};

//Taken from: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
MaterialType cyan_rubber = {
	{ 0.0f, 0.05f, 0.05f, 1.0f },
	{ 0.4f, 0.5f, 0.5f, 1.0f },
	{ 0.04f, 0.7f, 0.7f, 1.0f },
	10.0f
};

//Created using Mated
MaterialType faux_wood = {
	{ 0.24f, 0.14f, 0.07f, 1.0f },
	{ 0.28f, 0.16f, 0.1f, 1.0f },
	{ 0.47f, 0.29f, 0.0f, 1.0f },
	7.0f
};

MaterialType black_plastic = {
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.01f, 0.01f, 0.01, 1.0f },
	{ 0.50f, 0.50f, 0.50, 1.0f },
	32.0f
};
						
void set_material(GLenum face, MaterialType *material)
{
	glMaterialfv(face,GL_AMBIENT,material->ambient);
	glMaterialfv(face,GL_DIFFUSE,material->diffuse);
	glMaterialfv(face,GL_SPECULAR,material->specular);
	glMaterialf(face,GL_SHININESS,material->shininess);
}
