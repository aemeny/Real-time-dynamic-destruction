attribute vec2 a_Position;
attribute vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_Projection;

varying vec2 v_TexCoord;

void main()
{
  gl_Position = u_Projection * u_Model * vec4(a_Position, 0, 1);
	
  v_TexCoord = a_TexCoord;
}

