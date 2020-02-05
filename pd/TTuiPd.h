#pragma once

#include "m_pd.h"

#include <memory>
#include <TTuiDevice.h>

typedef struct _TTuiPd {
    t_object x_obj;
    std::shared_ptr<TTuiLite::TTuiDevice> device_;
} t_TTuiPd;


//define pure data methods
extern "C" {
void TTuiPd_free(t_TTuiPd *);
void *TTuiPd_new(t_floatarg);
void TTuiPd_setup(void);


// utility
void TTuiPd_process(t_TTuiPd *x, t_floatarg paint);
void TTuiPd_displayClear(t_TTuiPd *obj);
void TTuiPd_displayPaint(t_TTuiPd *obj);
void TTuiPd_info(t_TTuiPd *obj);

// graphics
void TTuiPd_clearRect(t_TTuiPd *obj, t_floatarg x, t_floatarg clr, t_floatarg y, t_floatarg w, t_floatarg h);
void TTuiPd_drawText(t_TTuiPd *obj, t_symbol *s, int argc, t_atom *argv);
void TTuiPd_drawPNG(t_TTuiPd *obj, t_floatarg x, t_floatarg y, t_symbol *s);


// text displays
void TTuiPd_displayText(t_TTuiPd *obj, t_symbol *s, int argc, t_atom *argv);
void TTuiPd_clearText(t_TTuiPd *obj, t_floatarg clr, t_floatarg line);
void TTuiPd_invertText(t_TTuiPd *obj, t_floatarg line);
}
