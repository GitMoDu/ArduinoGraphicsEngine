// SpriteTransformDemo.h

#ifndef _SPRITE_TRANSFORM_DEMO_h
#define _SPRITE_TRANSFORM_DEMO_h

#include <ArduinoGraphicsDrawer.h>

#include "Sprites.h"

using namespace DemoSprites;
using namespace SpriteShaderEffect;
using namespace SpriteShader;
using namespace SpriteTransform;

template<typename Layout>
class TransformDemo : public ElementDrawer
{
private:
	enum class DrawElementsEnum : uint8_t
	{
		Skew,
		Scale,
		Rotate,
		DrawElementsCount
	};

	static constexpr uint32_t SkewDuration = 1400000;
	static constexpr uint32_t ScaleDuration = 1500000;
	static constexpr uint32_t RotationDuration = 4000000;

	struct RectangleLayout
	{
		static constexpr uint8_t Width() { return 15; }
		static constexpr uint8_t Height() { return 7; }
	};

	ColorShader<HeartSprite> Heart{};
	HorizontalGradientShader<PyramidSprite> Pyramid{};
	GridShader<RectangleSprite<RectangleLayout::Width(), RectangleLayout::Height()>> Rectangle{};

	SkewHorizontalTransform<RectangleLayout::Height()> RectangleHorizontalSkewer{};
	SkewVerticalTransform<RectangleLayout::Width()> RectangleVerticalSkewer{};
	DownScaleXYTransform<HeartSprite::Width, HeartSprite::Height> HeartDownScaler{};
	RotateTransform<PyramidSprite::Width, PyramidSprite::Height> PyramidRotator{};

	uint8_t Rotation = 0;
	int8_t Angle = 0;

public:
	TransformDemo(IFrameBuffer* frame)
		: ElementDrawer(frame, (uint8_t)DrawElementsEnum::DrawElementsCount)
	{
		Heart.SetColor(UINT8_MAX, 0, 0);

		Rectangle.SetColor1(0, 0, UINT8_MAX);
		Rectangle.SetColor2(0, UINT8_MAX, 0);

		RectangleHorizontalSkewer.SetReferenceY(RectangleLayout::Height() / 2);
		RectangleVerticalSkewer.SetReferenceX(0);

		Pyramid.SetColor1(0xFF, 0x5A, 0x23);
		Pyramid.SetColor2(0xFF, 0xd3, 0x91);
	}

	void DrawCall(DrawState* drawState) final
	{
		switch (drawState->ElementIndex)
		{
		case (uint8_t)DrawElementsEnum::Skew:
			DrawSkew(drawState);
			break;
		case (uint8_t)DrawElementsEnum::Scale:
			DrawScale(drawState);
			break;
		case (uint8_t)DrawElementsEnum::Rotate:
			DrawRotate(drawState);
			break;
		default:
			break;
		}
	}

private:
	void DrawSkew(DrawState* drawState)
	{
		const uint16_t sectionProgress = drawState->GetProgress<SkewDuration * 2>();
		const uint16_t progress = drawState->GetProgress<SkewDuration>() + (INT16_MAX / 2);

		if (ProgressScaler::ScaleProgress(sectionProgress, (uint8_t)2) == 0)
		{
			const int8_t skew = -(int16_t)RectangleLayout::Width() + (int16_t)ProgressScaler::ScaleProgress(ProgressScaler::TriangleResponse(progress), (uint8_t)(RectangleLayout::Width() * 2));
			RectangleHorizontalSkewer.SetSkewX(skew);
			
			SpriteRenderer::TransformDraw(Frame, &Rectangle, &RectangleHorizontalSkewer,
				Layout::X() + (RectangleLayout::Width() / 2), Layout::Y() + RectangleLayout::Height());
		}
		else
		{
			const int8_t skew = -(int16_t)RectangleLayout::Height() + (int16_t)ProgressScaler::ScaleProgress(ProgressScaler::TriangleResponse(progress), (uint8_t)(RectangleLayout::Height() * 2));
			RectangleVerticalSkewer.SetSkewY(skew);
			
			SpriteRenderer::TransformDraw(Frame, &Rectangle, &RectangleVerticalSkewer,
				Layout::X() + (RectangleLayout::Width() / 2), Layout::Y() + RectangleLayout::Height());
		}	
	}

	void DrawScale(DrawState* drawState)
	{
		const uint16_t progress = ProgressScaler::TriangleResponse(drawState->GetProgress<ScaleDuration>());
		const uint8_t downScale = ProgressScaler::ScaleProgress(progress, (uint8_t)(HeartSprite::Width - 1));

		HeartDownScaler.SetDownScaleXY(downScale);

		SpriteRenderer::TransformDraw(
			Frame, &Heart, &HeartDownScaler,
			Layout::X() + (downScale / 2), Layout::Y() + (((uint16_t)Layout::Height() * 2) / 3) - (HeartSprite::Height / 2) + (downScale / 2));
	}

	void DrawRotate(DrawState* drawState)
	{
		const uint16_t progress = drawState->GetProgress<RotationDuration>();
		const int16_t angle = ProgressScaler::ScaleProgress(progress, (uint16_t)360);

		PyramidRotator.SetRotation(angle);

		SpriteRenderer::TransformDraw(Frame, &Pyramid, &PyramidRotator,
			(Layout::Width() - PyramidSprite::Width) / 2,
			(Layout::Height() - PyramidSprite::Height) / 2);
	}
};
#endif

