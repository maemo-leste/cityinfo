
/*
 * This file is a part of cityinfo
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact:  Qi Ouyang <qi.ouyang@nokia.com>
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


#ifndef __CITIES_DATABASE_H__
#define __CITIES_DATABASE_H__

#include <glib.h>

G_BEGIN_DECLS

/** 
 * CITYINFO_WRONG_ID:
 * 
 * Integer indicating wrong ID of the city.
 *
 * If You get this ID from some Cityinfo structure that means that this
 * particular structure does not represent any real city - it is useless (the
 * reason for this might be some errors while reading database, cloning city or
 * simply because structure is only allocated - returned from cityinfo_new())
 *
 */
#define CITYINFO_WRONG_ID -1

/** 
 * CITYINFO_WRONG_POS:
 *
 * Double indicating wrong X/Y position of the city.
 * 
 * If You get x/y postion of the city and You will get smaller value than this
 * it means that there were some problems and the whole source Cityinfo
 * structure is meaningless.
 *
 */
#define CITYINFO_WRONG_POS -1.0

/** 
 * CITYINFO_WRONG_LAT_LNG:
 * 
 * Double indicating wrong latitude/longitude value.
 *
 * If You get lat/lng postion of the city and You will get smaller value than
 * this it means that there were some problems and the whole source Cityinfo
 * structure is meaningless.
 *
 */
#define CITYINFO_WRONG_LAT_LNG -181.0

typedef struct _Cityinfo Cityinfo;
/**
 * Cityinfo:
 * @name:	Name of the city 
 * @code: 	Country code
 * @country: 	Name of the country
 * @zone: 	Name of the timezone used in particular city
 * @locale:	Locale used in the given city 
 * @lat:	Latitude of the city
 * @lng:	Longitude of the city
 * @x:		X position of the city (on the clock's map)
 * @y:		Y position of the city (on the clock's map)
 * @id:		ID of the city
 *
 * #Cityinfo is a structure which represents the city.
 */
struct _Cityinfo {
	gchar *name;    
	gchar *code;   
	gchar *country;
	gchar *zone;  
	gchar *locale;
	gdouble lat;   
	gdouble lng;  
	gdouble x;   
	gdouble y;  
	gint id;   
};

 
/** 
 * CityinfoFunc:
 * @city:	Currently proccessed city.
 * @data:	Pointer to user data.
 *
 * Returns:	TRUE if function cityinfo_foreach() should working further on next
 * cities in the databse, and FALSE if function should stop executing after this one
 *
 * Function cityinfo_foreach() use this type to define action which should be 
 * taken on each of the city in database. 
 **/
typedef gboolean (*CityinfoFunc) (const Cityinfo* city, gpointer data);

/* ==== get internal data of Cityinfo structure ==== */

gchar* cityinfo_get_name (const Cityinfo* city);

gchar* cityinfo_get_code (const Cityinfo* city);

gchar* cityinfo_get_country (const Cityinfo* city);

gchar* cityinfo_get_zone (const Cityinfo* city);

gchar* cityinfo_get_locale (const Cityinfo* city);

gdouble cityinfo_get_xpos (const Cityinfo* city);

gdouble cityinfo_get_ypos (const Cityinfo* city);

gdouble cityinfo_get_lat (const Cityinfo* city);

gdouble cityinfo_get_lng (const Cityinfo* city);

gint cityinfo_get_id (const Cityinfo* city);

/* ==== do some action on Cityinfo structure or database ==== */

Cityinfo* cityinfo_new (void);

void cityinfo_free (Cityinfo* city);

Cityinfo* cityinfo_clone (const Cityinfo* city);

void cityinfo_foreach (CityinfoFunc func, gpointer data);

Cityinfo* cityinfo_find_closest (gdouble x, gdouble y);

Cityinfo* cityinfo_from_id (gint id);

Cityinfo** cityinfo_get_all (void);

void cityinfo_free_all (Cityinfo** cities);

G_END_DECLS

#endif							/* __CITIES_DATABASE_H__ */
