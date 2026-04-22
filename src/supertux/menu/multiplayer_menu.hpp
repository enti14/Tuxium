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

#pragma once

#include "gui/menu.hpp"

class MultiplayerMenu final : public Menu
{
public:
  MultiplayerMenu();

  void menu_action(MenuItem& item) override;

private:
  enum MenuIDs
  {
    MNID_SERVER_REGION = 100,
    MNID_SERVER_SELECT,
    MNID_SERVER_ADD,
    MNID_SERVER_REMOVE,
    MNID_SERVER_FAVORITE,
    MNID_HOST_PORT,
    MNID_HOST_MAX_PLAYERS,
    MNID_QUICK_JOIN,
    MNID_HOST_SESSION,
    MNID_HOST_LAN
  };

  int m_server_region;
};
