#include <iostream>
#include "MousepadUI.h"

void DrawOffsetVertices(
	float x, 
	float y, 
	float w, 
	float h, 
	float px, 
	float py, 
	float scale)
{
	float toLeft	= -px *			w * scale;
	float toRight	= (1.0f - px) * w * scale;
	float toTop		= -py *			h * scale;
	float toBot		= (1.0 - py) *	h * scale;

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x + toLeft, y + toTop);
		//
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + toRight, y + toTop);
		//
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + toRight, y + toBot);
		//
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x + toLeft, y + toBot);
	}
	glEnd();
}

void DrawOffsetVertices(float x, float y, TexObj& to, float px, float py, float scale)
{
	to.GLBind();
	DrawOffsetVertices(x, y, to.width, to.height, px, py, scale);
}

Message::Message(MessageType msgTy, int idx)
{
	this->msgTy = msgTy;
	this->idx = idx;
}

void MousepadUI::ButtonState::Reset()
{
	this->heldDownTimer.Restart();
	this->isDown		= false;
	this->clickRecent	= 0.0f;
	this->isDown		= false;
}

MousepadUI::ButtonState::ButtonState(
	int buttonIdx,
	const std::string& normImgPath,
	const std::string& pressedImgPath,
	const std::string& holdimgPath)
{
	this->buttonIdx = buttonIdx;

	this->InitializeButtonGraphics(normImgPath, pressedImgPath, holdimgPath);
}

void MousepadUI::ButtonState::DrawOffsetVerticesForButtonSet(
	float x, 
	float y, 
	float px, 
	float py, 
	float scale,
	float holdingThreshold)
{
	float downTime = this->heldDownTimer.Seconds(false);

	if(this->isDown == false)
		DrawOffsetVertices(x, y, this->normal, px, py, scale);
	else if(downTime < holdingThreshold)
		DrawOffsetVertices(x, y, this->pressed, px, py, scale);
	else
		DrawOffsetVertices(x, y, this->hold, px, py, scale);

}

UIColor4 MousepadUI::ButtonState::GetMousepadColor()
{
	// Pressed down
	if(this->isDown)
		return UIColor4(0.75f, 0.75f, 0.25f, 1.0f);


	// Fading
	float colorComp =  std::clamp(1.0f - this->clickRecent, 0.0f, 1.0f);
	return UIColor4(colorComp, 1.0f, colorComp);
}

MousepadUI::MousepadUI()
	:	btnLeft(	0, "", "", ""),	// Empty strings for images paths to load, because 
		btnMiddle(	1, "", "", ""),	// that will be handled later in this->Initialize()
		btnRight(	2, "", "", "")
{}

bool MousepadUI::Initialize()
{
	if(this->initialized)
		return false;

	this->initialized = true;

	this->fontInsBAnno = FontMgr::GetInstance().GetFont(12);
	
	this->btnLeft.InitializeButtonGraphics(
		"Assets/Mousepad/Mousepad_Left_Norm.png",
		"Assets/Mousepad/Mousepad_Left_Pressed.png",
		"Assets/Mousepad/Mousepad_Left_Hold.png");

	this->btnRight.InitializeButtonGraphics(
		"Assets/Mousepad/Mousepad_Right_Norm.png",
		"Assets/Mousepad/Mousepad_Right_Pressed.png",
		"Assets/Mousepad/Mousepad_Right_Hold.png");

	this->btnMiddle.InitializeButtonGraphics(
		"Assets/Mousepad/Mousepad_MiddleBall_Norm.png",
		"Assets/Mousepad/Mousepad_MiddleBall_Pressed.png",
		"Assets/Mousepad/Mousepad_MiddleBall_Hold.png");

	this->ico_MousePadCrevice.LODEIfEmpty(	"Assets/Mousepad/Mousepad_Crevice.png");

	this->ico_CircleBacking.LODEIfEmpty("Assets/Mousepad/Mousepad_CircleBacking.png");
	return true;
}

MousepadUI::ButtonState* MousepadUI::GetButton(int buttonIdx)
{
	switch(buttonIdx)
	{
	case 0:
		return &this->btnLeft;

	case 1:
		return &this->btnMiddle;

	case 2:
		return &this->btnRight;
	}
	return nullptr;
}

