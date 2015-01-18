/**
 * time.c - Some time related routines
 *
 * Copyright (c) 2013 David Azarewicz david@88watts.net
 *
 * Parts copied from/inspired by routines in the nveth driver.
 * Those parts are Copyright (C) 2007 nickk
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
#include "infoseg.h"

static ULONG TimeLastMs;

ULONG __watcall TimeGetSec(void)
{
  if (D16g_pSysInfoSeg == NULL) return 0;
  return D16g_pSysInfoSeg->SIS_BigTime;
}

ULONG __watcall TimeGetMs(void)
{
  if (D16g_pSysInfoSeg == NULL) return 0;
  return D16g_pSysInfoSeg->SIS_MsCount;
}

void __watcall TimeReset(void)
{
  TimeLastMs = TimeGetMs();
}

void __watcall TimeInit(void)
{
  TimeLastMs = TimeGetMs();
}

ULONG __watcall TimeDiff(void)
{
  ULONG diff = 0;
  ULONG ms = TimeGetMs();
  if (ms < TimeLastMs)
  {
    diff = (-1L - TimeLastMs) + ms;
  }
  else
  {
    diff = ms - TimeLastMs;
  }
  return diff;
}

void __watcall msleep(ULONG msecs)
{
  if (msecs == 0) return;
  if (D16g_InitComplete) DevHelp_ProcBlock((ULONG)(PVOID)&msleep, msecs, 0);
  else mdelay(msecs);
}

/**
 * TimerInit
 *
 * Setup to time a program loop.
 *
 * This function sets up the timer structure so the
 * companion function TimerCheckAndBlock() will work.
 *
 * The accuracy is limited by the OS/2 timer interrupt frequency which
 * can lead to intervals up to 55ms (18.2 timer interrupts per second).
 */
void __watcall TimerInit(TIMER far *pTimer, ULONG Milliseconds)
{
  if (D16g_pSysInfoSeg == NULL) return;
  pTimer->pMsCount = &D16g_pSysInfoSeg->SIS_MsCount;
  pTimer->Start = D16g_pSysInfoSeg->SIS_MsCount;
  pTimer->End = pTimer->Start + Milliseconds;
}

/**
 * TimerCheckAndBlock
 *
 * Check if the timer is done.
 *
 * This function checks the previously setup timer and returns true if
 * done, blocks for one timer interval otherwise.
 *
 * The accuracy is limited by the OS/2 timer interrupt frequency which
 * can lead to intervals up to 55ms (18.2 timer interrupts per second).
 */
int __watcall TimerCheckAndBlock(TIMER far *pTimer)
{
  ULONG current;

  current = *pTimer->pMsCount;
  if (pTimer->Start <= pTimer->End) {
    if ((current >= pTimer->End) || (current < pTimer->Start)) return 1;
  } else {
    if ((current >= pTimer->End) && (current < pTimer->Start)) return 1;
  }
  DevHelp_ProcBlock((ULONG)(void far *)&TimerCheckAndBlock, 1, WAIT_IS_INTERRUPTABLE);
  return 0;
}

#if 0
/**
 * Sleep specified number of milliseonds.
 */
void msleep(ULONG millies)
{
  TIMER Timer;

  if (millies == 0) return;
  TimerInit(&Timer, millies);
  while (!TimerCheckAndBlock(&Timer));
}
#endif
