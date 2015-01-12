#include "Demo.h"
#include "ListWindow.h"
#include "ResourceEditWindow.h"
#include <stdio.h>
#include <stdarg.h>
#include "tests/tb_test.h"
#include "tb_system.h"
#include "tb_inline_select.h"
#include "tb_select.h"
#include "tb_menu_window.h"
#include "tb_editfield.h"
#include "tb_tab_container.h"
#include "tb_bitmap_fragment.h"
#include "animation/tb_widget_animation.h"
#include "tb_node_tree.h"
#include "tb_tempbuffer.h"
#include "tb_font_renderer.h"
#include "image/tb_image_manager.h"
#include "utf8/utf8.h"

#include "CodeEditField\CodeEditField.h"

static Application *application;

AdvancedItemSource advanced_source;
TBGenericStringItemSource name_source;
TBGenericStringItemSource popup_menu_source;

// == DemoWindow ==============================================================

DemoWindow::DemoWindow()
{
	application->GetRoot()->AddChild(this);
	LoadResourceFile("Demo/demo01/ui_resources/test_codewindow.tb.txt");
}

bool DemoWindow::LoadResourceFile(const char *filename)
{
	// We could do g_widgets_reader->LoadFile(this, filename) but we want
	// some extra data we store under "WindowInfo", so read into node tree.
	TBNode node;
	if (!node.ReadFile(filename))
		return false;
	LoadResource(node);
	return true;
}

void DemoWindow::LoadResourceData(const char *data)
{
	// We could do g_widgets_reader->LoadData(this, filename) but we want
	// some extra data we store under "WindowInfo", so read into node tree.
	TBNode node;
	node.ReadData(data);
	LoadResource(node);
}

void DemoWindow::LoadResource(TBNode &node)
{
	g_widgets_reader->LoadNodeTree(this, &node);

	// Get title from the WindowInfo section (or use "" if not specified)
	SetText(node.GetValueString("WindowInfo>title", ""));

	const TBRect parent_rect(0, 0, GetParent()->GetRect().w, GetParent()->GetRect().h);
	const TBDimensionConverter *dc = g_tb_skin->GetDimensionConverter();
	TBRect window_rect = GetResizeToFitContentRect();

	// Use specified size or adapt to the preferred content size.
	TBNode *tmp = node.GetNode("WindowInfo>size");
	if (tmp && tmp->GetValue().GetArrayLength() == 2)
	{
		window_rect.w = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(0)->GetString(), window_rect.w);
		window_rect.h = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(1)->GetString(), window_rect.h);
	}

	// Use the specified position or center in parent.
	tmp = node.GetNode("WindowInfo>position");
	if (tmp && tmp->GetValue().GetArrayLength() == 2)
	{
		window_rect.x = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(0)->GetString(), window_rect.x);
		window_rect.y = dc->GetPxFromString(tmp->GetValue().GetArray()->GetValue(1)->GetString(), window_rect.y);
	}
	else
		window_rect = window_rect.CenterIn(parent_rect);

	// Make sure the window is inside the parent, and not larger.
	window_rect = window_rect.MoveIn(parent_rect).Clip(parent_rect);

	SetRect(window_rect);

	// Ensure we have focus - now that we've filled the window with possible focusable
	// widgets. EnsureFocus was automatically called when the window was activated (by
	// adding the window to the root), but then we had nothing to focus.
	// Alternatively, we could add the window after setting it up properly.
	EnsureFocus();
}

bool DemoWindow::OnEvent(const TBWidgetEvent &ev)
{
	if (ev.type == EVENT_TYPE_KEY_DOWN && ev.special_key == TB_KEY_ESC)
	{
		// We could call Die() to fade away and die, but click the close button instead.
		// That way the window has a chance of intercepting the close and f.ex ask if it really should be closed.
		TBWidgetEvent click_ev(EVENT_TYPE_CLICK);
		m_close_button.InvokeEvent(click_ev);
		return true;
	}
	return TBWindow::OnEvent(ev);
}
// ======================================================

int fps = 0;
uint32 frame_counter_total = 0;
uint32 frame_counter = 0;
double frame_counter_reset_time = 0;

bool DemoApplication::Init()
{
	if (!Application::Init())
		return false;

	new DemoWindow();

	return true;
}