bool MousepadUI::ButtonState::InitializeButtonGraphics(
	const std::string& normImgPath,
	const std::string& pressedImgPath,
	const std::string& holdimgPath)
{
	bool ret = true;
	ret = this->normal.LODEIfEmpty(normImgPath) && ret;
	ret = this->pressed.LODEIfEmpty(pressedImgPath) && ret;
	ret = this->hold.LODEIfEmpty(holdimgPath) && ret;

	return ret;
}

void MousepadUI::Update(double dt)
{
	this->btnLeft.Decay(dt);
	this->btnMiddle.Decay(dt);
	this->btnRight.Decay(dt);
}

void DrawHollowRadial(int segments, float percent, float innerDiameter, float outerDiameter, float centerX, float centerY)
{
	const float PI = 3.14159f;
	float radians = percent * 2.0f * PI;
	
	glBegin(GL_QUADS);
	for(int i = 0; i < segments; ++i)
	{
		// Shifts radians from trig functions (cos/sin) to start at the 
		// top instead of at the middle right
		const float RadialShift = -PI * 0.5f;

		float lam_0 = RadialShift + ((float)(i + 0) / (float)segments) * radians;
		float lam_1 = RadialShift + ((float)(i + 1) / (float)segments) * radians;

		float seg_0x = cos(lam_0);
		float seg_0y = sin(lam_0);
		float seg_1x = cos(lam_1);
		float seg_1y = sin(lam_1);

		glVertex2f(centerX + seg_0x * outerDiameter,	centerY + seg_0y * outerDiameter);
		glVertex2f(centerX + seg_1x * outerDiameter,	centerY + seg_1y * outerDiameter);
		glVertex2f(centerX + seg_1x * innerDiameter,	centerY + seg_1y * innerDiameter);
		glVertex2f(centerX + seg_0x * innerDiameter,	centerY + seg_0y * innerDiameter);

	}
	glEnd();
}

