#include "ExpandFrame.h"

void ExpanderCtrl::Layout()
{
	scroll.SetPage(Hv(GetSize()));
	UpdateScrollBar();
}

void ExpanderCtrl::ChildMouseEvent(Ctrl *child, int event, Point p, int zdelta, dword keyflags)
{
	int i = -1;
	if (event == MOUSEWHEEL)
		scroll.Wheel(zdelta);
}

ExpandFrame & ExpanderCtrl::AddExpander(Ctrl &c, bool expand, int size)
{
	int sz = (size < 0) ? Hv(c.GetMinSize()) 
					    : (IsHorz() ? HorzLayoutZoom(size) : VertLayoutZoom(size));
	ExpandFrame &e = exp.Add();
	e.Set(c, sz, IsHorz() ? ExpandFrame::LEFT : ExpandFrame::TOP).SetStyle(*style);
	if (IsHorz()) {
		e.VSizePos();
		e.LeftPos(0, 0);
	}
	else {
		e.HSizePos();
		e.TopPos(0, 0);
	}
	e <<= THISBACK(OnExpand);
	Add(e);
	e.Expand(expand);
	e.NoWantFocus();
	Repos();
	if (GetCount() == 1) scroll.SetLine(Hv(e.GetMinSize()));
	return e;
}

int	ExpanderCtrl::GetNumber(Ctrl &c){
	for(int i = 0; i < exp.GetCount(); i++) {
		if (exp[i].addr == &c) return i;
	}
	return -1;
}

ExpandFrame& ExpanderCtrl::Expand(Ctrl &c, bool _expand){
	int n = GetNumber(c);
	if (n>=0) {
		Get(n).Expand(_expand);
	}
}

ExpanderCtrl & ExpanderCtrl::Horz(bool v)
{
	ASSERT(!GetCount()); // Don't call unless empty!
	horz = v;
	scroll.Horz(v);
	return *this;
}

void ExpanderCtrl::UpdateScrollBar()
{
	Size sz(0, 0);
	for (int i = 0; i < GetCount(); i++)
		sz += exp[i].GetStdSize();
	scroll.SetTotal(Hv(sz));
}

void ExpanderCtrl::OnScroll()
{
	Repos();
	scroller.Scroll(*this, scroll.Get());
}

void ExpanderCtrl::OnExpand()
{
	int p = scroll.Get();
	Repos();
	if (p != scroll.Get())
		OnScroll();
}

void ExpanderCtrl::Repos()
{
	if (!GetCount()) return;
	Rect r = GetSize();
	Size sz = r.GetSize();
	int &var = IsHorz() ? sz.cx : sz.cy;
	int total = 0;
	Point off = IsHorz() ? Point(-scroll.Get(), 0) : Point(0, -scroll.Get());
	for (int i = 0; i < GetCount(); i++) {
		ExpandFrame &e = exp[i];
		int intsz = Hv(e.GetStdSize()); 
		total += intsz;
		var = intsz;
		e.SetRect(Rect(off, sz));
		if (IsHorz())
			off.x += intsz;
		else
			off.y += intsz;
	}
	scroll.SetTotal(total);
}
void ExpanderCtrl::ChildGotFocus() {
	Ctrl::ChildGotFocus();
	lastfocus = GetFocusCtrl();
	if (followfocus)
		ScrollToActiveField();
}

void ExpanderCtrl::ScrollToActiveField() {
	const int topdist = 60;
	const int bottomdist = 60;
	if ((lastfocus->GetScreenRect().bottom - GetScreenRect().top) + bottomdist > (GetScrollPage())) {
		SetScrollPos((lastfocus->GetScreenRect().bottom - GetScreenRect().top) + bottomdist -  GetScrollPage() + GetScrollPos() );
	}
	if ((lastfocus->GetScreenRect().top - GetScreenRect().top) - topdist < 0) {
		SetScrollPos((lastfocus->GetScreenRect().top - GetScreenRect().top) - topdist + GetScrollPos());
	}
};

ExpanderCtrl::ExpanderCtrl()
{
	style = &ExpandFrame::StyleDefault();
	Vert();
	AddFrame(scroll);
	scroll <<= THISBACK(OnScroll);
	scroll.AutoHide(true);
	followfocus = true;
}

