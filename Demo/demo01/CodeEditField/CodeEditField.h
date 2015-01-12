#pragma once

#include "tb_editfield.h"

class CodeEditField : public tb::TBEditField
{
	virtual void OnInflate(const tb::INFLATE_INFO &info);

private:
	virtual void DrawString(tb::int32 x, tb::int32 y, tb::TBFontFace *font, const tb::TBColor &color, const char *str, tb::int32 len);
};
