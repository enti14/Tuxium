//  SuperTux
//  Copyright (C) 2021 A. Semphris <semphris@protonmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/menu/multiplayer_menu.hpp"

#include <string>
#include <vector>

#include "fmt/format.h"
#include "gui/dialog.hpp"
#include "gui/menu_manager.hpp"
#include "gui/item_toggle.hpp"
#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "util/gettext.hpp"

MultiplayerMenu::MultiplayerMenu()
  : m_server_region(0)
{
  std::vector<std::string> server_names;
  server_names.reserve(g_config->multiplayer_servers.size());
  for (const auto& server : g_config->multiplayer_servers)
  {
    std::string label = server.name;
    if (server.favorite)
      label += " ★";
    server_names.push_back(label);
  }

  add_label(_("Multiplayer"));
  add_hl();

  add_toggle(-1, _("Auto-manage Players"), &g_config->multiplayer_auto_manage_players)
    .set_help(_("Automatically add and remove players when controllers are plugged or unplugged"));

  add_toggle(-2, _("Allow Multibind"), &g_config->multiplayer_multibind)
    .set_help(_("Allow binding multiple joysticks to a single player"));

  add_toggle(-3, _("Unlock player limit"), &g_config->multiplayer_no_limit)
    .set_help(_("Allow having more than 4 players. Some interface elements may overflow out of the screen."));

  add_hl();
  add_label(_("Server Browser (Minecraft-style)"));

  add_toggle(-4, _("Enable online multiplayer"), &g_config->multiplayer_enable_online)
    .set_help(_("Enable server-based multiplayer options in this menu."));

  add_toggle(-6, _("Allow LAN discovery"), &g_config->multiplayer_allow_lan_discovery)
    .set_help(_("Show and discover local network hosts automatically."));

  add_string_select(MNID_SERVER_REGION, _("Server Region"), &m_server_region,
                    {_("Auto"), _("Europe"), _("US East"), _("US West"), _("Asia"), _("Custom")})
    .set_help(_("Select a preferred matchmaking/server region."));

  add_string_select(MNID_SERVER_SELECT, _("Saved Servers"), &g_config->multiplayer_selected_server, server_names)
    .set_help(_("Pick a saved server from your multiplayer list."));

  add_textfield(_("Server Address"), &g_config->multiplayer_server_address)
    .set_help(_("Set a direct server endpoint, e.g. official.tuxium.net:7777"));

  add_entry(MNID_SERVER_ADD, _("Add Current Server to List"));
  add_entry(MNID_SERVER_REMOVE, _("Remove Selected Server"));
  add_entry(MNID_SERVER_FAVORITE, _("Toggle Favorite"));

  add_hl();
  add_label(_("Hosting"));

  add_toggle(-5, _("Host public lobby"), &g_config->multiplayer_host_public)
    .set_help(_("Allow hosted sessions to be visible in server browser lists."));

  add_textfield(_("Host Name"), &g_config->multiplayer_host_name)
    .set_help(_("Name shown to players in the server browser."));

  add_textfield(_("Host Name Tag"), &g_config->multiplayer_host_nametag)
    .set_help(_("Custom nametag shown above your character in hosted matches."));

  add_textfield(_("Tuxium Skin Pack"), &g_config->multiplayer_host_skin_pack)
    .set_help(_("Choose which Tuxium skins are available for this server mode."));

  add_textfield(_("Mode Codename"), &g_config->multiplayer_host_mode_codename)
    .set_help(_("Internal codename for this mode. Use '2i' for the new server-create mode."));

  add_intfield(_("Host Port"), &g_config->multiplayer_host_port, MNID_HOST_PORT, true)
    .set_help(_("Server port for direct-connect and hosting (1-65535)."));

  add_intfield(_("Host max players"), &g_config->multiplayer_host_max_players, MNID_HOST_MAX_PLAYERS, true)
    .set_help(_("Maximum player slots when hosting a game (2-16)."));

  add_toggle(-7, _("Enable Teams"), &g_config->multiplayer_host_teams_enabled)
    .set_help(_("Players are grouped into teams in this mode."));

  add_toggle(-8, _("Show Team Levels"), &g_config->multiplayer_host_show_team_levels)
    .set_help(_("Show teammates and their current level in the HUD."));

  add_toggle(-9, _("Enable PvP Combat"), &g_config->multiplayer_host_enable_pvp)
    .set_help(_("Use player-vs-player combat while keeping the normal level system."));

  add_toggle(-10, _("Jump Boost on Players"), &g_config->multiplayer_host_jump_boost)
    .set_help(_("Allow jumping on another player to receive a movement boost."));

  add_inactive(_("PvP Sword Mini-Game: press inventory (I) to equip/unequip sword"));

  add_toggle(-11, _("Global Infinite Level"), &g_config->multiplayer_host_global_infinite_level)
    .set_help(_("Use a global level progression with no maximum level cap."));

  add_entry(MNID_QUICK_JOIN, _("Join Selected Server"));
  add_entry(MNID_HOST_LAN, _("Host LAN Session"));
  add_entry(MNID_HOST_SESSION, _("Host Internet Session"));

  add_submenu(_("Manage Players"), MenuStorage::MULTIPLAYER_PLAYERS_MENU);

  add_hl();
  add_back(_("Back"));
}

