# Render-Engine

![](https://raw.githubusercontent.com/TristanSmeets/Render-Engine/develop/Images/thumb.jpg)
Originally a part of a university semester where I focussed on learning more about C++ and OpenGL.
I'm currently working on refactoring the render engine into a game engine.

Currently plans:
- ~~Refactoring Shader class.~~
- Refactor post processing shaders into 1 shader.
- Rework mesh loading.
- Rework texture loading.
- More rendering clean up.
- Add ECS structure.

### Demo Video
This is a demo video of the render engine at the end of the university semester.
[Youtube](https://youtu.be/ufwLdmk4k64)

### Basic Information

**Languages:**
C++, GLSL

**Dependencies**
- [GLFW 3.3](https://www.glfw.org/)
- [Assimp 5.0](https://www.assimp.org/)
- [stb image](https://github.com/nothings/stb/blob/master/stb_image.h)
- [GLM 0.9.9](https://glm.g-truc.net/0.9.9/index.html)

**Features**
These feature were in at the end of the original university assignment.
- Lambertian reflectance
- Cook-Torrance specular lighting
- Image based lighting
- Point lights with shadows
- PBR Materials
- Screen Space Ambient Occlussion
- FXAA
- Depth Of field
- Bloom
