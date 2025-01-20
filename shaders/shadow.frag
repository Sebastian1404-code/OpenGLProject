#version 410 core
out vec4 fColor;
uniform sampler2D diffuseTexture; // The texture containing transparency (if applicable)
in vec2 fTextCoords;
void main()
{
	float alpha = texture(diffuseTexture, fTextCoords).a;
	if (alpha <= 0.5f)
        	discard;
	fColor = vec4(1.0f);
}
