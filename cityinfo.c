/*
 * This file is a part of cityinfo
 *
 * Copyright (C) 2013 Ivaylo Dimitrov(freemangordon@abv.bg), all rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <gconf/gconf-client.h>
#include <libintl.h>
#include <locale.h>

#include "cityinfo.h"

#define CITYINFO_MAX_STR_LEN 512
#define CITYINFO_DB "/usr/share/clock/wdb"

static int records_count = -1;
static GConfClient *gc_client = NULL;
static gboolean text_domain_binded = FALSE;

Cityinfo*
cityinfo_new (void)
{
  Cityinfo *city;

  city = g_try_malloc0(sizeof(Cityinfo));
  if (city)
  {
    city->id = CITYINFO_WRONG_ID;
    city->x = CITYINFO_WRONG_POS - 1.0;
    city->y = CITYINFO_WRONG_POS - 1.0;
    city->lat = CITYINFO_WRONG_LAT_LNG - 1.0;
    city->lng = CITYINFO_WRONG_LAT_LNG - 1.0;
  }

  return city;
}

gchar*
cityinfo_get_zone (const Cityinfo* city)
{
  gchar *rv = NULL;

  if (city)
    rv = city->zone;

  return rv;
}

static int
_db_count_records(FILE *db)
{
  long off;
  char buf[1024];

  g_assert(NULL != db);

  if (records_count == -1)
  {
    off = ftell(db);
    rewind(db);
    records_count = 0;

    while (fgets(buf, sizeof(buf), db))
      if (buf[0])
        records_count++;

    fseek(db, off, SEEK_SET);
  }

  return records_count;
}

static gint
_cityinfo_gconf_get_int(const gchar *key)
{
  if (!gc_client)
  {
    g_type_init();
    gc_client = gconf_client_get_default();
  }

  if (gc_client)
    return gconf_client_get_int(gc_client, key, NULL);
  else
    return 0;
}

static char**
_db_row_get_fields(char *row, char separator, int max_columns)
{
  char **rv;
  int i = 0;

  if (max_columns == -1)
    max_columns = 20;

  rv = g_try_malloc0(max_columns * sizeof(char*));

  if (!rv)
    return NULL;

  while(i < max_columns)
  {
    rv[i] = row;

    /* find the end of the current column or the end of the row */
    while(*row && (*row != separator))
      row++;

    /* it is the end of the row, get out */
    if(!*row)
      break;

    *row = 0;

    /* move to the start of the next fiels */
    row++;

    i++;
  }

  return rv;
}

static Cityinfo *
_db_row_to_cityinfo(char *row)
{
  Cityinfo *city;
  char **fields;

  g_assert(NULL != row);

  city = cityinfo_new();
  if (!city)
    return NULL;

  fields = _db_row_get_fields(row, '|', 10);
  if (!fields)
  {
    cityinfo_free(city);
    return NULL;
  }

  if (!text_domain_binded)
  {
    text_domain_binded = TRUE;
    bindtextdomain("osso-clock", "/usr/share/locale");
  }

  /* city name */
  if (*fields[1] == '/')
  {
    int i;
    char **tmp_fields = _db_row_get_fields(fields[1], ':', -1);

    i = _cityinfo_gconf_get_int(tmp_fields[0]);
    if (i > 18)
      i = 0;

    city->name = g_strndup(
          dcgettext("osso-cities",tmp_fields[i + 1], LC_MESSAGES),
          CITYINFO_MAX_STR_LEN);

    g_free(tmp_fields);
  }
  else
    city->name = g_strndup(
          dcgettext("osso-cities", fields[1], LC_MESSAGES),
          CITYINFO_MAX_STR_LEN);

  /* city code */
  city->code = g_strndup(fields[2], CITYINFO_MAX_STR_LEN);

  /* country */
  if (*fields[3] == '/')
  {
    int i;
    char **tmp_fields = _db_row_get_fields(fields[3], ':', -1);

    i = _cityinfo_gconf_get_int(*tmp_fields);
    if (i > 18)
      i = 0;

    city->country = g_strndup(
          dcgettext("osso-countries", tmp_fields[i + 1], LC_MESSAGES),
          CITYINFO_MAX_STR_LEN);

    g_free(tmp_fields);
  }
  else
    city->country = g_strndup(
          dcgettext("osso-countries", fields[3], LC_MESSAGES),
          CITYINFO_MAX_STR_LEN);

  g_strstrip(fields[9]);

  /* the others */
  city->zone = g_strndup(fields[4], CITYINFO_MAX_STR_LEN);
  city->locale = g_strndup(fields[9], CITYINFO_MAX_STR_LEN);
  city->x = g_ascii_strtod(fields[7], 0);
  city->y = g_ascii_strtod(fields[8], 0);
  city->lat = g_ascii_strtod(fields[5], 0);
  city->lng = g_ascii_strtod(fields[6], 0);
  city->id = strtol(fields[0], NULL, 10);

  g_free(fields);

  return city;
}

Cityinfo **
cityinfo_get_all(void)
{
  FILE *db;

  int i;
  Cityinfo **cities;
  int rec_cnt;
  char buf[1024];

  db = fopen(CITYINFO_DB, "r");
  g_assert(NULL != db);

  rec_cnt = _db_count_records(db);
  cities = (Cityinfo **)g_try_malloc0((rec_cnt + 1) * sizeof(Cityinfo*));
  if (cities)
    for ( i = 0; i < rec_cnt; i++ )
    {
      if (!fgets(buf, sizeof(buf), db))
        break;

      cities[i] = _db_row_to_cityinfo(buf);
    }

  fclose(db);

  return cities;
}