void MousepadUI::Render(IMousepadUIBehaviour* uiProvider, float x, float y, float scale)
{
	
	static UIColor4 disabledBtnColor(0.2f, 0.2f, 0.2f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw the middle mouse button backplate.
	this->ico_MousePadCrevice.GLBind();
	DrawOffsetVertices(x, y, this->ico_MousePadCrevice, 0.5f, 1.0f, scale);

	//////////////////////////////////////////////////
	//
	//	GATHER DATA
	//
	//////////////////////////////////////////////////

	// Setup the substate, and collect what buttons are allowed to be shown.
	int hasButtonClick[3] = {true, true, true};
	int hasButtonHold[3] = {true, true, true};
	if(uiProvider)
	{
		for(int i = 0; i < (int)ButtonID::Totalnum; ++i)
		{ 
			hasButtonClick[i] = uiProvider->GetButtonUsable((ButtonID)i, false);
			hasButtonHold[i] = uiProvider->GetButtonUsable((ButtonID)i, true);
		}
	}


	//////////////////////////////////////////////////
	//
	//	DEFINE HOW TO RENDER THE BUTTONS
	//
	//////////////////////////////////////////////////

	// Define a structure that has all the parameters of how we'll draw each
	// button. Then we draw all button mechanically the same way based on only
	// the mechanics and data.
	// 
	struct IcoDrawData
	{
	public:
		// The button ID being handled
		ButtonID bid;

		// The location of the graphic to line up at the center.
		//
		// Note that the side of the button graphic will depend on the actual asset's size.
		float btnPercentOffsX;
		float btnPercentOffsY;

		// Offset location of the center of the icon.
		UIVec2 icoOffs;
		// Offset of pivot location of the text
		UIVec2 texOffs;
		// Pivot location of text, using vector components [0, 1],
		// where 0 is top left, and 1 is bottom right
		UIVec2 texPivot;

		// If true, the hold dial will be drawn on itself. Else, it will be offset via holdIconOffset.
		bool holdDialOnSelf;
		// The offset to the center of the hold icon. Only relevant when holdDialOnSelf is false.
		UIVec2 holdIconOffset;
		UIVec2 holdTexOffs;
		UIVec2 holdTexPivot;

	public:
		UIVec2 GetAnnotationCenter(float x, float y, float scale) const
		{
			return UIVec2(
				x + this->icoOffs.x * scale,
				y + this->icoOffs.y * scale);
		}

		UIVec2 GetHoldAnnotationCenter(float x, float y, float scale) const
		{
			UIVec2 ret = GetAnnotationCenter(x, y, scale);

			if(holdDialOnSelf)
				return ret;

			ret.x += this->holdIconOffset.x * scale;
			ret.y += this->holdIconOffset.y * scale;

			return ret;
		}
			
	};
	// Note that this data is static, meaning it's initialized in global memory only once,
	// Either the first time it's used, or when the program is initialized.
	static const IcoDrawData rdata[(int)ButtonID::Totalnum] = // UI Rendering data
	{
		// These indices will map to HMDOpSub_Base::ButtonID,
		// [0] -> ButtonID::Left
		{ButtonID::Left,	1.0f, 1.0f,	UIVec2(-175.0f,	-175.0f),	UIVec2(-90.0f, -20.0f),UIVec2(1.0f, 0.5f), false,	UIVec2(-400.0f, 100.0f),UIVec2(-70.0f, 0.0f),	UIVec2(1.0f, 0.5f)},	
		// [1] -> ButtonID::Middle
		{ButtonID::Middle,	0.5f, 0.5f,	UIVec2(0.0f,	0.0f),		UIVec2(70.0f, 20.0f),	UIVec2(0.0f, 0.5f), true,	UIVec2(0.0f, 0.0f),		UIVec2(50.0f, 50.0f),	UIVec2(0.5f, 0.5f)},	
		// [2] -> ButtonID::Right
		{ButtonID::Right,	0.0f, 1.0f, UIVec2(175.0f,	-175.0f),	UIVec2(90.0f, -20.0f),	UIVec2(0.0f, 0.5f), false,	UIVec2(400.0f, 100.0f), UIVec2(75.0f, 0.0f),	UIVec2(0.0f, 0.5f)}
	};


	//////////////////////////////////////////////////
	//
	//	RENDER BUTTON BACKPLATES
	//
	//////////////////////////////////////////////////
	for(int i = 0; i < (int)ButtonID::Totalnum; ++i)
	{
		const IcoDrawData& icd = rdata[i];
		ButtonState* bs = this->GetButton(i);

		if(hasButtonClick[i])
			bs->GetMousepadColor().GLColor4();
		else
			glColor3f(0.5f, 0.5f, 0.5f);

		// Draw the mouse graphic
		bs->DrawOffsetVerticesForButtonSet(
			x, 
			y,
			icd.btnPercentOffsX, 
			icd.btnPercentOffsY,
			scale,
			this->ButtonHoldTime);
	}

	//////////////////////////////////////////////////
	//
	//	RENDER HOLD ICONS
	//
	//////////////////////////////////////////////////
	if(uiProvider)
	{ 
		const float PI = 3.14159f;
		const int circleParts = 32;
		float radOut = 80.0f * scale;
		float radIn = radOut - 10.0f;
	
		for(int i = 0; i < (int)ButtonID::Totalnum; ++i)
		{
			ButtonID bid = (ButtonID)i;
			const IcoDrawData& icd = rdata[i];
			UIVec2 annotationCenter = icd.GetHoldAnnotationCenter(x, y, scale);
			ButtonState* bs = this->GetButton(i);
	
			glEnable(GL_TEXTURE_2D);
	
			// If re-using the non-hold annotation, don't render backing plate and circle.
			if(!icd.holdDialOnSelf)
			{
				DrawOffsetVertices(
					annotationCenter.x, 
					annotationCenter.y, 
					this->ico_CircleBacking, 
					0.5f, 0.5f, 
					scale);
	
				if(hasButtonHold[i])
				{ 
	
					glColor3f(1.0f, 1.0f, 1.0f);
					DrawOffsetVertices(
						annotationCenter.x, 
						annotationCenter.y, 
						this->btnMiddle.normal, 
						0.5f, 0.5f, 
						scale);
	
					TexObj::SPtr btnIco = this->GetBAnnoIco(uiProvider->GetIconPath(bid, true));
					if(btnIco)
					{ 
						DrawOffsetVertices(
							annotationCenter.x, 
							annotationCenter.y, 
							*btnIco.get(), 
							0.5f, 0.5f, 
							scale);
					}

					float secondsMidDown = 0.0f;
					if(bs->isDown)
						secondsMidDown = bs->heldDownTimer.Seconds(false);

					if(!icd.holdDialOnSelf && hasButtonHold[i] && secondsMidDown >= ButtonHoldTime)
						bs->GetMousepadColor().GLColor3();
					else
						glColor3f(1.0f, 1.0f, 1.0f);
	
					// Get position of text from location and offset
					float baTxtX = annotationCenter.x + icd.holdTexOffs.x * scale;
					float baTxtY = annotationCenter.y + icd.holdTexOffs.y * scale;
					std::string holdStr = uiProvider->GetActionName(bid, true);
					// Apply pivot offset.
					float extHoriz = this->fontInsBAnno.GetAdvance(holdStr.c_str());
					float extVert = this->fontInsBAnno.LineHeight();
					baTxtX -= extHoriz * icd.holdTexPivot.x;
					baTxtY += extVert * icd.holdTexPivot.y;
	
					this->fontInsBAnno.RenderFont(holdStr.c_str(), baTxtX, baTxtY);
	
					// DRAW THE RADIAL
					if(secondsMidDown > 0.0f)
					{ 
						float ringFilled = std::min(1.0f, secondsMidDown / ButtonHoldTime);
				
						glDisable(GL_TEXTURE_2D);
						if(secondsMidDown < ButtonHoldTime)
							glColor3f(0.0f, 1.0f, 0.0f);
						else
							glColor3f(1.0f, 0.5f, 0.0f);
				
						DrawHollowRadial(circleParts, ringFilled, radIn, radOut, annotationCenter.x, annotationCenter.y);
					}
				}
				else
				{
					glColor3f(0.5f, 0.5f, 0.5f);
					DrawOffsetVertices(
						annotationCenter.x, 
						annotationCenter.y, 
						this->btnMiddle.normal, 
						0.5f, 0.5f, 
						scale);
				}
			}
			else
			{
				if(bs->isDown)
				{ 
					float secondsMidDown	= bs->heldDownTimer.Seconds(false);
					float ringFilled = std::min(1.0f, secondsMidDown / ButtonHoldTime);

					glDisable(GL_TEXTURE_2D);
					if(secondsMidDown < ButtonHoldTime)
						glColor3f(0.0f, 1.0f, 0.0f);
					else
						glColor3f(1.0f, 0.5f, 0.0f);

					DrawHollowRadial(circleParts, ringFilled, radIn, radOut, annotationCenter.x, annotationCenter.y);
				}
			}
		}
	}

	//////////////////////////////////////////////////
	//
	//	RENDER CLICK ANNOTATIONS
	//	Both icons and text
	//
	//////////////////////////////////////////////////
	if(uiProvider)
	{ 
		glEnable(GL_TEXTURE_2D);
		for(int i = 0; i < (int)ButtonID::Totalnum; ++i)
		{
			ButtonID bid = (ButtonID)i;
			TexObj::SPtr btnIco = this->GetBAnnoIco(uiProvider->GetIconPath(bid, false));
			ButtonState* bs = this->GetButton(i);
			const IcoDrawData& icd = rdata[i];

			bool isHeld = bs->isDown && bs->heldDownTimer.Seconds(false) >= ButtonHoldTime;
			// Override to show the hold value on the click?
			bool showHold = hasButtonHold[i] && isHeld && icd.holdDialOnSelf;

			if(!showHold && !hasButtonClick[i])
				continue;


			glColor3f(1.0f, 1.0f, 1.0f);
			UIVec2 annoCenter = icd.GetAnnotationCenter(x, y, scale);
			if(btnIco.get() != nullptr)
			{
				DrawOffsetVertices(
					annoCenter.x, 
					annoCenter.y, 
					*btnIco.get(), 
					0.5f, 0.5f, 
					scale);
			}

			
			// Color the text the same as the button color (for now).
			//
			// If it's held down, don't emphasize it, and instead emphasize the hold-down
			if(!showHold)
				glColor3f(1.0f, 1.0f, 1.0f);
			else
				bs->GetMousepadColor().GLColor3();

			std::string bannoStr;
			if(showHold)
				bannoStr = uiProvider->GetActionName((ButtonID)i, true);
			else
				bannoStr = uiProvider->GetActionName((ButtonID)i, false);
			
			if(!bannoStr.empty())
			{
				// Get position of text from location and offset
				float baTxtX = annoCenter.x + icd.texOffs.x * scale;
				float baTxtY = annoCenter.y + icd.texOffs.y * scale;
				// Apply pivot offset.
				float extHoriz = this->fontInsBAnno.GetAdvance(bannoStr.c_str());
				float extVert = this->fontInsBAnno.LineHeight();
				baTxtX -= extHoriz * icd.texPivot.x;
				baTxtY += extVert * icd.texPivot.y;

				this->fontInsBAnno.RenderFont(bannoStr.c_str(), baTxtX, baTxtY);
			}
		}
	}
}

void MousepadUI::OnButtonUp(IMousepadUIBehaviour* uib, int button)
{
	// NOTE: There's a lot of similar code, we may want to look into
	// a way to unify the code for the various similar key up handlers.

	ButtonState* bs = this->GetButton(button);
	if(bs == nullptr)
		return;

	int msDown = bs->heldDownTimer.Milliseconds(true);
	float secDown = (float)msDown/1000.0f;



	if(uib != nullptr)
	{ 
		if(secDown >= ButtonHoldTime)
			uib->HandleMessage(Message(MessageType::HoldUp, button));
		else
			uib->HandleMessage(Message(MessageType::Up, button));
	}

	bs->FlagUp();
	bs->isDown = false;
}

void MousepadUI::OnButtonDown(IMousepadUIBehaviour* uib, int button)
{
	ButtonState* bs = this->GetButton(button);
	if(bs == nullptr)
		return;

	// It might seem odds to check if the button was already down
	// before doing down press stuff, but this is the timer for if
	// the button is being held down, and there could be other
	// things that emulate mouse button clicks such as foot pedals
	// that emulate keyboard presses - and keyboard presses can repeat themselves
	// when held down (such as in notepad, when press 'a', you'll immediately
	// get "a", but after a while you'll get "aaaaaaaaaaaaaaaaaaaaaaaaaa" 
	// being spammed from the input.)
	//
	// If this is the case, we don't want t keep restarting the timer - but
	// recognize it as one continuous (simulated) mouse-down press.
	//
	// Note that this only affects the middle mouse button, and only for not
	// restarting the timer for holding down the button. A more robust and 
	// general solution may be required for additional/future features.
	if(!bs->isDown)		
		bs->heldDownTimer.Restart();

	bs->FlagDown();

	if(uib != nullptr)
		uib->HandleMessage(Message(MessageType::Down, button));
}

void MousepadUI::ButtonState::Decay(double dt)
{
	if(this->isDown)
		return;

	this->clickRecent = 
		(float)std::max(0.0, this->clickRecent	- dt * clickDecayRate);
}

void MousepadUI::ButtonState::FlagUp()
{
	this->isDown = false;
}

void MousepadUI::ButtonState::FlagDown()
{
	this->isDown = true;
	this->clickRecent = 1.0f;
}

void MousepadUI::Reset()
{
	this->btnLeft.Reset();
	this->btnMiddle.Reset();
	this->btnRight.Reset();
}

TexObj::SPtr MousepadUI::GetBAnnoIco(const std::string& path)
{
	if(path.empty())
		return nullptr;

	auto itFind = this->cachedBAnnoIcos.find(path);
	if(itFind == this->cachedBAnnoIcos.end())
	{
		BAnnoIcon bico;
		bico.path = path;
		bico.loaded = TexObj::MakeSharedLODE(path);

		if(bico.loaded == nullptr)
		{
			std::cerr << "Failed to load button annotation " << path << std::endl;
		}

		this->cachedBAnnoIcos[path] = bico;

		return bico.loaded;
	}

	return itFind->second.loaded;
}

void MousepadUI::Shutdown()
{
	this->cachedBAnnoIcos.clear();
}