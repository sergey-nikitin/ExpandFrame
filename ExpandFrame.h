#ifndef _FrameTest_ExpandFrame_h_
#define _FrameTest_ExpandFrame_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#define TAB(parentCtrlName,boolExpand,charTitle) CtrlLayout(parentCtrlName);\
	parentCtrlName.NoWantFocus();\
	expander.AddExpander(parentCtrlName, boolExpand).SetTitle(charTitle); 


class ExpandFrame : public CtrlFrame, public Ctrl {
public:
	typedef ExpandFrame CLASSNAME;
	virtual void FrameAdd(Ctrl& parent);
	virtual void FrameRemove();
	virtual void FrameAddSize(Size& sz);
	virtual void FrameLayout(Rect& r);
	virtual void Paint(Draw& draw);
	
	
	void PaintBorder(Draw& w);
	
	virtual void ChildGotFocus();
	virtual void ChildLostFocus();
public:	
	struct Style : ChStyle<Style> {
		Image	btnimage[4]; // Left, Up, Right, Down
		Value	background[2];
		Color	text[2];
		Color	border[2];
		Font	font;
		bool 	buttoncenter;
		int		buttonstyle;
		int		borderwidth;
	};	
	static const Style& StyleDefault();	
	static const Style& StylePlain();
	static const Style& StylePlainSV();
	static const Style& StyleButton();
	Ctrl*			addr;
		
private:
	friend struct ImgButton;
	struct ImgButton : public FlatButton
	{
		enum { LEFT, TOP, RIGHT, BOTTOM };
		virtual void Paint(Draw &w);
	};
	
	ImgButton		btn;
	Image			img;
	WString			title;
	int				type;
	int				child_size;
	bool			expand:1;
	bool			childfocus:1;
	bool			ignorefocus:1;
	bool			hasarrow:1;
	
	const Style *	style;	
	const Style *	St() const					{ return style ? style : &StyleButton(); }		
	
	int 			TitleSz() const				{ return St()->font.GetHeight()+4; }
	int				ExpandSz() const			{ return expand ? child_size+1 : 0; }
	
	void			UpdateButton();
	void			SetChildPos();
	bool			IsSet()	const				{ return GetLastChild() != &btn; }
	void 			Expand0(bool _expand)		{ if (_expand != expand) { Expand(_expand); Action(); } }
public:
	enum { LEFT, TOP, RIGHT, BOTTOM };

	ExpandFrame & 	SetStyle(const Style &s)		{ style = &s; UpdateButton(); Refresh(); return *this; }

	ExpandFrame & 	SetTitle(const char *_title) 	{ return SetTitle((WString)_title); }
	ExpandFrame & 	SetTitle(const WString &_title) { title = _title; Refresh(); return *this; }
	const WString &	GetTitle() const				{ return title; }

	ExpandFrame &	SetImage(const Image &image)	{ img = image; Refresh(); return *this; }
	const Image &	GetImage() const				{ return img; }
	void			ChangeFrameSize(int s)			{ child_size = s; }

	ExpandFrame& Set(Ctrl& c, int size, int _type);
	ExpandFrame& Left(Ctrl& c, int size = -1)   { return Set(c, (size < 0) ? c.GetMinSize().cx : HorzLayoutZoom(size), LEFT); }
	ExpandFrame& Top(Ctrl& c, int size = -1)    { return Set(c, (size < 0) ? c.GetMinSize().cy : VertLayoutZoom(size), TOP); }
	ExpandFrame& Right(Ctrl& c, int size = -1)  { return Set(c, (size < 0) ? c.GetMinSize().cx : HorzLayoutZoom(size), RIGHT); }
	ExpandFrame& Bottom(Ctrl& c, int size = -1) { return Set(c, (size < 0) ? c.GetMinSize().cy : VertLayoutZoom(size), BOTTOM); }
	ExpandFrame& SetStyle(const Style *_style) 	{ style = _style; SetChildPos(); UpdateButton(); RefreshParentLayout(); return *this; }

	ExpandFrame& IgnoreFocus(bool ignore = true) { ignorefocus = ignore; if (!childfocus) Refresh(); }

	ExpandFrame& Expand(bool _expand = true);
	ExpandFrame& Collapse()						{ return Expand(false); }
	bool 		 IsExpanded() const				{ return expand; }
	
	virtual Size GetMinSize() const;
	virtual Size GetStdSize() const;
	
	ExpandFrame& Arrow(bool _arrow = true)		{ hasarrow = _arrow; btn.Refresh(); }
	
	void Show(bool show = true)				  	{ Ctrl::Show(show); }
	void Hide()								  	{ Ctrl::Hide(); }
	bool IsShown() const					  	{ return Ctrl::IsShown(); }
	int focus;
	ExpandFrame();
};

class ExpanderCtrl : public ParentCtrl
{
public:
	virtual void Layout();					
	virtual void MouseWheel(Point p, int zdelta, dword keyflags) { scroll.Wheel(zdelta); }
	virtual void ChildMouseEvent(Ctrl *child, int event, Point p, int zdelta, dword keyflags);

private:
	Array<ExpandFrame> 		exp;
	ScrollBar 				scroll;
	Scroller				scroller;
	bool					followfocus;
	bool 					horz;
	const ExpandFrame::Style *style;
	
	bool 	IsHorz() const					{ return horz; }
	int		Hv(const Size &sz) const		{ return IsHorz() ? sz.cx : sz.cy; }
	void	UpdateScrollBar();
	void 	OnScroll();
	void 	OnExpand();
	void 	Repos();
public:
	void SetScrollPos(int pos){scroll.Set(pos); OnScroll();}
	int GetScrollPos(){return scroll.Get();}
	int GetScrollPage(){return (scroll.GetPage()); }
	int GetScrollTotal(){return (scroll.GetTotal()); }
	void ScrollToActiveField();
	virtual void ChildGotFocus();

	typedef ExpanderCtrl CLASSNAME;
	Ctrl *lastfocus;
	
	ExpandFrame &	AddExpander(Ctrl &c, int size = -1)		{ return AddExpander(c, true); }
	ExpandFrame &	AddExpander(Ctrl &c, bool expand, int size = -1);
	
	ExpandFrame&  	Get(int i)								{ return exp[i]; }
	const ExpandFrame&  Get(int i) const					{ return exp[i]; }
	int	GetNumber(Ctrl &c);
	ExpandFrame& Expand(Ctrl &c, bool _expand = true);
	ExpanderCtrl &	SetStyle(const ExpandFrame::Style &s)	{ style = &s; return *this; }
	ExpanderCtrl &	Horz(bool v = true);
	ExpanderCtrl &	Vert(bool v = true)						{ return Horz(!v); }
	
	void 			Clear()									{ exp.Clear(); scroller.Clear(); }
	int	 			GetCount()								{ return exp.GetCount(); }
	void 			DoNotFollowFocus( bool f = true) 		{ followfocus = f ? false : true; };
	
	ExpanderCtrl();
};

#endif
