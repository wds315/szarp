/* 
  SZARP: SCADA software 
  

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
/* 
 * draw3 
 * SZARP

 *
 * $Id: wxgraphs.h 1 2009-06-24 15:09:25Z isl $
 */

#include <vector>

#include "ids.h"
#include "classes.h"
#include "drawtime.h"
#include "dbinquirer.h"
#include "database.h"
#include "draw.h"
#include "cfgmgr.h"

#include "graphsutils.h"

std::vector<SeasonLimit> get_season_limits_indexes(DrawsSets *ds, Draw* draw) {

	std::vector<SeasonLimit> ret;

	IPKConfig *ipk = dynamic_cast<IPKConfig*>(ds);
	if (ipk == NULL)
		return ret;

	const TSSeason* s = ipk->GetTSzarpConfig()->GetSeasons();

	wxDateTime pt = draw->GetTimeOfIndex(0);
	wxDateTime::Tm tm = pt.GetTm(wxDateTime::Local);
	int year = tm.year;

	TSSeason::Season season = s->GetSeason(year);
	bool is_summer = s->CheckSeason(season, tm.mon - wxDateTime::Jan + 1, tm.mday);

	for (size_t i = 1; i < draw->GetValuesTable().size() - 1; ++i) {
		wxDateTime t = draw->GetTimeOfIndex(i + 1);
		tm = t.GetTm(wxDateTime::Local);
		if (tm.year != year) {
			year = tm.year;
			season = s->GetSeason(year);
		}
		bool is = s->CheckSeason(season, (tm.mon - wxDateTime::Jan) + 1, tm.mday);
		if (is != is_summer) {
			size_t index;
			switch (draw->GetPeriod()) {
				case PERIOD_T_MINUTE:
				case PERIOD_T_3MINUTE:
				case PERIOD_T_30MINUTE:
				case PERIOD_T_DAY:
				case PERIOD_T_MONTH:
				case PERIOD_T_WEEK:
					index = i;
					break;
				case PERIOD_T_LAST:
				case PERIOD_T_OTHER:
				case PERIOD_T_SEASON:
				case PERIOD_T_YEAR:
				case PERIOD_T_DECADE:
					if (is)
						if (tm.mday == season.day_start)
							index = i;
						else
							index = i - 1;
					else
						if (tm.mday  == season.day_end)
							index = i;
						else
							index = i - 1;

					break;
				default:
					index = 0;
					assert(false);
			}

			SeasonLimit sl;
			sl.index = index;
			sl.summer = is;
			if (is) {
				sl.day = season.day_start;
				sl.month = season.month_start;
			} else {
				sl.day = season.day_end;
				sl.month = season.month_end;
			}

			ret.push_back(sl);
		}
		is_summer = is;
		pt = t;
	}

	return ret;
}

wxString get_short_day_name(wxDateTime::WeekDay day) {
    switch (day) {
	case 0 : return _("Su");
	case 1 : return _("Mo");
	case 2 : return _("Tu");
	case 3 : return _("We");
	case 4 : return _("Th");
	case 5 : return _("Fr");
	case 6 : return _("Sa");
	default :
		 break;
    }
    return _T("?");
}

wxString get_date_string(PeriodType period, const wxDateTime& prev_date, const wxDateTime &date) {
	wxString ret;
	switch (period) {
		case PERIOD_T_DECADE :
			ret = wxString::Format(_T("%04d"), date.GetYear());
			break;
		case PERIOD_T_YEAR :
			ret = wxString::Format(_T("%02d"), date.GetMonth() + 1);
			break;
		case PERIOD_T_MONTH :
			ret = wxString::Format(_T("%02d"), date.GetDay());
			break;
		case PERIOD_T_WEEK :
			ret = get_short_day_name(date.GetWeekDay());
			break;
		case PERIOD_T_DAY :
			ret = wxString::Format(_T("%02d"), date.GetHour());
			break;
		case PERIOD_T_30MINUTE :
			if (prev_date.IsValid() && prev_date.GetHour() != date.GetHour())
				ret = wxString::Format(_T("%02d:%02d"), date.GetHour(), date.GetMinute());
			else
				ret = wxString::Format(_T("%02d"), date.GetMinute());
			break;
		case PERIOD_T_3MINUTE :
			ret = wxString::Format(_T("%02d"), date.GetSecond() / 10 * 10);
			break;
		case PERIOD_T_MINUTE :
			ret = wxString::Format(_T("%02d"), date.GetSecond() / 5 * 5);
			break;
		case PERIOD_T_SEASON :
			ret = wxString::Format(_T("%02d"), date.GetWeekOfYear());
			break;
		default:
			break;
	}
	return ret;
}

double get_y_position(const double& value, DrawInfo* di) {
	double max = di->GetMax();
	double min = di->GetMin();

	double dif = max - min;

	if( max <= min  ) {
		// FIXME:  Draw3 should atomaticly detect axes in that case, but
		//         it currently doesnt :(
		wxLogWarning(_T("Parameter %s has invalid min/max values: min %f, max %f. Min is set to 0, and max to 100."), di->GetName().c_str(), min, max);
		min = 0;
		max = 100;
	}

	int sc = di->GetScale();
	double ret;
	if (sc) {
		double smin, smax, sdif, k;

		smin = di->GetScaleMin();
		smax = di->GetScaleMax();

		assert(smax > smin);
		
		sdif = smax - smin;

		k = (dif - sdif) / sdif / (1 / (sc / 100.) - 1);

		double scaled = wxMax(value - smax, 0) +
			wxMax(wxMin(value - smin, smax - smin), 0) * k +
			wxMax(wxMin(value - min, smin - min), 0);

		ret = scaled / (dif + (k - 1) * sdif);

	} else {
		ret = (value - min) / dif;
	}

	return ret;
}
