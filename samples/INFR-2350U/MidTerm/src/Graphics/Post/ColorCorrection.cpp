#include "ColorCorrection.h"

void ColorCorrection::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->Init(width, height);

	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/Post/ColorCorrection.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	_LUT.loadFromFile("cubes/ShiftedBrightenedCorrection.cube");
	//LUT3D _LUT("cubes/ShiftedBrightenedCorrection.cube");
	SetLUT(_LUT);
	PostEffect::Init(width, height);
}

void ColorCorrection::ApplyEffect(PostEffect* buffer)
{

	//Binds buffer
	_buffers[0]->Bind();
	//Binds shader
	_shaders[0]->Bind();
	//Binds Texture to slot 30
	buffer->BindColorAsTexture(0, 0, 0);
	// Binds LUT to slot 30
	_LUT.bind(30);
	//Draws Quad
	_buffers[0]->DrawFullscreenQuad();
	//Unbinds LUT
	_LUT.unbind(30);
	//Unbinds texture
	buffer->UnbindTexture(30);
	//Unbinds shader
	_shaders[0]->UnBind();
	//Unbinds buffer
	_buffers[0]->Unbind();

}



LUT3D ColorCorrection::GetLUT() const
{
	return _LUT;
}

void ColorCorrection::SetLUT(LUT3D lut)
{
	_LUT = lut;
}

