#include "Framebuffer.h"

GLuint Framebuffer::_fullscreenQuadVBO = 0;
GLuint Framebuffer::_fullscreenQuadVAO = 0;

int Framebuffer::_maxColorAttachments = 0;
bool Framebuffer::_isInitFSQ = false;

DepthTarget::~DepthTarget()
{
	//unloads the depth target
	Unload();
}

void DepthTarget::Unload()
{
	//Deletes the texture at the specified handle
	glDeleteTextures(1, &_texture.GetHandle());
}

ColorTarget::~ColorTarget()
{
	//Unloads the color target
	Unload();
}

void ColorTarget::Unload()
{
	glDeleteTextures(_numAttachments, &_textures[0].GetHandle());
}

Framebuffer::Framebuffer()
{
	
}

Framebuffer::~Framebuffer()
{
	Unload();
}

void Framebuffer::Unload()
{
	//deletes framebuffer
	glDeleteFramebuffers(1, &_FBO);
	//set init to false
	_isInit = false;
}

void Framebuffer::Init(unsigned width, unsigned height)
{
	//Sets the size to width and height
	SetSize(width, height);

	//inits framebuffer
	Init();
}

void Framebuffer::Init()
{
	//Generates the FBO
	glGenFramebuffers(1, &_FBO);
	//bind it
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	if (_depthActive)
	{
		_clearFlag |= GL_DEPTH_BUFFER_BIT;

		glGenTextures(1, &_depth._texture.GetHandle());

		glBindTexture(GL_TEXTURE_2D, _depth._texture.GetHandle());
		
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, _width, _height);

		//Set texture parameters
		glTextureParameteri(_depth._texture.GetHandle(), GL_TEXTURE_MIN_FILTER, _filter);
		glTextureParameteri(_depth._texture.GetHandle(), GL_TEXTURE_MAG_FILTER, _filter);
		glTextureParameteri(_depth._texture.GetHandle(), GL_TEXTURE_WRAP_S, _wrap);
		glTextureParameteri(_depth._texture.GetHandle(), GL_TEXTURE_WRAP_T, _wrap);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth._texture.GetHandle(), 0);

		glBindFramebuffer(GL_TEXTURE_2D, GL_NONE);

		//are there more than zero color attachments?
		if (_color._numAttachments)
		{
			//bcuz we have a color target we include a color buffer bit into clear flag
			_clearFlag |= GL_COLOR_BUFFER_BIT;
			
			GLuint* textureHandles = new GLuint(_color._numAttachments);
		
			glGenTextures(_color._numAttachments, textureHandles);
			for (unsigned i = 0; i < _color._numAttachments; i++)
			{
				_color._textures[i].GetHandle() = textureHandles[i];

				glBindTexture(GL_TEXTURE_2D, _color._textures[i].GetHandle());

				glTexStorage2D(GL_TEXTURE_2D, 1, _color._formats[i], _width, _height);

				glTextureParameteri(_color._textures[i].GetHandle(), GL_TEXTURE_MIN_FILTER, _filter);
				glTextureParameteri(_color._textures[i].GetHandle(), GL_TEXTURE_MAG_FILTER, _filter);
				glTextureParameteri(_color._textures[i].GetHandle(), GL_TEXTURE_WRAP_S, _wrap);
				glTextureParameteri(_color._textures[i].GetHandle(), GL_TEXTURE_WRAP_T, _wrap);
			
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _color._textures[i].GetHandle(), 0);
			}

			delete[] textureHandles;
		}

		CheckFBO();
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
		_isInit = true;
	}
}

void Framebuffer::AddDepthTarget()
{
	if (_depth._texture.GetHandle())
	{
		_depth.Unload();
	}
	_depthActive = true;
}

void Framebuffer::AddColorTarget(GLenum format)
{
	_color._textures.resize(_color._numAttachments + 1);

	_color._formats.push_back(format);

	_color._buffers.push_back(GL_COLOR_ATTACHMENT0 + _color._numAttachments);

	_color._numAttachments++;
}

void Framebuffer::BindDepthAsTexture(int textureSlot) const
{
	_depth._texture.Bind(textureSlot);
}

void Framebuffer::BindColorAsTexture(unsigned colorBuffer, int textureSlot) const
{
	_color._textures[colorBuffer].Bind(textureSlot);
}

void Framebuffer::UnbindTexture(int textureSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Framebuffer::Reshape(unsigned width, unsigned height)
{
	SetSize(width, height);
	Unload();
	_depth.Unload();
	_color.Unload();
	Init();
}

void Framebuffer::SetSize(unsigned width, unsigned height)
{
	_width = width;
	_height = height;
}

void Framebuffer::SetViewPort() const
{
	glViewport(0, 0, _width, _height);
}

void Framebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	if (_color._numAttachments)
	{
		glDrawBuffers(_color._numAttachments, &_color._buffers[0]);
	}
}

void Framebuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void Framebuffer::RenderToFSQ() const
{
	SetViewPort();
	Bind();
	DrawFullscreenQuad();
	UnBind();
}

void Framebuffer::DrawToBackBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);

	glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);
}

void Framebuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
	glClear(_clearFlag);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

bool Framebuffer::CheckFBO()
{
	Bind();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer broken\n");
		return false;
	}
	return true;
}

void Framebuffer::InitFullscreenQuad()
{
	float VBO_DATA[]
	{
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		-1.f, 1.f, 0.f,

		1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f,
		1.f, -1.f, 0.f,

		0.f, 0.f,
		1.f, 0.f,
		0.f, 1.f,

		1.f, 1.f,
		0.f, 1.f,
		1.f, 0.f
	};

	int vertexSize = 6 * 3 * sizeof(float);

	int texCoordSize = 6 * 2 * sizeof(float);

	glGenVertexArrays(1, &_fullscreenQuadVAO);
	glBindVertexArray(_fullscreenQuadVAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &_fullscreenQuadVBO);

	glBindBuffer(GL_ARRAY_BUFFER, _fullscreenQuadVBO);

	glBufferData(GL_ARRAY_BUFFER, vertexSize + texCoordSize, VBO_DATA, GL_STATIC_DRAW);

#pragma warning(push)
#pragma warning(disable : 4312)

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(vertexSize));
#pragma warning(pop)

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

}

void Framebuffer::DrawFullscreenQuad()
{
	glBindVertexArray(_fullscreenQuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(GL_NONE);
}



