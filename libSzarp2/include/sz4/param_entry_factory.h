#ifndef __SZ4_PARAM_ENTRY_FACTORY_H__
#define __SZ4_PARAM_ENTRY_FACTORY_H__
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

#include "config.h"

#include "sz4/param_entry.h"

namespace sz4 {

struct param_entry_factory {

	template<
		template<typename DT, typename TT, class BT> class entry_type,
		typename data_type,
		typename time_type,
		typename types
	>
	generic_param_entry* create(base_templ<types>* base, TParam* param, const boost::filesystem::wpath &buffer_directory) {
		return new param_entry_in_buffer<entry_type, data_type, time_type, types>(base, param, buffer_directory);
	}
};

}

#endif
