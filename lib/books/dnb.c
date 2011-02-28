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
#include <stdlib.h>
#include <string.h>

#include "dnb.h"

#include "../types.h"
#include "../core.h"
#include "../stringop.h"

#define TABLE_START "<table valign=\"bottom\" cellpadding=\"3\" cellspacing=\"0\" class=\"yellow\" width=\"100%\" summary=\"Vollanzeige des Suchergebnises\">"
#define TABLE_END "</table>"

#define COLUMN_KEY "<td width=\"25%\""
#define COLUMN_KEY_START ">"
#define COLUMN_KEY_END "</td>"
#define COLUMN_VALUE "<td"
#define COLUMN_VALUE_START ">"
#define COLUMN_VALUE_END "</td>"

#define URL "https://portal.d-nb.de/opac.htm?method=showFullRecord&currentResultId=%artist%%2526any&currentPosition=0"
#define DOMAIN "d-nb."

const char * books_dnb_url(GlyQuery * settings)
{
    if (strcmp(settings->artist, "LINK") == 0)
    {
        if (strstr(settings->album, DOMAIN) != NULL)
        {
            return settings->album;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return URL;
    }
}

GlyMemCache * get_column(char ** pointer, char * start, char * end)
{
    char * column = NULL;
    column = (column=getStr(pointer, start, end)) == NULL ? "" : column;
    column = remove_html_tags_from_string(column, strlen(column));
    trim_inplace(column);

    GlyMemCache * rc = DL_init();
    if(rc)
    {
        rc->data = column;
        rc->size = strlen(column);
    }
    return rc;
}

GlyCacheList * books_dnb_parse(cb_object * capo)
{
    char * pointer = capo->cache->data;
    pointer = getStr(&pointer, TABLE_START, TABLE_END);

    if (pointer == NULL)
    {
        return NULL;
    }

    GlyMemCache * r = NULL;
    GlyCacheList *ls = NULL;

    int urlc = 0;
    while (continue_search(urlc++,capo->s))
    {
        if ((pointer = strstr(pointer, COLUMN_KEY)) == NULL)
        {
            if(!ls) ls = DL_new_lst();

            r = get_column(&pointer,COLUMN_KEY_START,COLUMN_KEY_END);
            DL_add_to_list(ls,r);

            if ((pointer = strstr(pointer, COLUMN_VALUE)) != NULL)
            {
                r = get_column(&pointer,COLUMN_VALUE_START, COLUMN_VALUE_END);
                DL_add_to_list(ls,r);
            }
        }
    }

    return ls;
}
