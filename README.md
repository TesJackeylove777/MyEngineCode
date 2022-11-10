# MyEngineCode
The source code of my game engine
## How to check the source code
In the Engine file, you could check the frame of whole engine.  

In the simple_library file, you could check the C-style library that I used.  

In the shader file, you could check the simple shader file(At present I combined VertexShader and PixelShader together into Hi.glsl)  
  

## I build a frame for a 3D game engine. So far, I have completed the following contents:

Log System and error system:  Helping users to assert in somewhere and users could check the error messages.  

**Command List**  

**Command Queue**  

**Command Allocator**  

**Resource Descriptor**  

**Swap Chain**: Back buffer and front buffer, which prevent the image from tearing  

**Fence**: It makes CPU wait for the GPU, although not efficient but can let the render pipeline's commands run in sequence)  

**Anti-Aliasing(MSAA)**  

**Initialization of the display window**: Including canvas length, width, display dimension, display quality, etc  

**Initialization of camera system**  

**Creating GUID of objects**    

**Simple Shader**  

**Mesh System**: Including the creation of cuboid, cylinder, sphere, cone, plane and how to import custom model (obj format)  

**And a great number of engineering parts and details**  

