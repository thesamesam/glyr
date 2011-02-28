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

#include "amazon.h"

#include "../core.h"
#include "../stringop.h"
#include "../core.h"
#include "../glyr.h"

// Example snippet of what we parse:
/***

<SmallImage>
<URL>
http://ecx.images-amazon.com/images/I/51rnlRwtsiL._SL75_.jpg
</URL>
<Height Units="pixels">75</Height>
<Width Units="pixels">75</Width>
</SmallImage>

<MediumImage>
<URL>
http://ecx.images-amazon.com/images/I/51rnlRwtsiL._SL160_.jpg
</URL>
<Height Units="pixels">160</Height>
<Width Units="pixels">160</Width>
</MediumImage>
<LargeImage>
<URL>
http://ecx.images-amazon.com/images/I/51rnlRwtsiL.jpg
</URL>
<Height Units="pixels">455</Height>
<Width Units="pixels">455</Width>
</LargeImage>

***/


/***

Country settings:

0) US
1) Canada
2) UK
3) France
4) Germany
5) Japan

***/

// A short note:
// Since some time amazon does not allow anonymous acces to their webservices.
// So you have to register an (free) account and you'll get an Accesskey and a Secretkey,
// sadly amazon does not allow you to publish the secretkey to the public,
// therefore I had to use freeapisign.com to access the webservices, the bad thing is:
// it is limited to 30K requests per month. If you have an own account: replace the Acceskey here,
// as an fallback there is the albumart and coverhunt plugin which implement a search on amazon (on serverside)
#define ACCESS_KEY API_KEY_AMAZON

const char * cover_amazon_url(GlyQuery * sets)
{
    if(sets->cover.min_size <= 500 || sets->cover.min_size)
    {
        if(!strcmp(sets->lang,"us"))
            return "http://free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
        else if(!strcmp(sets->lang,"ca"))
            return "http://ca.free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
        else if(!strcmp(sets->lang,"uk"))
            return "http://co.uk.free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
        else if(!strcmp(sets->lang,"fr"))
            return "http://fr.free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
        else if(!strcmp(sets->lang,"de"))
            return "http://de.free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
        else if(!strcmp(sets->lang,"jp"))
            return "http://co.jp.free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
        else
            return "http://free.apisigning.com/onca/xml?Service=AWSECommerceService&AWSAccessKeyId="ACCESS_KEY"&Operation=ItemSearch&SearchIndex=Music&ResponseGroup=Images&Keywords=%artist%+%album%\0";
    }
    return NULL;
}

#define MAX(X) (capo->s->cover.max_size <  X && capo->s->cover.max_size != -1)
#define MIN(X) (capo->s->cover.min_size >= X && capo->s->cover.min_size != -1)

GlyCacheList * cover_amazon_parse(cb_object *capo)
{
    char *tag_ssize = (capo->s->cover.max_size == -1 && capo->s->cover.min_size == -1) ? "<LargeImage>"  :
                      (MAX( 30) && MIN(-1)) ? "<SwatchImage>" :
                      (MAX( 70) && MIN(30)) ? "<SmallImage>"  :
                      (MAX(150) && MIN(70)) ? "<MediumImage>" :
                      "<LargeImage>"  ;

#undef MAX
#undef MIN

    int urlc = 0;
    GlyCacheList * r_list = NULL;

    char * find = capo->cache->data;
    while( (find = strstr(find +1, tag_ssize)) != NULL && continue_search(urlc,capo->s))
    {
        /* Next two XML tags not relevant */
        nextTag(find);
        nextTag(find);

        char * endTag = NULL;
        if( (endTag = strstr(find, "</URL>")) != NULL)
        {
            char * result_url = copy_value(find,endTag);
            if(result_url)
            {
                if(!r_list) r_list = DL_new_lst();

                GlyMemCache * result_cache = DL_init();
                result_cache->data = result_url;
                result_cache->size = strlen(result_url);

                DL_add_to_list(r_list,result_cache);
                urlc++;
            }
        }
    }
    return r_list;
}
