"""
  SZARP: SCADA software 
  Darek Marcinkiewicz <reksio@newterm.pl>

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

"""

import lxml
import lxml.etree
import param

LOG_PARAM_COUNT=250

IPK_NAMESPACE='http://www.praterm.com.pl/SZARP/ipk'

class IPK:
	def __init__(self, ipk_path):
		ipk = lxml.etree.parse(ipk_path)

		self.params = [
			param.from_node(p) for p in ipk.xpath("//s:device//s:param", namespaces={'s': IPK_NAMESPACE })
		]
		self.device_params_count = len(self.params)

		self.params.extend(
			param.from_node(p) for p in ipk.xpath("//s:defined/s:param|s:drawdefinable/s:param", namespaces={'s': IPK_NAMESPACE })
		)

		param_map = {}
		for p in self.params:
			param_map[p.param_name] = p	

		for p in self.params:
			if not p.combined:
				continue

			lsw_param = param_map[p.lsw_param_name]
			msw_param = param_map[p.msw_param_name]

			p.lsw_param = lsw_param
			p.msw_param = msw_param

			lsw_param.lsw_combined_referencing_param = p
			msw_param.msw_combined_referencing_param = p

	def adjust_param_index(self, index):
		if index < self.device_params_count:
			return (False, index)
		elif index < self.device_params_count + LOG_PARAM_COUNT:
			return (True, None)
		else:
			return (False, index - LOG_PARAM_COUNT)

