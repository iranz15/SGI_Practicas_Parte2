  //[VERTEX SHADER]
  #version 330 core
  uniform mat4 ProjectionModelviewMatrix;
  varying vec3 TexCoord0;
  void main()
  {
     gl_Position = ProjectionModelviewMatrix * gl_Vertex;
     TexCoord0 = gl_Normal;
  }