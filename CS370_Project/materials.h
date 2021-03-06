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
	{ 0.25f, 0.25f, 0.25f, 0.075f }, 
	{ 0.65f, 0.65f, 0.65f, 0.075f }, 
	{ 0.75f, 0.75f, 0.75f, 0.075f },
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
	{ 0.1f, 0.1f, 0.1f, 1.0f },
	{ 0.25f, 0.25f, 0.25f, 1.0f },
	{ 0.574597f, 0.574597f, 0.574597f, 1.0f },
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
	50.0f
};

//Created using Mated
MaterialType light_wood = {
	{ 0.44f, 0.34f, 0.2f, 1.0f },
	{ 0.48f, 0.36f, 0.25f, 1.0f },
	{ 0.67f, 0.49f, 0.0f, 1.0f },
	10.0f
};

//Created using Mated
MaterialType dark_wood = {
	{ 0.1f, 0.07f, 0.035f, 1.0f },
	{ 0.125f, 0.08f, 0.05f, 1.0f },
	{ 0.25f, 0.15f, 0.0f, 1.0f },
	0.0f
};

MaterialType black_plastic = {
	{ 0.0f, 0.0f, 0.0f, 1.0f },
	{ 0.01f, 0.01f, 0.01, 1.0f },
	{ 0.0f, 0.0f, 0.0, 1.0f },
	0.5f
};

MaterialType red_rubber = {
	{ 0.05f, 0.0f, 0.0f, 1.0f },
	{ 0.5f, 0.4f, 0.4f, 1.0f },
	{ 0.7f, 0.04f, 0.04f, 1.0f },
	10.0f
};

MaterialType clear = {
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.1f, 0.1f, 0.1f, 1.0f },
	{ 0.1f, 0.1f, 0.1f, 1.0f },
	20.0f
};

MaterialType white = {
	{ 0.5f, 0.5f, 0.5f, 1.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f },
	{ 0.5f, 0.5f, 0.5f, 1.0f },
	50.0f
};

MaterialType light_yellow = {
	{ 0.5f, 0.5f, 0.35f, 1.0f },
	{ 0.5f, 0.5f, 0.35f, 1.0f },
	{ 0.5f, 0.5f, 0.35f, 1.0f },
	20.0f
};

MaterialType obsidian = {
	{ 0.05375f, 0.05f, 0.06625f, 1.0f },
	{ 0.18275f, 0.17f, 0.22525f, 1.0f },
	{ 0.332741f, 0.328634f, 0.346435f, 1.0f },
	10.0f
};
						
void set_material(GLenum face, MaterialType *material)
{
	glMaterialfv(face,GL_AMBIENT,material->ambient);
	glMaterialfv(face,GL_DIFFUSE,material->diffuse);
	glMaterialfv(face,GL_SPECULAR,material->specular);
	glMaterialf(face,GL_SHININESS,material->shininess);
}
