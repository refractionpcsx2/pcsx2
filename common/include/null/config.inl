/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2018 PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PS2Eext.h"
#include <string>

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>

PluginLog g_plugin_log;

class nullWxDialog : public wxDialog
{
    private:
        wxCheckBox* console_checkbox;
        wxCheckBox* file_checkbox;

    public:
        nullWxDialog();
        ~nullWxDialog();
};

nullWxDialog::nullWxDialog() : wxDialog(nullptr, wxID_ANY, "Config", wxDefaultPosition)
{
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    console_checkbox = new wxCheckBox(this, wxID_ANY, wxT("Log to console"), wxPoint(20, 20), wxSize(150, 30));
    file_checkbox = new wxCheckBox(this, wxID_ANY, wxT("Log to file"), wxPoint(20, 20), wxSize(150, 30));

    console_checkbox->SetValue(g_plugin_log.WriteToConsole);
    file_checkbox->SetValue(g_plugin_log.WriteToFile);

    sizer->Add(console_checkbox);
    sizer->Add(file_checkbox);

    sizer->SetSizeHints(this);
    SetSizer(sizer);
}

nullWxDialog::~nullWxDialog()
{
    g_plugin_log.WriteToConsole = console_checkbox->GetValue();
    g_plugin_log.WriteToFile = file_checkbox->GetValue();

    delete console_checkbox;
    delete file_checkbox;
}

void ConfigureLogging()
{
    // Yes, yes, unique_ptr. This compiles.
    nullWxDialog* nullDialog = new nullWxDialog();
    nullDialog->ShowModal();
    delete nullDialog;
}

void SaveConfig(const std::string &pathname)
{
    PluginConf ini;
    if (!ini.Open(pathname, WRITE_FILE)) {
        g_plugin_log.WriteLn("Failed to open %s", pathname.c_str());
        return;
    }

    ini.WriteInt("write_to_console", g_plugin_log.WriteToConsole);
    ini.WriteInt("write_to_file", g_plugin_log.WriteToFile);
    ini.Close();
}

void LoadConfig(const std::string &pathname)
{
    PluginConf ini;
    if (!ini.Open(pathname, READ_FILE)) {
        g_plugin_log.WriteLn("Failed to open %s", pathname.c_str());
        SaveConfig(pathname);
        return;
    }

    g_plugin_log.WriteToConsole = ini.ReadInt("write_to_console", 0) != 0;
    g_plugin_log.WriteToFile = ini.ReadInt("write_to_file", 0) != 0;
    ini.Close();
}
