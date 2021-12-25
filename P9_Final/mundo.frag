 //[FRAGMENT SHADER]
  #version 330 core
  uniform samplerCube Texture0;
  varying vec3 TexCoord0;
  void main()
  {
     vec4 texel = textureCube(Texture0, TexCoord0);
     gl_FragColor = texel;
  }