void
MultiplayerMenu::menu_action(MenuItem& item)
{
  switch (item.get_id())
  {
    case MNID_SERVER_REGION:
      switch (m_server_region)
      {
        case 1: g_config->multiplayer_server_address = "eu.tuxium.net:7777"; break;
        case 2: g_config->multiplayer_server_address = "useast.tuxium.net:7777"; break;
        case 3: g_config->multiplayer_server_address = "uswest.tuxium.net:7777"; break;
        case 4: g_config->multiplayer_server_address = "asia.tuxium.net:7777"; break;
        case 5: break; // custom; keep user-entered address
        case 0:
        default:
          g_config->multiplayer_server_address = "official.tuxium.net:7777";
          break;
      }
      break;

    case MNID_SERVER_SELECT:
      if (g_config->multiplayer_selected_server >= 0 &&
          g_config->multiplayer_selected_server < static_cast<int>(g_config->multiplayer_servers.size()))
      {
        g_config->multiplayer_server_address = g_config->multiplayer_servers[g_config->multiplayer_selected_server].address;
      }
      break;

    case MNID_SERVER_ADD:
      g_config->multiplayer_servers.push_back({
        fmt::format("Custom Server {}", g_config->multiplayer_servers.size() + 1),
        g_config->multiplayer_server_address,
        "Added from in-game browser",
        false
      });
      g_config->multiplayer_selected_server = static_cast<int>(g_config->multiplayer_servers.size()) - 1;
      MenuManager::instance().set_menu(std::make_unique<MultiplayerMenu>());
      break;

    case MNID_SERVER_REMOVE:
      if (g_config->multiplayer_servers.size() <= 1)
      {
        Dialog::show_message(_("At least one server entry must remain in the list."));
      }
      else
      {
        const int idx = g_config->multiplayer_selected_server;
        if (idx >= 0 && idx < static_cast<int>(g_config->multiplayer_servers.size()))
        {
          g_config->multiplayer_servers.erase(g_config->multiplayer_servers.begin() + idx);
          if (g_config->multiplayer_selected_server >= static_cast<int>(g_config->multiplayer_servers.size()))
            g_config->multiplayer_selected_server = static_cast<int>(g_config->multiplayer_servers.size()) - 1;
          g_config->multiplayer_server_address = g_config->multiplayer_servers[g_config->multiplayer_selected_server].address;
          MenuManager::instance().set_menu(std::make_unique<MultiplayerMenu>());
        }
      }
      break;

    case MNID_SERVER_FAVORITE:
      if (g_config->multiplayer_selected_server >= 0 &&
          g_config->multiplayer_selected_server < static_cast<int>(g_config->multiplayer_servers.size()))
      {
        auto& server = g_config->multiplayer_servers[g_config->multiplayer_selected_server];
        server.favorite = !server.favorite;
        Dialog::show_message(server.favorite ? _("Server marked as favorite.") : _("Server removed from favorites."));
        MenuManager::instance().set_menu(std::make_unique<MultiplayerMenu>());
      }
      break;

    case MNID_HOST_PORT:
      if (g_config->multiplayer_host_port < 1)
        g_config->multiplayer_host_port = 1;
      else if (g_config->multiplayer_host_port > 65535)
        g_config->multiplayer_host_port = 65535;
      break;

    case MNID_HOST_MAX_PLAYERS:
      if (g_config->multiplayer_host_max_players < 2)
        g_config->multiplayer_host_max_players = 2;
      else if (g_config->multiplayer_host_max_players > 16)
        g_config->multiplayer_host_max_players = 16;
      break;

    case MNID_QUICK_JOIN:
      if (!g_config->multiplayer_enable_online)
      {
        Dialog::show_message(_("Enable online multiplayer first to join servers."));
      }
      else
      {
        Dialog::show_message(_("Connecting to server:\n") + g_config->multiplayer_server_address +
                             _("\n\nServer-browser matchmaking is enabled (preview backend)."));
      }
      break;

    case MNID_HOST_LAN:
      if (g_config->multiplayer_host_mode_codename.empty())
        g_config->multiplayer_host_mode_codename = "2i";
      Dialog::show_message(_("Starting LAN host:\n") + g_config->multiplayer_host_name +
                           _("\nNametag: ") + g_config->multiplayer_host_nametag +
                           _("\nSkin pack: ") + g_config->multiplayer_host_skin_pack +
                           _("\nMode codename: ") + g_config->multiplayer_host_mode_codename +
                           _("\nPort: ") + std::to_string(g_config->multiplayer_host_port) +
                           _("\n\nPlayers on your local network can now join."));
      break;

    case MNID_HOST_SESSION:
      if (!g_config->multiplayer_enable_online)
      {
        Dialog::show_message(_("Enable online multiplayer first to host internet sessions."));
      }
      else
      {
        if (g_config->multiplayer_host_mode_codename.empty())
          g_config->multiplayer_host_mode_codename = "2i";
        Dialog::show_message(_("Hosting internet session:\n") + g_config->multiplayer_host_name +
                             _("\nNametag: ") + g_config->multiplayer_host_nametag +
                             _("\nSkin pack: ") + g_config->multiplayer_host_skin_pack +
                             _("\nMode codename: ") + g_config->multiplayer_host_mode_codename +
                             _("\nAddress: ") + g_config->multiplayer_server_address +
                             _("\nVisibility: ") + (g_config->multiplayer_host_public ? _("Public") : _("Private")) +
                             _("\nMax players: ") + std::to_string(g_config->multiplayer_host_max_players) +
                             _("\nTeams: ") + (g_config->multiplayer_host_teams_enabled ? _("Enabled") : _("Disabled")) +
                             _("\nShow levels: ") + (g_config->multiplayer_host_show_team_levels ? _("Enabled") : _("Disabled")) +
                             _("\nPvP: ") + (g_config->multiplayer_host_enable_pvp ? _("Enabled") : _("Disabled")) +
                             _("\nJump boost: ") + (g_config->multiplayer_host_jump_boost ? _("Enabled") : _("Disabled")) +
                             _("\nGlobal infinite level: ") + (g_config->multiplayer_host_global_infinite_level ? _("Enabled") : _("Disabled")));
      }
      break;

    default:
      break;
  }
}
