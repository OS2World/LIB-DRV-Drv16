/**
 * defidc.c - Default IDC handler
 *
 * Copyright (c) 2013 David Azarewicz david@88watts.net
 *
 * This file is part of the Drv16 Device Driver Development Kit.
 *
 * Drv16 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Drv16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Drv16.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "Dev16lib.h"

#pragma aux IdcHandler far parm [es bx] value [ax];
USHORT IdcHandler(void far * pData)
{
  return 0;
}


