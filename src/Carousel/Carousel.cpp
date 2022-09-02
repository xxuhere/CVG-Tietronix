#include "Carousel.h"
#include "../UISys/CacheRecordUtils.h"

CarouselMoment::CarouselMoment()
{
}

CarouselMoment::CarouselMoment(
	const UIRect& relIcon, 
	const UIColor4& bgColor, 
	const UIColor4& iconColor,
	const UIColor4& textColor,
	const UIColor4& outlineColor,
	float outlineThickness,
	float labelSize,
	float labelRot,
	float indentIn,
	float pushEmphasis,
	float trim,
	const UIVec2& labelRelPos)
{
	this->relIcon			= relIcon;
	this->bgColor			= bgColor;
	this->iconColor			= iconColor;
	this->textColor			= textColor;
	this->outlineColor		= outlineColor;
	this->outlineThickness	= outlineThickness;
	this->labelSize			= labelSize;
	this->labelRot			= labelRot;
	this->indentIn			= indentIn;
	this->pushEmphasis		= pushEmphasis;
	this->trim				= trim;
	this->labelRelPos		= labelRelPos;
}

CarouselMoment CarouselMoment::Lerp(
	const CarouselMoment& a, 
	const CarouselMoment& b, 
	float t)
{
	return CarouselMoment(
		UIRect::Lerp(	a.relIcon,			b.relIcon,				t),
		UIColor4::Lerp(	a.bgColor,			b.bgColor,				t),
		UIColor4::Lerp(	a.iconColor,		b.iconColor,			t),
		UIColor4::Lerp(	a.textColor,		b.textColor,			t),
		UIColor4::Lerp(	a.outlineColor,		b.outlineColor,			t), 
		::Lerp(			a.outlineThickness,	b.outlineThickness,		t),
		::Lerp(			a.labelSize,		b.labelSize,			t),
		::Lerp(			a.labelRot,			b.labelRot,				t),
		::Lerp(			a.indentIn,			b.indentIn,				t),
		::Lerp(			a.pushEmphasis,		b.pushEmphasis,			t),
		::Lerp(			a.trim,				b.trim,					t),
		UIVec2::Lerp(	a.labelRelPos,		b.labelRelPos,			t));

}

CarouselStyle::CarouselStyle()
{
	this->SetOrientation(this->orientation);
}

void CarouselStyle::SetOrientation(Orientation o)
{
	this->orientation = o;

	if(o == Orientation::Vertical)
		this->SetDefaultMomentsVertical();
	else
		this->SetDefaultMomentsHorizontal();
}

