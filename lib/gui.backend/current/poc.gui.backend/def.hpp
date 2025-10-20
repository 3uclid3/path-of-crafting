#pragma once

#ifndef POC_GUI_BACKEND
#error "POC_GUI_BACKEND is not defined"
#endif

#define POC_GUI_BACKEND_HEADER_IMPL(b, f) <poc.gui.backend.b/f>
#define POC_GUI_BACKEND_HEADER(f) POC_GUI_BACKEND_HEADER_IMPL(POC_GUI_BACKEND, f)
