/* Texture map fragment shader */

//Sampler variable
uniform sampler2D textureMap;

void main()
{
	//Sample texture
	vec4 texColor = texture2D(textureMap, gl_TexCoord[0].st);

	//Apply texture map using replacement
    gl_FragColor = texColor * gl_Color;
    //gl_FragColor = gl_Color;
}