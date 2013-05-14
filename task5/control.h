#include <LEDA/graphics/window.h>

static leda::window *ControlWindowP;
static double ControlWindowSpeed;

void create_control() {
  ControlWindowP = new leda::window(260, 60, "Remote Control");
  //panel_item pi = ControlWindowP->int_item("Speed", ControlWindowSpeed, 95, 105, 1, "Animation Speed");
  ControlWindowP->buttons_per_line(3);
  ControlWindowP->button("Stop",  41);
  ControlWindowP->button("Step",  42);
  ControlWindowP->button("Cont",  43);
  ControlWindowP->button("Slow",  44);
  ControlWindowP->button("Normal",45);
  ControlWindowP->button("Fast",  46);
  ControlWindowSpeed = 1.0;
  ControlWindowP->display(0, 0);
}

void destroy_control() {
  ControlWindowP->close();
  delete ControlWindowP;
}

void control_wait(double t) {
  int val;
  double x, y;
  //while (ControlWindowP->get_event(val, x, y) != no_event) ;
  for (unsigned int i = 0; i < 20; i++) {
    val = ControlWindowP->get_mouse(x, y);
    //if (val != NO_BUTTON) std::cout << "ControlWindowP: " << val << std::endl;
    switch(val) {
      case 41: ControlWindowSpeed= 0.0; break;
      case 42: ControlWindowSpeed=-1.0; break;
      case 43: ControlWindowSpeed= 1.0; break;
      case 44: ControlWindowSpeed= 2.0; break;
      case 45: ControlWindowSpeed= 1.0; break;
      case 46: ControlWindowSpeed= 0.3; break;
    }
  }
  if (ControlWindowSpeed == 0.0) {
    while (ControlWindowSpeed == 0.0) {
      val = ControlWindowP->get_mouse(x,y);
      //if (val!=NO_BUTTON) std::cout << "ControlWindowP: " <<val << std::endl;
      switch(val) {
        case 41: ControlWindowSpeed= 0.0; break;
        case 42: ControlWindowSpeed=-1.0; break;
        case 43: ControlWindowSpeed= 1.0; break;
        case 44: ControlWindowSpeed= 2.0; break;
        case 45: ControlWindowSpeed= 1.0; break;
        case 46: ControlWindowSpeed= 0.3; break;
      }
    }
  }

  if (ControlWindowSpeed == -1.0) {
    ControlWindowSpeed = 0.0;
  } else {
#if __LEDA__ < 360
    wait     (t * ControlWindowSpeed);
#else
    leda_wait(t * ControlWindowSpeed);
#endif
  }
}
