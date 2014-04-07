/*
** Copyright (C) 2014 Cisco and/or its affiliates. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
// inspector.cc author Russ Combs <rucombs@cisco.com>

#include "inspector.h"

#include <assert.h>

//-------------------------------------------------------------------------
// packet handler stuff
//-------------------------------------------------------------------------

SO_PUBLIC unsigned Inspector::max_slots = 1;
SO_PUBLIC unsigned THREAD_LOCAL Inspector::slot = 0;

SO_PUBLIC Inspector::Inspector()
{
    assert(slot < max_slots);
    ref_count = new unsigned[max_slots];

    for ( unsigned i = 0; i < max_slots; ++i )
        ref_count[i] = 0;
}

SO_PUBLIC Inspector::~Inspector()
{
    unsigned total = 0;

    for (unsigned i = 0; i < max_slots; ++i )
        total += ref_count[i];

    assert(!total);

    delete[] ref_count;
}

SO_PUBLIC bool Inspector::is_inactive()
{
    for (unsigned i = 0; i < max_slots; ++i )
        if ( ref_count[i] )
            return false;
    
    return true;
}
