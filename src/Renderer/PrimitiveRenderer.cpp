#include "Renderer/PrimitiveRenderer.h"
#include "pthread.h"
#include <iostream>

/*
 class fucker:public Gtk::DrawingArea{
public:
	fucker() {}
	~fucker() {}
protected:
	virtual bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr){
		// Get the window size
			int winWidth=get_allocated_width();
			int winHeight=get_allocated_height();

			// Get the image size

			// If requested, reset view
			// Create a new image for display filled with grey
			Glib::RefPtr<Gdk::Pixbuf> display = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8,winWidth,winHeight);

//			display->
//			pixels[0] = 2;
			display->fill(0x0F00FFF0000);
			for(int i = 0; i < display->get_height(); i++)
				for(int j = 0; j < display->get_width(); j++)
				{
					 int offset = i*display->get_rowstride() + j*display->get_n_channels();
					guchar * pixel = &display->get_pixels()[ offset ]; // get pixel pointer
					pixel[1] = 1;
				}

			// Compute offset of the source image
			Gdk::Cairo::set_source_pixbuf(cr,display,0,0);
			cr->rectangle(0,0,winWidth,winHeight);
			cr->fill();

			// Display the center target if requested

			// Event has been handled
			return false;
	}
};

int Startup::start(int argc, char **argv)
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "www.lucidarme.me");
	Gtk::Window window;

	// Create the drawing and add an image from file
	fucker Img;

	// Insert the drawing in the window
	window.add(Img);
	// Resize the window
	window.resize(600,400);
	// Set the window title
	// Show the drawing
	(Img).show();

	// Start main loop
	return app->run(window);
}
*/



void GtkPrimitiveRenderer::drawPixel(int x, int y)
{
//	Glib::RefPtr<Gdk::Window> a = inner_drawing_area.get_parent_window();
//	Cairo::RefPtr<Cairo::Context> cr = a->create_cairo_context();
////	std::cerr << "here" << std::endl;
//	display->fill(0x0F00FFF0000);
//	Gdk::Cairo::set_source_pixbuf(cr,display,0,0);

}

int GtkPrimitiveRenderer::getFPS()
{
	return inner_fps;
}

GtkPrimitiveRenderer& GtkPrimitiveRenderer::windowHide()
{
	window.hide();
	return *this;
}

GtkPrimitiveRenderer& GtkPrimitiveRenderer::windowShow()
{
	window.show();
	return *this;
}

GtkPrimitiveRenderer& GtkPrimitiveRenderer::windowFullScreenMode(bool on)
{
	if(on)
		window.fullscreen();
	else
		window.unfullscreen();
	return *this;
}

GtkPrimitiveRenderer::GtkPrimitiveRenderer(int window_width, int window_height, int argc, char **argv) : app(Gtk::Application::create(argc, argv)),
		window(), inner_drawing_area(), child_running(false), child_thread(0), inner_fps(1000)
{
	window.resize(window_width, window_height);
	window.add(inner_drawing_area);
	display = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB,false,8, window_width, window_height);
	pthread_mutex_init(&p_mutex, NULL);
	pthread_cond_init(&p_condition, NULL);
	render_cycle_connect = Glib::signal_timeout().connect(sigc::mem_fun(window, &GtkPrimitiveRenderer::innerWindow::on_timeout), inner_fps);
	app->signal_activate().connect(sigc::mem_fun(*this, &GtkPrimitiveRenderer::childStartAppNotify));
	inner_drawing_area.show();
}

int GtkPrimitiveRenderer::startSession()
{
	pthread_mutex_lock(&p_mutex);
	int rc = pthread_create(&child_thread, NULL, childStartPoint, (void *)this);
	if(rc){
		child_running = false;
	}else{
		pthread_cond_wait(&p_condition, &p_mutex);
	}
	pthread_mutex_unlock(&p_mutex);
	return rc == 0;
}

GtkPrimitiveRenderer::~GtkPrimitiveRenderer()
{
	if(child_running){
		pthread_join(child_thread, (void ** )NULL);
	}
}

void* GtkPrimitiveRenderer::childStartPoint(void* arg_void)
{
	GtkPrimitiveRenderer * arg = (GtkPrimitiveRenderer *)arg_void;
	std::cerr << "starting application" << std::endl;
	arg->app->run(arg->window);
	std::cerr << "ending application" << std::endl;
	pthread_mutex_lock(&arg->p_mutex);
	arg->child_running = false;
	pthread_cond_broadcast(&arg->p_condition);
	pthread_mutex_unlock(&arg->p_mutex);
}

void GtkPrimitiveRenderer::childStartAppNotify(void)
{
	pthread_mutex_lock(&p_mutex);
	child_running = true;
	pthread_cond_broadcast(&p_condition);
	pthread_mutex_unlock(&p_mutex);
}

void GtkPrimitiveRenderer::changeFps(int new_fps)
{
	render_cycle_connect.disconnect();
	inner_fps = new_fps;
	render_cycle_connect = Glib::signal_timeout().connect(sigc::mem_fun(window, &GtkPrimitiveRenderer::innerWindow::on_timeout), inner_fps);
}

GtkPrimitiveRenderer& GtkPrimitiveRenderer::windowResize(int width, int height)
{
	window.resize(width, height);
	return *this;
}

bool GtkPrimitiveRenderer::innerWindow::on_timeout()
{
}