Cityinfo *
cityinfo_from_id(gint id)
{
  FILE *db;
  int i;
  Cityinfo *city;
  char buf[1024];

  if (id < 0)
    return NULL;

  db = fopen(CITYINFO_DB, "r");

  g_assert(NULL != db);

  if (id >= _db_count_records(db))
    goto err_out;

  for (i = 0; ;i++)
  {
    if (!fgets(buf, sizeof(buf), db))
      goto err_out;

    if (i == id)
      break;
  }

  city = _db_row_to_cityinfo(buf);
  fclose(db);
  return city;

err_out:
  fclose(db);
  return NULL;
}

void
cityinfo_free(Cityinfo *city)
{
  if ( city )
  {
    g_free(city->name);
    city->name = NULL;
    g_free(city->code);
    city->code = NULL;
    g_free(city->country);
    city->country = NULL;
    g_free(city->zone);
    city->zone = NULL;
    g_free(city->locale);
    city->locale = NULL;
    g_free(city);
  }
}

gchar *
cityinfo_get_code(const Cityinfo *city)
{
  if (city)
    return city->code;

  return NULL;
}

gdouble
cityinfo_get_ypos(const Cityinfo *city)
{
  if (city)
    return city->y;

  return CITYINFO_WRONG_POS - 1.0;
}

gdouble
cityinfo_get_xpos(const Cityinfo *city)
{
  if (city)
    return city->x;

  return CITYINFO_WRONG_POS - 1.0;
}

gdouble
cityinfo_get_lng(const Cityinfo *city)
{
  if (city)
    return city->lng;

  return  CITYINFO_WRONG_LAT_LNG - 1.0;
}

gdouble
cityinfo_get_lat(const Cityinfo *city)
{
  if (city)
    return city->lat;

  return  CITYINFO_WRONG_LAT_LNG - 1.0;
}

gchar *
cityinfo_get_name(const Cityinfo *city)
{
  if (city)
    return city->name;

  return NULL;
}

gchar *
cityinfo_get_locale(const Cityinfo *city)
{
  if (city)
    return city->locale;

  return NULL;
}

gchar *
cityinfo_get_country(const Cityinfo *city)
{
  if (city)
    return city->country;

  return NULL;
}

gint
cityinfo_get_id(const Cityinfo *city)
{
  if (city)
    return city->id;

  return CITYINFO_WRONG_ID;
}

Cityinfo *
cityinfo_clone(const Cityinfo *city)
{
  Cityinfo *city_clone;

  if (!city)
    return NULL;

  city_clone = cityinfo_new();
  if (!city_clone)
    return NULL;


  if ((city_clone->code = g_strndup(city->code, CITYINFO_MAX_STR_LEN)) &&
      (city_clone->country = g_strndup(city->country, CITYINFO_MAX_STR_LEN)) &&
      (city_clone->zone = g_strndup(city->zone, CITYINFO_MAX_STR_LEN)) &&
      (city_clone->locale = g_strndup(city->locale, CITYINFO_MAX_STR_LEN)) &&
      (city_clone->name = g_strndup(city->name, CITYINFO_MAX_STR_LEN)))
  {
    city_clone->x = city->x;
    city_clone->y = city->y;
    city_clone->lat = city->lat;
    city_clone->lng = city->lng;
    city_clone->id = city->id;

    return city_clone;
  }

  cityinfo_free(city_clone);

  return NULL;
}

void
cityinfo_foreach(CityinfoFunc func, gpointer data)
{
  FILE *db;
  char buf[1024];

  if (!func)
    return;

  db = fopen(CITYINFO_DB, "r");
  g_assert(NULL != db);

  while (fgets(buf, sizeof(buf), db))
  {
    Cityinfo *city = _db_row_to_cityinfo(buf);
    if (city)
    {
      gboolean cont = func(city, data);
      cityinfo_free(city);

      if (!cont)
        break;
    }
  }

  fclose(db);
}

void
cityinfo_free_all(Cityinfo **cities)
{
  Cityinfo *city;
  Cityinfo **tmp = cities;

  if (cities)
  {
    while((city = *tmp))
    {
      cityinfo_free(city);
      tmp++;
    }

    g_free(cities);
  }
}

Cityinfo *
cityinfo_find_closest(gdouble x, gdouble y)
{
  Cityinfo *city = NULL;
  Cityinfo *candidate_city;
  gdouble dx;
  gdouble dy;
  gdouble distance;
  FILE *db;
  gdouble best_distance;
  char buf[1024];

  if ( x < 0.0 || x > 1.0 || y < 0.0 || y > 1.0 )
    return NULL;

  db = fopen(CITYINFO_DB, "r");
  g_assert(NULL != db);

  best_distance = 2.0;

  while (fgets(buf, sizeof(buf), db))
  {
    candidate_city = _db_row_to_cityinfo(buf);
    if (!candidate_city)
      break;

    dx = x - candidate_city->x;
    if (dx < 0.0)
      dx = -(x + candidate_city->x);

    dy = y - candidate_city->y;
    if (dy < 0.0)
      dy = -(y + candidate_city->y);

    if (dx > 0.5)
      dx = 1.0 - dx;

    if (dy > 0.5)
      dy = 1.0 - dy;

    distance = dx * dx + dy * dy;
    if (distance <= best_distance)
    {
      cityinfo_free(city);
      best_distance = distance;
      city = candidate_city;
    }
    else
      cityinfo_free(candidate_city);
  }

  fclose(db);

  return city;
}
