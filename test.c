#include <cityinfo.h>
#include <stdio.h>

void dump_city_info(const Cityinfo* ci)
{
  printf("%s %s %s %s %s %lf %lf %lf %lf %d\n",
         cityinfo_get_name(ci),
         cityinfo_get_code(ci),
         cityinfo_get_country(ci),
         cityinfo_get_zone(ci),
         cityinfo_get_locale(ci),
         cityinfo_get_lat(ci),
         cityinfo_get_lng(ci),
         cityinfo_get_xpos(ci),
         cityinfo_get_ypos(ci),
         cityinfo_get_id(ci));
}

gboolean cb(const Cityinfo* ci, gpointer data)
{
  dump_city_info(ci);
  return TRUE;
}

int main()
{
  Cityinfo** ci = cityinfo_get_all();
  Cityinfo** ci1 = ci;
  Cityinfo * city;
  int i = 0;
  gdouble x,y;

  for(;*ci; ci++)
    dump_city_info(*ci);

  cityinfo_free_all(ci1);

  cityinfo_foreach(cb, NULL);

  while((city = cityinfo_from_id(i)))
  {
    dump_city_info(city);
    cityinfo_free(city);
    i++;
  }

  for(x=0.0; x<1.0; x+=.1)
    for(y=0.0; y<1.0; y+=.1)
    {
      city = cityinfo_find_closest(x,y);
      dump_city_info(city);
      cityinfo_free(city);
    }

    return 0;
}
