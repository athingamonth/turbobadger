#include "CodeEditField.h"

#include "tb_widgets_reader.h"
#include <ctype.h>

using namespace tb;
TB_WIDGET_FACTORY(CodeEditField, TBValue::TYPE_STRING, WIDGET_Z_TOP) {}
void CodeEditField::OnInflate(const INFLATE_INFO &info)
{
	TBEditField::OnInflate(info);
}

void CodeEditField::DrawString(int32 x, int32 y, TBFontFace *font, const TBColor &color, const char *str, int32 len)
{
	TBColor finalColor(color);
	StringHasColorOverride(str, len, finalColor);
	TBEditField::DrawString(x, y, font, finalColor, str, len);
}

bool CodeEditField::StringHasColorOverride(const char* str, int32 len, TBColor& colour)
{
	char* matchAgainst = "code";
	int32 matchLen = (int32)strlen(matchAgainst);
	if (matchLen != len)
	{
		return false;
	}

	for (int32 i = 0; i < len; i++)
	{
		if (toupper(matchAgainst[i]) != toupper(str[i]))
		{
			return false;
		}
	}

	colour = TBColor(255, 0, 208);
	return true;
}
