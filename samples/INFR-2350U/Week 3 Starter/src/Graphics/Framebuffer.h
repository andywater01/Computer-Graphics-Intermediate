#pragma once
#include <vector>
#include <Texture2D.h>
#include <Shader.h>

struct DepthTarget
{
	//Deconstructor for Depth Target
	//*Unloads texture
	~DepthTarget();
	//Deletes the texture of the depth target
	void Unload();
	//Holds the depth texture
	Texture2D _texture;
};

struct ColorTarget
{
	//Deconstructor for Color Target
	//*Unloads all the color targets
	~ColorTarget();
	//Deletes the texture of the colour targets
	void Unload();
	//Holds the colour texture
	std::vector<Texture2D> _textures;
	std::vector<GLenum> _formats;
	std::vector<GLenum> _buffers;
	//Stores the number of colour attachments the target has
	unsigned int _numAttachments = 0;
};

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	//unloads the framebuffer
	void Unload();

	//Initalize framebuffer with size
	virtual void Init(unsigned width, unsigned height);

	//initialized framebuffer
	void Init();

	//Adds depth target
	// ** ONLY ONE ** //
	void AddDepthTarget();

	//Adds a colour target
	// ** You can have as many as you want ** //
	void AddColorTarget(GLenum format);

	//Binds our depth buffer as a texture to specified slot
	void BindDepthAsTexture(int textureSlot) const;
	//Binds our color buffer as a texture
	void BindColorAsTexture(unsigned colorBuffer, int textureSlot) const;
	//Unbinds the texture from specified slot
	void UnbindTexture(int textureSlot) const;

	//Resized the framebuffer
	void Reshape(unsigned width, unsigned height);
	//Sets the size of the framebuffer
	void SetSize(unsigned width, unsigned height);

	//Sets  the viewport to fullscreen (using the size of framebuffer)
	void SetViewPort() const;

	//Binds the framebuffer
	void Bind() const;
	//Unbinds the framebuffer
	void UnBind() const;

	//Renders the framebuffer to our FullScreenQuad
	void RenderToFSQ() const;

	//Draws the contents of the framebuffer to the back buffer
	void DrawToBackBuffer();

	//clears the framebuffer useing clear flag
	void Clear();

	bool CheckFBO();

	//Initialize fullscreen quad
	//*Creates VAO for full screen quad
	//*covers -1 to 1 range
	static void InitFullscreenQuad();
	//Draws our fullscreen quad
	static void DrawFullscreenQuad();

	//Initial width and height is zero
	unsigned int _width = 0;
	unsigned int _height = 0;

protected:
	//OpenGL framebuffer handle
	GLuint _FBO;
	//Depth attachment (either one or none)
	DepthTarget _depth;
	//Color attachments (can be either 1 or more)
	ColorTarget _color;

	//Default filter is GL_NEAREST
	GLenum _filter = GL_NEAREST;
	//Default filer is GL_TO_EDGE
	GLenum _wrap = GL_CLAMP_TO_EDGE;

	GLbitfield _clearFlag = 0;

	//is the framebuffer initialized 
	bool _isInit = false;

	//depth attachment?
	bool _depthActive = false;

	//Full screen quad VBO handle
	static GLuint _fullscreenQuadVBO;
	//fullscreen quad VAO handle
	static GLuint _fullscreenQuadVAO;

	//the max amount of colour attachments
	static int _maxColorAttachments;
	//is the fullscreen quad initialized
	static bool _isInitFSQ;
};