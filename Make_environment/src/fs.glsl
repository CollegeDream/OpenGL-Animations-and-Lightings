#version 450 core                                                 

in vec4 vs_color;
in vec2 vs_uv;   

uniform sampler2D twoDTex; 

out vec4 color;  
                                                                  
void main(void)                                                   
{                     
   // color = texture(twoDTex, vs_uv * vec2(1.0,1.0));//Texture interpolation                            
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
} 
