/*
OBS Hide Desktop Icons While Streaming
Copyright (C) Susko3 <susko3@protonmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#pragma once

namespace Windows
{

/**
 * @brief Queries the operating system for desktop icons visibility.
 *
 * @return Whether the desktop icons are currently visible.
 */
bool GetDesktopIconsVisible();

/**
 * @brief Hide or show the desktop icons on Windows
 *
 * @param show true to show, false to hide
 */
void SetDesktopIconsVisible(bool show);

}
