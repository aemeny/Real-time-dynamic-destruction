// Vertex Shader
uniform mat4 u_Model;
uniform mat4 u_Viewing;
uniform mat4 u_Projection;

attribute vec3 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord;

varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec2 v_TexCoord;

void main() {
    gl_Position = u_Projection * u_Viewing * u_Model * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_Normal = normalize(mat3(u_Model) * a_Normal);
    v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
}                             
                                       