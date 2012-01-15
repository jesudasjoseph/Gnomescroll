
uniform mat4 InRotationMatrix;

//uniform vec3 InTranslation;
//attribute vec3 InCood0;
//attribute vec4 InRGBA;
//varying vec4 _rgba;

attribute vec3 InNormal;

//varying float 

//varying vec3 normal;

varying float diffuse;

//varying vec3 lcolor;

void main(void) 
{

	gl_FrontColor = gl_Color;
	vec4 pos = InRotationMatrix*gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * pos;

	vec3 normal = gl_NormalMatrix*InNormal;
	const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

	diffuse = abs(dot(normal, light_dir ));

/*
	gl_FrontColor = gl_Color;

	vec4 pos = InRotationMatrix*gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * pos;

	//normal = gl_NormalMatrix*InNormal;
	normal = InNormal;

	//vec3 test = normalize(vec3(0.56, 0.68, 0.23));
 
	//diffuse = dot(InNormal, test );

	//gl_Position = gl_ModelViewProjectionMatrix * InCood0;
	//_rgba = InRGBA;
*/

/*
	vec3 normal, lightDir;
	
	//vec4 diffuse, ambient, globalAmbient;
	float diffuse, ambient, globalAmbient;
	float NdotL;

	float material_diffuse = 0.50;
	float light_diffuse = 0.70;

	normal = normalize(gl_NormalMatrix * InNormal );
	lightDir = normalize(gl_NormalMatrix*vec3(0.56, 0.68, 0.23));

	NdotL = max(dot(normal, lightDir), 0.0);
	diffuse = material_diffuse * light_diffuse;

	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	lcolor =  NdotL * diffuse + globalAmbient + ambient;
*/
}
