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
	float downTime = this->heldDownTimer.Milliseconds(false) / 1000.0f;

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
	:	btnLeft(	0, "", "", ""),
		btnMiddle(	1, "", "", ""),
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

void MousepadUI::Render(IMousepadUIBehaviour* uiProvider, float x, float y, float scale)
{
	static UIColor4 disabledBtnColor(0.2f, 0.2f, 0.2f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor3f(1.0f, 1.0f, 1.0f);

	// For all the quads we're about to lay down, we're starting at
	// the top left and moving clockwise.
	this->ico_MousePadCrevice.GLBind();
	UIColor4 colMLeft	= this->btnLeft.GetMousepadColor();
	UIColor4 colMMiddle = this->btnMiddle.GetMousepadColor();
	UIColor4 colMRight	= this->btnRight.GetMousepadColor();

	// Setup the substate, and collect what buttons are allowed to be shown.
	int hasButton[4] = 
	{true, true, true, true};
	if(uiProvider)
	{
		for(int i = 0; i < (int)ButtonID::Totalnum; ++i)
			hasButton[i] = uiProvider->GetButtonUsable((ButtonID)i);
	}

	// Draw the middle mouse button.
	DrawOffsetVertices(x, y, this->ico_MousePadCrevice, 0.5f, 1.0f, scale);
	//
	if(hasButton[1] || hasButton[3])
		colMMiddle.GLColor4();
	else
		disabledBtnColor.GLColor4();

	this->btnMiddle.DrawOffsetVerticesForButtonSet(
		x, 
		y,
		0.5f, 
		0.5f,
		scale,
		this->ButtonHoldTime);

	// Draw the dial/ring for holding the middle mouse button down.
	if(this->btnMiddle.isDown && hasButton[3])
	{
		glDisable(GL_TEXTURE_2D);

		// If a mouse button is pressed down, draw a ring
		// whos angle represents the amount of the time the button
		// has been pressed.
		//
		// Since all mouse buttons can be pressed, and technically
		// can be pressed simultaneously, we use the largest timer
		// for the value to draw the ring for.
		const float PI = 3.14159f;
		const int circleParts = 32;

		float secondsMidDown	= 0.0f;
		//
		if(this->btnMiddle.isDown && hasButton[3])
			secondsMidDown	= this->btnMiddle.heldDownTimer.Milliseconds(false) / 1000.0f;

		float arcLen = std::min(1.0f, secondsMidDown / ButtonHoldTime) * 2.0f * PI;

		if(secondsMidDown < ButtonHoldTime)
			glColor3f(0.0f, 1.0f, 0.0f);
		else
			glColor3f(1.0f, 0.5f, 0.0f);

		float radOut = 80.0f * scale;
		float radIn = radOut - 10.0f;
		glBegin(GL_QUADS);
		for(int i = 0; i < circleParts; ++i)
		{
			float lam_0 = -PI * 0.5f + ((float)(i + 0) / (float)circleParts) * arcLen;
			float lam_1 = -PI * 0.5f + ((float)(i + 1) / (float)circleParts) * arcLen;

			float seg_0x = cos(lam_0);
			float seg_0y = sin(lam_0);
			float seg_1x = cos(lam_1);
			float seg_1y = sin(lam_1);

			glVertex2f(x + seg_0x * radOut, y + seg_0y * radOut);
			glVertex2f(x + seg_1x * radOut, y + seg_1y * radOut);
			glVertex2f(x + seg_1x * radIn,	y + seg_1y * radIn);
			glVertex2f(x + seg_0x * radIn,	y + seg_0y * radIn);

		}
		glEnd();
		glEnable(GL_TEXTURE_2D);
	}

	// Draw the left mouse button.
	if(hasButton[0])
		colMLeft.GLColor4();
	else
		disabledBtnColor.GLColor4();

	this->btnLeft.DrawOffsetVerticesForButtonSet(
		x, 
		y, 
		1.0f, 
		1.0f, 
		scale,
		this->ButtonHoldTime);

	// Draw the right mouse button.
	if(hasButton[2])
		colMRight.GLColor4();
	else
		disabledBtnColor.GLColor4();

	this->btnRight.DrawOffsetVerticesForButtonSet(
		x, 
		y, 
		0.0f, 
		1.0f, 
		scale,
		this->ButtonHoldTime);

	// Draw the button annotations.
	if(uiProvider != nullptr)
	{
		glColor3f(0.5f, 0.5f, 0.5f);

		struct IcoDrawData
		{
			// Offset location of the center of the icon.
			UIVec2 icoOffs;
			// Offset of pivot location of the text
			UIVec2 texOffs;
			// Pivot location of text, using vector components [0, 1],
			// where 0 is top left, and 1 is bottom right
			UIVec2 texPivot;
		};
		static const IcoDrawData rdata[4] =
		{
			// These indices will map to HMDOpSub_Base::ButtonID,
			// [0] -> ButtonID::Left
			{UIVec2(-175.0f,	-175.0f),	UIVec2(-250.0f, -175.0f),	UIVec2(1.0f, 0.5f)},	
			// [1] -> ButtonID::Middle
			{UIVec2(0.0f,		0.0f),		UIVec2(70.0f, 20.0f),		UIVec2(0.0f, 0.5f)},	
			// [2] -> ButtonID::Right
			{UIVec2(175.0f,		-175.0f),	UIVec2(250.0f, -175.0f),	UIVec2(0.0f, 0.5f)},	
			// [3] -> ButtonID::HoldMiddle
			{UIVec2(275.0f,		-25.0f),	UIVec2(310, -25.0f),		UIVec2(0.0f, 0.25f)},	
		};

		// There's some violation of type saftey here. Ideally we would find a way
		// to have substateMachine to hold items of type HMDOpSub_Base instead of 
		// it's parent class, Substate<StateHMDOp>.

		for(int i = 0; i < (int)ButtonID::Totalnum; ++i)
		{
			if(!hasButton[i])
				continue;

			ButtonID bid = (ButtonID)i;
			TexObj::SPtr btnIco = this->GetBAnnoIco(uiProvider->GetIconPath(bid));

			if(btnIco.get() != nullptr)
			{
				DrawOffsetVertices(
					x + rdata[i].icoOffs.x * scale, 
					y + rdata[i].icoOffs.y * scale, 
					*btnIco.get(), 
					0.5f, 0.5f, 
					scale);
			}

			//this->fontInsBAnno
			std::string bannoStr = uiProvider->GetActionName(bid);
			if(!bannoStr.empty())
			{
				// Get position of text from location and offset
				float baTxtX = x + rdata[i].texOffs.x * scale;
				float baTxtY = y + rdata[i].texOffs.y * scale;
				// Apply pivot offset.
				float extHoriz = this->fontInsBAnno.GetAdvance(bannoStr.c_str());
				float extVert = this->fontInsBAnno.LineHeight();
				baTxtX -= extHoriz * rdata[i].texPivot.x;
				baTxtY += extVert * rdata[i].texPivot.y;

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

	this->clickRecent = (float)std::max(0.0, this->clickRecent	- dt * clickDecayRate);
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