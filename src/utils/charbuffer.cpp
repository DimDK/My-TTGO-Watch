/****************************************************************************
 *   Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"
#ifdef NATIVE_64BIT
    #include "logging.h"
#else
    #include <Arduino.h>
#endif

#include "charbuffer.h"
#include "utils/alloc.h"

CharBuffer::CharBuffer( void ) : filter(true), msg(NULL), capacity(0), size(0) {
    // Do not alloc here, PSRAm subsystem is not necessarily ready
}

void CharBuffer::append(char c) {
#ifdef NATIVE_64BIT
    log_v("CharBuffer::append: size %ld capacity %ld", size, capacity);
#else
    log_v("CharBuffer::append: size %d capacity %d", size, capacity);
#endif
    if ( capacity == 0 )
        clear();
    if ( size + 2 > capacity ) {
#ifdef NATIVE_64BIT
        log_v("CharBuffer::append realloc: size %ld capacity %ld", size, capacity);
#else
        log_v("CharBuffer::append realloc: size %d capacity %d", size, capacity);
#endif
        // Realloc
        char *new_msg = NULL;
        new_msg = (char *)REALLOC( msg, capacity + CHUNK_CAPACITY );
        if ( new_msg == NULL ) {
            log_e("msg realloc fail");
            while(true);            
        }
        msg = new_msg;
        capacity += CHUNK_CAPACITY;
    }
    size++;

    if( filter ) {
        if( !( ( c >= ' ' && c <= '~' ) || c == '\r' || c == '\n' || c == '\t') ) {
            c = '?';
        }
    }

    msg[ size - 1 ] = c;
    msg[ size ] = '\0';
}

void CharBuffer::setFilter( bool enable ) {
    filter = enable;
}

void CharBuffer::clear( void ) {
    if ( capacity != INITIAL_CAPACITY ) {
        log_d("CharBuffer::clear alloc");
        free(msg);
        msg = (char *)CALLOC( INITIAL_CAPACITY, 1 );
        if ( msg == NULL ) {
            log_e("msg alloc fail");
            while(true);
        }
        capacity = INITIAL_CAPACITY;
    }
    msg[0] = '\0';
    size = 0;
}

void CharBuffer::erase( size_t pos ) {
    if ( pos >= size )
        // Nothing to do
        return;
    for ( int i = pos ; i < size ; i++ ) {
        msg[i] = '\0';
    }
    size = pos;
}