#include "CodeEditField.h"

#include "tb_widgets_reader.h"

using namespace tb;
TB_WIDGET_FACTORY(CodeEditField, TBValue::TYPE_STRING, WIDGET_Z_TOP) {}
void CodeEditField::OnInflate(const INFLATE_INFO &info)
{
	TBEditField::OnInflate(info);
}

void CodeEditField::DrawString(int32 x, int32 y, TBFontFace *font, const TBColor &color, const char *str, int32 len)
{
	TBColor finalColor(color);
	if (strncmp(str, "code", len) == 0)
	{
		finalColor = TBColor(255, 0, 208);
	}
	TBEditField::DrawString(x, y, font, finalColor, str, len);
}
