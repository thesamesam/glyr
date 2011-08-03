/***********************************************************
* This file is part of glyr
* + a commnadline tool and library to download various sort of musicrelated metadata.
* + Copyright (C) [2011]  [Christopher Pahl]
* + Hosted at: https://github.com/sahib/glyr
*
* glyr is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* glyr is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with glyr. If not, see <http://www.gnu.org/licenses/>.
**************************************************************/

#ifndef CORE_H
#define CORE_H

#include "types.h"
#include "apikeys.h"
#include "config.h"

/* Global */
#include <string.h>
#include <curl/curl.h>
#include <glib.h>

#define PRT_COLOR glyr_USE_COLOR
#define USE_COLOR
#ifdef  USE_COLOR
#define C_B "\x1b[34;01m" // blue
#define C_M "\x1b[35;01m" // magenta
#define C_C "\x1b[36;01m" // Cyan
#define C_R "\x1b[31;01m" // Red
#define C_G "\x1b[32;01m" // Green
#define C_Y "\x1b[33;01m" // Yellow
#define C_  "\x1b[0m"     // Reset
#else
#define C_B "" // blue
#define C_M "" // magenta
#define C_C "" // Cyan
#define C_R "" // Red
#define C_G "" // Green
#define C_Y "" // Yellow
#define C_  "" // Reset
#endif

// libglyr uses checksums to filter double items
// Also you can use those as easy comparasion method
// There is no valid reason to diasable this actually
#define CALC_MD5SUMS true

/*------------------------------------------------------*/

// Internal calback object, used for cover, lyrics and other
// This is only used inside the core and the plugins
// Other parts of the program shall not use this struct
// GlyMemCache is what you're searching
// It models the data that one plugin needs.
typedef struct cb_object
{
    // What url to download before the callback is called
    char *url;

    // What curl handle this is attached
    CURL *handle;

    // pointer to settings struct (artist,album,etc)
    GlyQuery * s;

    // internal cache attached to this url
    GlyMemCache *cache;

} cb_object;

/*------------------------------------------------------*/

// Internal representation of one metadataprovider
// PLEASE FILL _ALL_ FIELDS!
typedef struct MetaDataFetcher
{
    /* cover, lyrics, stuff */
    const char * name;

    /* A List of MetaDataSources */
    GList * provider;

    /* what this thing delievers; e.g. GET_COVERART */
    enum GLYR_GET_TYPE type;

    /* callbacks */
    bool (*validate)(GlyQuery *);
    void (*init)(void);
    void (*destroy)(void);
    GList* (*finalize)(GlyQuery*,GList*,gboolean*);

    /* Wether this Fetcher delievers the full data (lyrics),
       or just URLs of the data. */
    gboolean full_data;

} MetaDataFetcher;

/*------------------------------------------------------*/

// Internal representation of one provider
// PLEASE FILL _ALL_ FIELDS!
typedef struct MetaDataSource
{
    gchar * name;  /* Name of this provider            */
    gchar key;     /* A key that may be used in --from */

    GList * (* parser) (struct cb_object *);  /* called when parsing is needed                  */
    const char  * (* get_url)(GlyQuery *); 	/* called when the url of this provider is needed */
    gchar  * endmarker;              	        /* Download stops if this mark is found           */

    enum GLYR_GET_TYPE type; /* For what fetcher this provider is working.. */
    gint priority;  /* What priority this plugin has            */
    gboolean free_url; /* URL is dyn. allocated - set this always! */

    gint quality;  /* Measurement of how good the content  usually is [0-100] */
    gint speed;    /* Measurement of how fast the provider usually is [0-100] */

} MetaDataSource;

/*------------------------------------------------------*/


//TODO
enum CORE_ERR
{
    ALL_OK,
};

/*------------------------------------------------------*/

typedef GList*(*AsyncDLCB)(cb_object*,void *,bool*,gint*);
GList * start_engine(GlyQuery * query, MetaDataFetcher * fetcher);
GList * async_download(GList * url_list, GList * endmark_list, GlyQuery * s, long parallel_fac, long timeout_fac, AsyncDLCB callback, void * userptr);
GlyMemCache * download_single(const char* url, GlyQuery * s, const char * end);

/*------------------------------------------------------*/

gboolean continue_search(gint current, GlyQuery * s);
void update_md5sum(GlyMemCache * c);

GlyMemCache * DL_init(void);
GlyMemCache * DL_copy(GlyMemCache * src);
void DL_free(GlyMemCache *cache);

// Will be replaced by g_print
int glyr_message(int v, GlyQuery * s, FILE * stream, const char * fmt, ...);

gsize delete_dupes(GList * result, GlyQuery * s);
gsize delete_invalid_format(GList * result, gchar * allowed_formats);
#endif
