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
#include <stdbool.h>

#include "../core.h"
#include "../stringlib.h"

#define INFO_BEGIN "</script><br />"
#define INFO_ENDIN "<br />"

const char * ainfo_lyricsreg_url(GlyQuery * s)
{
    return "http://www.lyricsreg.com/biography/%artist%/";
}

GlyCacheList * ainfo_lyricsreg_parse(cb_object * capo)
{
    GlyCacheList * ls = NULL;
    size_t ib_len = strlen(INFO_BEGIN);
    char * point_to_start = strstr(capo->cache->data,INFO_BEGIN);
    if(point_to_start != NULL)
    {
        point_to_start += ib_len;
        char * mend = strstr(point_to_start, INFO_ENDIN);
        char * info = copy_value(point_to_start, mend);
        if(info != NULL && (mend-point_to_start) > 150)
        {
            ls = DL_new_lst();
            GlyMemCache * tmp = DL_init();
            tmp->data = beautify_lyrics(info);
            tmp->size = (tmp->data) ? strlen(tmp->data) : 0;
            tmp->dsrc = strdup(capo->url);

            DL_add_to_list(ls,tmp);

            free(info);
        }
    }
    return ls;
}
