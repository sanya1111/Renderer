#ifndef RENDERER_H_
#define RENDERER_H_

#include <gtkmm.h>
#include <iostream>

class PrimitiveRenderer{
public:
	virtual ~PrimitiveRenderer() {}
	virtual int startSession() = 0;
	virtual void drawPixel(int x, int y) = 0;
	virtual int getFPS() = 0;
	virtual PrimitiveRenderer& windowHide() = 0;
	virtual PrimitiveRenderer& windowShow() = 0;
	virtual PrimitiveRenderer& windowFullScreenMode(bool on = false) = 0;
	virtual PrimitiveRenderer& windowResize(int width, int height) = 0;
};

class GtkPrimitiveRenderer:public PrimitiveRenderer, public sigc::trackable{
	Glib::RefPtr<Gtk::Application> app;
	Glib::RefPtr<Gdk::Pixbuf> display;
	Gt
//	Gtk::DrawingArea inner_drawing_area;
	bool child_running;
	int inner_fps;
	sigc::connection render_cycle_connect;
	class innerWindow: public Gtk::Window{
	public:
		bool on_timeout();
	}window;
	int bitch;
	pthread_t child_thread;
	pthread_mutex_t p_mutex;
	pthread_cond_t p_condition;

	class MyDrawingArea :public Gtk::DrawingArea{
	public:
		bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr){
			std::cerr << "Draw" << std::endl;
		}
	}inner_drawing_area;

	static void * childStartPoint(void *arg);
	void childStartAppNotify(void);
public:
	GtkPrimitiveRenderer(int window_width, int window_height, int argc, char **argv);
	virtual ~GtkPrimitiveRenderer();
	virtual int startSession() ;
	virtual void drawPixel(int x, int y) ;
	virtual int getFPS() ;
	virtual void changeFps(int);
	virtual GtkPrimitiveRenderer& windowHide() ;
	virtual GtkPrimitiveRenderer& windowShow() ;
	virtual GtkPrimitiveRenderer& windowFullScreenMode(bool on = false);
	virtual GtkPrimitiveRenderer& windowResize(int width, int height) ;
};



#endif /* RENDERER_H_ */
