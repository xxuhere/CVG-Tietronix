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
};