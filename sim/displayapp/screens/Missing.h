#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/Apps.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Missing : public Screen {
      public:
        Missing(DisplayApp* app, Pinetime::Applications::Apps app_key);
        ~Missing() override;
      };
    }
  }
}
