#pragma once

#include "Graphics/Post/PostEffect.h"
#include "Graphics/LUT.h"

class ColorCorrection : public PostEffect
{
public:
	void Init(unsigned width, unsigned height);

	void ApplyEffect(PostEffect* buffer) override;

	LUT3D GetLUT() const;

	void SetLUT(LUT3D lut);
private:
	float _intensity = 1.0f;
	LUT3D _LUT;
};