void CarouselStyle::SetDefaultMomentsHorizontal()
{

	// Offset of where labels are placed,
	// for compressed entries.
	UIVec2 horizLabelOffs(15.0f, 80.0f);

	this->drawMoment_Active = 
		CarouselMoment( 
			UIRect(10.0f, 10.0f, 80.0f, 80.0f), // Interior icon
			UIColor4(1.0f, 1.0f, 1.0f, 1.5f),	// Background color
			UIColor4(1.0f, 1.0f, 1.0f, 1.0),	// Icon color
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),	// Text color
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),	// Outline color
			3.0f, 14, 0.0f, 0.0f, 10.0f, 0.0f,	// Thickness / LabelSz / LabelRot / PlateInd / PushEmphasis / Trim
			UIVec2(40, 105.0f));

	this->drawMoment_Nbr0 =
		CarouselMoment(
			UIRect(2.0f, 2.0f, 16.0f, 16.0f),
			UIColor4(0.5f, 0.5f, 0.5f, 1.0f),
			UIColor4(1.0f, 1.0f, 1.0f, 0.5f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			1.0f, 8.0f, 90.0f, 5.0f, 5.0f, 0.0f,
			horizLabelOffs);

	this->drawMoment_Nbr1 =
		CarouselMoment(
			UIRect(2.0f, 2.0f, 16.0f, 16.0f),
			UIColor4(0.35f, 0.35f, 0.35f, 1.0f),
			UIColor4(1.0f, 1.0f, 1.0f, 0.25f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			0.0f, 8.0f, 90.0f, 2.0f, 2.0f, 0.0f,
			horizLabelOffs);

	this->drawMoment_Rest =
		CarouselMoment(
			UIRect(2.0f, 2.0f, 16.0f, 16.0f),
			UIColor4(0.2f, 0.2f, 0.2f, 1.0f),
			UIColor4(1.0f, 1.0f, 1.0f, 0.2f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			0.0f, 9.0f, 90.0f, 0.0f, 0.0f, 0.0f,
			horizLabelOffs);
}

void CarouselStyle::SetDefaultMomentsVertical()
{
	// Offset of where labels are placed,
	// for compressed entries.
	UIVec2 vertLabelOffs(25, 15);

	this->drawMoment_Active = 
		CarouselMoment( 
			UIRect(10.0f, 10.0f, 80.0f, 80.0f), // Interior icon
			UIColor4(1.0f, 1.0f, 1.0f, 1.5f),	// Background color
			UIColor4(1.0f, 1.0f, 1.0f, 1.0),	// Icon color
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),	// Text color
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),	// Outline color
			3.0f, 14, 0.0f, 0.0f, 0.0f, 0.0f,	// Thickness / LabelSz / LabelRot / PlateInd / PushEmphasis / Trim
			UIVec2(40, 105.0f));

	this->drawMoment_Nbr0 =
		CarouselMoment(
			UIRect(5.0f, 2.0f, 16.0f, 16.0f),
			UIColor4(0.5f, 0.5f, 0.5f, 1.0f),
			UIColor4(1.0f, 1.0f, 1.0f, 0.5f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			1.0f, 8.0f, 0.0f, 5.0f, 0.0f, 2.0f,
			vertLabelOffs);

	this->drawMoment_Nbr1 =
		CarouselMoment(
			UIRect(7.0f, 2.0f, 16.0f, 16.0f),
			UIColor4(0.35f, 0.35f, 0.35f, 1.0f),
			UIColor4(1.0f, 1.0f, 1.0f, 0.25f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			0.0f, 8.0f, 0.0f, 2.0f, 0.0f, 4.0f,
			vertLabelOffs);

	this->drawMoment_Rest =
		CarouselMoment(
			UIRect(8.0f, 2.0f, 16.0f, 16.0f),
			UIColor4(0.2f, 0.2f, 0.2f, 1.0f),
			UIColor4(1.0f, 1.0f, 1.0f, 0.2f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			UIColor4(0.0f, 0.0f, 0.0f, 1.0f),
			0.0f, 9.0f, 0.0f, 0.0f, 0.0f, 5.0f,
			vertLabelOffs);
}

void Carousel::Clear()
{
	this->entries.clear();
	this->currentEntry = 0;
	this->currentShown = 0.0f;
	this->hasLoaded = false;
}

bool Carousel::LoadAssets(bool force)
{
	int fails = 0;
	for(int i = 0; i < this->entries.size(); ++i)
	{
		this->entries[i].cachedIndex = i;

		if(this->entries[i].IsImageLoaded() && !force)
			continue;

		TexObj::SPtr load = 
			TexObj::MakeSharedLODE(this->entries[i].iconFilepath);

		if(load == nullptr)
		{
			++fails;
			continue;
		}

		this->entries[i].icon = load;
	}

	this->labelFont = FontMgr::GetInstance().GetFont(10);

	return (fails == 0);
}


void Carousel::EndAnimation(const CarouselStyle& style, bool updateCache)
{
	this->currentShown = (int)this->currentEntry;

	if(updateCache)
		this->UpdateAndRecacheScene(style);
}

void Carousel::UpdateAndRecacheScene(const CarouselStyle& style)
{
	if(style.GetOrientation() == CarouselStyle::Orientation::Vertical)
		this->_UpdateAndRecacheScene_Vert(style);
	else
		this->_UpdateAndRecacheScene_Horiz(style);
}

void Carousel::_UpdateAndRecacheScene_Horiz(const CarouselStyle& style)
{
	if(this->entries.empty())
		return;

	// TODO: These moments should probably be defined and
	// referenced from the CarouselStyle class.
	//
	// The drawing properties of various distances from being the active state.
	// TODO: Generate these interpolation targets programatically 
	// base off the style or some other data.

	float totWidth = style.boxExpandedWidth + (this->entries.size() - 1) * style.boxComprDim;	
	float fy = -style.boxExpandedHeight * 0.5f;
	float fx = -totWidth * 0.5f;

	for(int i = 0; i < this->entries.size(); ++i)
	{
		Entry& e = this->entries[i];
		float sDst = e.cachedIndex  - this->currentShown;	// Signed
		float uDst = abs(sDst);								// Unsigned

		// Peicewise interpolation of all the draw details
		if(uDst < 1.0f)
			e.drawDetails = CarouselMoment::Lerp(style.drawMoment_Active, style.drawMoment_Nbr0, fmod(uDst, 1.0f));
		else if(uDst < 2.0f)
			e.drawDetails = CarouselMoment::Lerp(style.drawMoment_Nbr0, style.drawMoment_Nbr1, fmod(uDst, 1.0f));
		else if(uDst < 3.0f)
			e.drawDetails = CarouselMoment::Lerp(style.drawMoment_Nbr1, style.drawMoment_Rest, fmod(uDst, 1.0f));
		else
			e.drawDetails = style.drawMoment_Rest;

		float eWidth = style.boxComprDim;
		if(uDst < 1.0f)
			eWidth = ::Lerp(style.boxExpandedWidth, style.boxComprDim, uDst);

		e.clientRect.Set(fx, fy - e.drawDetails.pushEmphasis, eWidth, style.boxExpandedHeight);

		if(sDst == 0.0f)
			e.plateRect = e.clientRect;
		else if(sDst < 0.0f) // If to the left, shift plate to the right
		{ 
			e.plateRect.Set(
				e.clientRect.pos.x, 
				e.clientRect.pos.y + e.drawDetails.trim, 
				e.clientRect.dim.x + e.drawDetails.indentIn, 
				e.clientRect.dim.y - e.drawDetails.trim * 2.0f);
		}
		else // else if right, shift left
		{
			e.plateRect.Set(
				e.clientRect.pos.x - e.drawDetails.indentIn, 
				e.clientRect.pos.y + e.drawDetails.trim, 
				e.clientRect.dim.x + e.drawDetails.indentIn, 
				e.clientRect.dim.y - e.drawDetails.trim * 2.0f);
		}

		fx += eWidth;
	}
}

void Carousel::_UpdateAndRecacheScene_Vert(const CarouselStyle& style)
{
	if(this->entries.empty())
		return;

	// TODO: These moments should probably be defined and
	// referenced from the CarouselStyle class.
	//
	// The drawing properties of various distances from being the active state.
	// TODO: Generate these interpolation targets programatically 
	// base off the style or some other data.

	float totHeight = style.boxExpandedHeight + (this->entries.size() - 1) * style.boxComprDim;	
	float fx = -style.boxExpandedWidth * 0.5f;
	float fy = -totHeight * 0.5f;

	for(int i = 0; i < this->entries.size(); ++i)
	{
		Entry& e = this->entries[i];
		float sDst = e.cachedIndex  - this->currentShown;	// Signed
		float uDst = abs(sDst);								// Unsigned

															// Peicewise interpolation of all the draw details
		if(uDst < 1.0f)
			e.drawDetails = CarouselMoment::Lerp(style.drawMoment_Active, style.drawMoment_Nbr0, fmod(uDst, 1.0f));
		else if(uDst < 2.0f)
			e.drawDetails = CarouselMoment::Lerp(style.drawMoment_Nbr0, style.drawMoment_Nbr1, fmod(uDst, 1.0f));
		else if(uDst < 3.0f)
			e.drawDetails = CarouselMoment::Lerp(style.drawMoment_Nbr1, style.drawMoment_Rest, fmod(uDst, 1.0f));
		else
			e.drawDetails = style.drawMoment_Rest;

		float eHeight = style.boxComprDim;
		if(uDst < 1.0f)
			eHeight = ::Lerp(style.boxExpandedHeight, style.boxComprDim, uDst);

		e.clientRect.Set(fx - e.drawDetails.pushEmphasis, fy, style.boxExpandedWidth, eHeight);

		if(sDst == 0.0f)
			e.plateRect = e.clientRect;
		else if(sDst < 0.0f)
		{ 
			e.plateRect.Set(
				e.clientRect.pos.x + e.drawDetails.trim, 
				e.clientRect.pos.y, 
				e.clientRect.dim.x - e.drawDetails.trim * 2.0f, 
				e.clientRect.dim.y + e.drawDetails.indentIn);
		}
		else // else if right, shift left
		{
			e.plateRect.Set(
				e.clientRect.pos.x + e.drawDetails.trim, 
				e.clientRect.pos.y - e.drawDetails.indentIn, 
				e.clientRect.dim.x - e.drawDetails.trim * 2.0f, 
				e.clientRect.dim.y + e.drawDetails.indentIn);
		}

		fy += eHeight;
	}

	// If we're centered instead of stable, Calculate where our
	// center item is, and move EVERYTHING so that everything is
	// still placed relatively the same, but the seleted is now
	// at the origin.
	if(style.center && this->entries.size() > 1)
	{
		// Calculate center, respecting that fact that the "selection" may
		// actually be between entries, that themselves are moving (animating)
		// if this is happening during a transition animation.
		int idxA = (int)std::floor(this->currentShown);
		int idxB = std::min((int)this->entries.size() - 1, (int)std::ceil(this->currentShown));

		Entry& entA = this->entries[idxA];
		Entry& entB = this->entries[idxB];
		float mod = (float)fmod(this->currentShown, 1.0);

		float yOffs = 
			::Lerp(
				entA.clientRect.Center().y, 
				entB.clientRect.Center().y, 
				mod);

		for(Entry& e : this->entries)
		{ 
			e.plateRect.pos.y -= yOffs;
			e.clientRect.pos.y -= yOffs;
		}
	}
}

void Carousel::Update(const CarouselStyle& style, float dx)
{
	if((float)this->currentEntry == this->currentShown)
		return;

	
	if(this->currentEntry > this->currentShown)
	{	// If we need to animate up
		this->currentShown = 
			std::min(
				(float)this->currentEntry,
				this->currentShown + dx * style.transitionSpeed);
	}
	else
	{
		// else down
		this->currentShown = 
			std::max(
				(float)this->currentEntry,
				this->currentShown - dx * style.transitionSpeed);
	}
}

void Carousel::ProcessUpdateAndDrawOrder(
	float x, 
	float y, 
	const CarouselStyle& style, 
	float scale,
	std::vector<Entry*>& drawOrder)
{
	for(int i = 0; i < this->entries.size(); ++i)
		drawOrder.push_back(&this->entries[i]);

	if(CheckCache(this->currentShown, this->lastShown))
		this->UpdateAndRecacheScene(style);

	std::sort(
		drawOrder.begin(), 
		drawOrder.end(), 
		[this](Entry* ea, Entry* eb)
		{
			return abs(ea->cachedIndex - this->currentShown) > abs(eb->cachedIndex - this->currentShown);
		});
}


void Carousel::Render(
	float x, 
	float y, 
	const CarouselStyle& style, 
	float scale)
{

	// There are 4 different layers that need to be handled, depending
	// on their distance from the active animation point.
	//
	// Keep in mind a point's location isn't dependent on its index and
	// the current selected index, but also the moment in the animation
	// we're in.
	//
	// Depending on which layer an entry is categorized to be in, will
	// also change its Z-order, which means will affect at what phase
	// we render it so we can painter's algorithm it correct in respect
	// to its neighboring entries.

	std::vector<Entry*> order;
	this->ProcessUpdateAndDrawOrder( x, y, style, scale, order);


	for(Entry* e : order)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		float dst = e->cachedIndex - this->currentShown;
		float udst = abs(dst);

		glColor4fv(e->drawDetails.bgColor.ar);
		e->plateRect.GLQuad(UIVec2(x, y));

		// RENDER THE ICON
		//////////////////////////////////////////////////
		if(e->IsImageLoaded())
		{ 
			glEnable(GL_TEXTURE_2D);
			glColor4fv(e->drawDetails.iconColor.ar);
			e->icon->GLBind();

			UIRect rectIco(
				x + e->clientRect.pos.x + e->drawDetails.relIcon.pos.x,
				y + e->clientRect.pos.y + e->drawDetails.relIcon.pos.y,
				e->drawDetails.relIcon.dim.x,
				e->drawDetails.relIcon.dim.y);
			//
			rectIco.GLQuadTex();
		}

		// RENDER THE TEXT
		//////////////////////////////////////////////////
		glColor3fv(e->drawDetails.textColor.ar);
		glPushMatrix();
		glTranslatef(
			x + e->clientRect.pos.x + e->drawDetails.labelRelPos.x, 
			y + e->clientRect.pos.y + e->drawDetails.labelRelPos.y,
			0.0f);
		glScalef(1.0f, -1.0f, 1.0f);
		glRotatef(e->drawDetails.labelRot, 0.0f, 0.0f, 1.0f);
		this->labelFont._RenderFontRaw(e->label.c_str());
		glPopMatrix();

		// RENDER THE OUTLINE
		//////////////////////////////////////////////////
		if(e->drawDetails.outlineThickness >= 1.0f)
		{
			float olw = e->drawDetails.outlineThickness;
			UIVec2 plTL(x + e->plateRect.pos.x, y + e->plateRect.pos.y);
			UIVec2 plBR(plTL.x + e->plateRect.dim.x, plTL.y + e->plateRect.dim.y);
			UIVec2 outlTL(plTL.x - olw, plTL.y - olw);
			UIVec2 outlBR(plBR.x + olw, plBR.y + olw);

			glColor4fv(e->drawDetails.outlineColor.ar);
			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
			glVertex2f( plTL.x,		plTL.y	);	
			glVertex2f( outlTL.x,	outlTL.y);	
			glVertex2f( outlBR.x,	outlTL.y); 
			glVertex2f( plBR.x,		plTL.y	);
			//
			glVertex2f( plBR.x,		plTL.y	);	
			glVertex2f( outlBR.x,	outlTL.y);	
			glVertex2f( outlBR.x,	outlBR.y); 
			glVertex2f( plBR.x,		plBR.y	);
			//
			glVertex2f( plBR.x,		plBR.y	);	
			glVertex2f( outlBR.x,	outlBR.y);	
			glVertex2f( outlTL.x,	outlBR.y); 
			glVertex2f( plTL.x,		plBR.y	);
			//
			glVertex2f( plTL.x,		plBR.y	);	
			glVertex2f( outlTL.x,	outlBR.y);	
			glVertex2f( outlTL.x,	outlTL.y); 
			glVertex2f( plTL.x,		plTL.y  );
			glEnd();
		}
	}
}

bool Carousel::Goto(int idx, bool anim)
{
	bool anyChange = false;
	if(this->currentEntry != idx)
	{ 
		this->currentEntry = idx;
		anyChange = true;
	}

	if(this->currentShown != this->currentEntry)
	{
		anyChange = true;

		// If we're animating, don't mess with currentShown so 
		// that the carousel can animate to the correct position
		// from its current state. Else, instantly snap its visual
		// position so no animation is needed.
		if(!anim)
			this->currentShown = this->currentEntry;
	}
	return anyChange;
}

bool Carousel::GotoNext(bool anim)
{
	if(this->currentEntry >= this->entries.size() - 1)
		return false;

	++this->currentEntry;
	if(anim == false)
		this->currentShown = this->currentEntry;

	return true;
}

bool Carousel::GotoPrev(bool anim)
{
	if(this->currentEntry == 0)
		return false;

	--this->currentEntry;
	if(anim == false)
		this->currentShown = this->currentEntry;

	return true;
}

bool Carousel::Goto(const std::string& id, bool anim)
{
	for(int i = 0; i < this->entries.size(); ++i)
	{
		if(this->entries[i].id == id)
		{
			return this->Goto(i, anim);
			break;
		}
	}
	return false;
}

bool Carousel::Append(std::vector<CarouselData>& vec)
{
	if(this->hasLoaded)
		return false;

	for(const CarouselData& cd : vec)
		this->Append(cd);

	return true;
}

bool Carousel::Append(const CarouselData& cd)
{
	if(this->hasLoaded)
		return false;

	return this->Append(
		cd.id, 
		cd.iconFilepath, 
		cd.label, 
		cd.caption);

	return true;
}

bool Carousel::Append(
	const std::string& id, 
	const std::string& iconFilepath, 
	const std::string& label,
	const std::string& caption)
{
	if(this->hasLoaded)
		return false;

	this->entries.push_back(Entry(id, iconFilepath, label, caption));

	return true;
}

Carousel::Entry::Entry()
{}

Carousel::Entry::Entry(
	const std::string& id, 
	const std::string& iconFilepath,
	const std::string& label,
	const std::string& caption)
	: CarouselData(id, iconFilepath, label, caption)
{
}

std::string Carousel::GetCurrentLabel() const
{
	if(this->entries.empty())
		return "";

	return this->entries[this->currentEntry].label;
}

std::string Carousel::GetCurrentCaption() const
{
	if(this->entries.empty())
		return "";

	return this->entries[this->currentEntry].caption;
}

int Carousel::GetCurrentIndex() const
{
	if(this->entries.empty())
		return -1;

	return this->currentEntry;
}

CarouselData Carousel::GetCurrentData() const
{
	if(this->entries.empty())
		return CarouselData("", "", "", "");

	return this->entries[this->currentEntry];
}

bool Carousel::AtStart() const
{
	return this->currentEntry <= 0;
}

bool Carousel::AtEnd() const
{
	return 
		this->entries.empty() ||
		this->currentEntry >= this->entries.size() - 1;
}