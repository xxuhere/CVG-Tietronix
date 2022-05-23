#pragma once

class UIColor4
{
public:
	union
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		float ar[4];
	};
public:
	UIColor4();
	UIColor4(float r, float g, float b);
	UIColor4(float r, float g, float b, float a);

	void Set(float r, float g, float b);
	void Set(float r, float g, float b, float a);

	void GLColor3();
	void GLColor4();

	inline void SetColor_Black()	{ this->Set(0.0f, 0.0f, 0.0f); }
	inline void SetColor_White()	{ this->Set(1.0f, 1.0f, 1.0f); }
	inline void SetColor_FRed()		{ this->Set(1.0f, 0.0f, 0.0f); }
	inline void SetColor_FGreen()	{ this->Set(0.0f, 1.0f, 0.0f); }
	inline void SetColor_FBlue()	{ this->Set(0.0f, 0.0f, 1.0f); }
	inline void SetColor_FCyan()	{ this->Set(0.0f, 1.0f, 1.0f); }
	inline void SetColor_FMagenta()	{ this->Set(1.0f, 1.0f, 0.0f); }

	bool operator== (const UIColor4& o) const;
};

struct ColorSetInteractable
{
public:
	UIColor4 norm;
	UIColor4 hover;
	UIColor4 pressed;

public:
	ColorSetInteractable();

	ColorSetInteractable(
		const UIColor4& norm, 
		const UIColor4& hover, 
		const UIColor4& pressed);

	void Set(
		const UIColor4& norm, 
		const UIColor4& hover, 
		const UIColor4& pressed);

	UIColor4& GetContexedColor(int pressedCt, bool isHovering);
};