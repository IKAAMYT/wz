#include "gui.h"
#include "gui_colors.h"
#include "../../../../dependencies/oxorany/oxorany.h"
#include "../../../core/call of duty/settings/settings.h"

void ui::tabs::settings(const TabCategory tab)
{
    if (tab.name == "settings")
    {
        const int full_height = static_cast<int>(ui::size.y - 95.0f);
        if (ui::begin_child_left( ( "settings" ), full_height))
        {
            ImGui::TextColored ( ui::colors::text, "general" );
        }
        ImGui::EndChild();
    }
}
