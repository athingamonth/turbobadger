#include "CodeEditField.h"

#include "tb_widgets_reader.h"

using namespace tb;
TB_WIDGET_FACTORY(CodeEditField, TBValue::TYPE_STRING, WIDGET_Z_TOP) {}
void CodeEditField::OnInflate(const INFLATE_INFO &info)
{
	TBEditField::OnInflate(info);
}
