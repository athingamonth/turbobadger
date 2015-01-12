#ifndef DEMO_H
#define DEMO_H

#include "tb_widgets.h"
#include "tb_widgets_common.h"
#include "tb_widgets_reader.h"
#include "tb_widgets_listener.h"
#include "tb_message_window.h"
#include "tb_msg.h"
#include "tb_scroller.h"
#include "platform/Application.h"

using namespace tb;

class DemoApplication : public Application
{
public:
	DemoApplication() : Application() {}

	virtual bool Init();
	virtual void RenderFrame(int window_w, int window_h);
};

class DemoWindow : public TBWindow
{
public:
	DemoWindow();
	bool LoadResourceFile(const char *filename);
	void LoadResourceData(const char *data);
	void LoadResource(TBNode &node);

	virtual bool OnEvent(const TBWidgetEvent &ev);
};

#endif // DEMO_H
