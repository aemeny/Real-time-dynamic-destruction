// Fragment Shader
uniform sampler2D u_Texture;
uniform vec3 u_ViewPos;

varying vec2 v_TexCoord;
varying vec3 v_Normal;
varying vec3 v_FragPos;

void main() {
   vec3 lightPos = vec3(20, 100, 20);

   vec3 ambientColor = vec3(0.2, 0.2, 0.3);
   vec3 diffuseColor = vec3(1.0, 0.9, 0.8);
   vec3 specularColor = vec3(1.0, 0.95, 0.95);
   
   vec3 norm = normalize(v_Normal);
   vec3 lightDir = normalize(lightPos - v_FragPos);
   
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diffuseColor * diff;
   
   // Fill light
   vec3 fillLightDir = normalize(vec3(-lightPos.x, lightPos.y, -lightPos.z));
   float fillDiff = max(dot(norm, fillLightDir), 0.0) * 0.2;
   vec3 fill = diffuseColor * fillDiff * 0.5;
   
   // Specular
   vec3 viewDir = normalize(u_ViewPos - v_FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
   vec3 specular = specularColor * spec;
   
   // Rim (Fresnel) Light
   float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
   vec3 rimColor = vec3(1.0, 0.8, 0.6) * fresnel * 0.3;
   
   // Dynamic ambient (optional)
   float facing = max(dot(norm, vec3(0,1,0)), 1.0);
   vec3 dynamicAmbient = ambientColor * (0.3 + 0.7 * facing);
   
   // Texture sampling
   vec4 texColor = texture2D(u_Texture, v_TexCoord);
   
   // Final lighting
   vec3 lighting = dynamicAmbient + diffuse + fill + specular + rimColor;

   gl_FragColor = vec4(lighting, 1.0) * texColor;
   // Gamma correction
   gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0/0.95));
}