void DemoApplication::RenderFrame(int window_w, int window_h)
{
	// Override RenderFrame without calling super, since we want
	// to inject code between BeginPaint/EndPaint.
	// Application::RenderFrame(window_w, window_h);

	// Render
	g_renderer->BeginPaint(window_w, window_h);
	GetRoot()->InvokePaint(TBWidget::PaintProps());

#if defined(TB_RUNTIME_DEBUG_INFO) && defined(TB_IMAGE)
	// Enable to debug image manager fragments
	//g_image_manager->Debug();
#endif

	frame_counter++;
	frame_counter_total++;

	// Update the FPS counter
	double time = TBSystem::GetTimeMS();
	if (time > frame_counter_reset_time + 1000)
	{
		fps = (int) ((frame_counter / (time - frame_counter_reset_time)) * 1000);
		frame_counter_reset_time = time;
		frame_counter = 0;
	}

	// Draw FPS
	TBWidgetValue *continuous_repaint_val = g_value_group.GetValue(TBIDC("continous-repaint"));
	bool continuous_repaint = continuous_repaint_val ? !!continuous_repaint_val->GetInt() : 0;

	TBStr str;
	if (continuous_repaint)
		str.SetFormatted("FPS: %d Frame %d", fps, frame_counter_total);
	else
		str.SetFormatted("Frame %d", frame_counter_total);
	GetRoot()->GetFont()->DrawString(5, 5, TBColor(255, 255, 255), str);

	g_renderer->EndPaint();

	// If we want continous updates or got animations running, reinvalidate immediately
	if (continuous_repaint || TBAnimationManager::HasAnimationsRunning())
		GetRoot()->Invalidate();
}

int app_main()
{
	application = new DemoApplication();

	ApplicationBackend *application_backend = ApplicationBackend::Create(application, 1280, 700, "Demo");
	if (!application_backend)
		return 1;

	tb_core_init(application_backend->GetRenderer(), "resources/language/lng_en.tb.txt");

	// Load the default skin, and override skin that contains the graphics specific to the demo.
	g_tb_skin->Load("resources/default_skin/skin.tb.txt", "Demo/demo01/skin/skin.tb.txt");

	// Register font renderers.
#ifdef TB_FONT_RENDERER_TBBF
	void register_tbbf_font_renderer();
	register_tbbf_font_renderer();
#endif
#ifdef TB_FONT_RENDERER_STB
	void register_stb_font_renderer();
	register_stb_font_renderer();
#endif
#ifdef TB_FONT_RENDERER_FREETYPE
	void register_freetype_font_renderer();
	register_freetype_font_renderer();
#endif

	// Add fonts we can use to the font manager.
#if defined(TB_FONT_RENDERER_STB) || defined(TB_FONT_RENDERER_FREETYPE)
	g_font_manager->AddFontInfo("resources/vera.ttf", "Vera");
#endif
#ifdef TB_FONT_RENDERER_TBBF
	g_font_manager->AddFontInfo("resources/default_font/segoe_white_with_shadow.tb.txt", "Segoe");
	g_font_manager->AddFontInfo("Demo/fonts/neon.tb.txt", "Neon");
	g_font_manager->AddFontInfo("Demo/fonts/orangutang.tb.txt", "Orangutang");
	g_font_manager->AddFontInfo("Demo/fonts/orange.tb.txt", "Orange");
#endif

	// Set the default font description for widgets to one of the fonts we just added
	TBFontDescription fd;
#ifdef TB_FONT_RENDERER_TBBF
	fd.SetID(TBIDC("Segoe"));
#else
	fd.SetID(TBIDC("Vera"));
#endif
	fd.SetSize(g_tb_skin->GetDimensionConverter()->DpToPx(14));
	g_font_manager->SetDefaultFontDescription(fd);

	// Create the font now.
	TBFontFace *font = g_font_manager->CreateFontFace(g_font_manager->GetDefaultFontDescription());

	// Render some glyphs in one go now since we know we are going to use them. It would work fine
	// without this since glyphs are rendered when needed, but with some extra updating of the glyph bitmap.
	if (font)
		font->RenderGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~•·åäöÅÄÖ");

	// Give the root widget a background skin
	application->GetRoot()->SetSkinBg(TBIDC("background"));

	application->Init();
	application->Run();
	application->ShutDown();

	delete application;

	return 0;
}
