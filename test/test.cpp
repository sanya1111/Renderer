#include "Renderer/PrimitiveRenderer.h"

int main(int argc, char** argv){
	GtkPrimitiveRenderer owowo(600, 400, argc, argv);
	int result =  owowo.startSession();
	owowo.windowResize(200, 200).windowHide().windowFullScreenMode(true).windowFullScreenMode(false);
	owowo.drawPixel(2, 2);
}
