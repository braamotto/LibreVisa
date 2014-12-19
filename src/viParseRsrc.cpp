/* 
 * Copyright (C) 2014 Simon Richter, Christian Lindner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"
#include <string>
#include <vector>
#include <boost/regex.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/algorithm/string/regex.hpp>
using namespace std;

/// Parse a resource string to get the interface information.
///
/// This operation parses a resource string to verify its validity.  It should succeed for all strings returned by 
/// viFindRsrc() and recognized by viOpen().  This operation is useful if you want to know what interface 
/// a given resource descriptor would use without actually opening a session to it.
///
/// The values returned in intfType and intfNum correspond to the attributes VI_ATTR_INTF_TYPE and 
/// VI_ATTR_INTF_NUM. These values would be the same if a user opened that resource with viOpen() and 
/// queried the attributes with viGetAttribute().
/// 
/// @param sesn [in] Resource Manager session (should always be the Default Resource Manager for VISA returned from viOpenDefaultRM()).
/// @param rsrcName [in] Unique symbolic name of a resource.
/// @param intfType [out] Interface type of the given resource string.
/// @param intfNum [out] Board number of the interface of the given resource string.
/// @return This is the operational return status. It returns either a completion code or an error code as follows.
/// @retval VI_SUCCESS Resource string is valid.
/// @retval VI_ERROR_INV_SESSION The given session or object reference is invalid (both are the same value).
/// @retval VI_ERROR_INV_OBJECT The given session or object reference is invalid (both are the same value).
/// @retval VI_ERROR_NSUP_OPER The given sesn does not support this operation. For VISA, this operation is supported only by the Default Resource Manager session.
/// @retval VI_ERROR_INV_RSRC_NAME Invalid resource reference specified. Parsing error.
/// @retval VI_ERROR_RSRC_NFOUND Insufficient location information or resource not present in the system.
/// @retval VI_ERROR_ALLOC Insufficient system resources to parse the string.
/// @retval VI_ERROR_LIBRARY_NFOUND A code library required by VISA could not be located or loaded.
/// @retval VI_ERROR_INTF_NUM_NCONFIG The interface type is valid but the specified interface number is not configured.
ViStatus _VI_FUNC viParseRsrc(ViSession /*rmSesn*/, ViRsrc rsrcName, ViPUInt16 intfType, ViPUInt16 intfNum)
{
  boost::cmatch match;

  // USB INSTR Resource:
  // USB[board]::manufacturer ID::model code::serial number[::USB interface number][::INSTR]
  // "USB5::0x03eb::0x2423::123456"
  boost::regex regexUSB("USB(\\d*)::0x([0-9a-fA-F]{4})::0x([0-9a-fA-F]{4})::([\\d\\-.a-zA-Z]+)(|::\\d+)(|::INSTR)");
  if(regex_match(rsrcName, match, regexUSB))
  {
    *intfType = VI_INTF_USB;
    std::stringstream ssInterfaceNum(match[1]);
    std::stringstream ssUsbVendorId(match[2]);
    std::stringstream ssUsbDeviceId(match[3]);
    std::stringstream ssUsbSerial(match[4]);
    ssInterfaceNum >> *intfNum;
    return VI_SUCCESS;
  }

  return VI_ERROR_INV_RSRC_NAME;
}

/// Parse a resource string to get extended interface information.
/// 
/// This operation parses a resource string to verify its validity.  It should succeed for all strings returned by 
/// viFindRsrc() and recognized by viOpen().  This operation is useful if you want to know what interface a given 
/// resource descriptor would use without actually opening a session to it.
/// 
/// The values returned in intfType, intfNum, and rsrcClass correspond to the attributes VI_ATTR_INTF_TYPE, VI_ATTR_INTF_NUM, 
/// and VI_ATTR_RSRC_CLASS. These values would be the same if a user opened that resource with viOpen() and queried the 
/// attributes with viGetAttribute().
///
/// The value returned in unaliasedExpandedRsrcName should in most cases be identical to the VISA-defined canonical 
/// resource name.  However, there may be cases where the canonical name includes information that the driver may not 
/// know until the resource has actually been opened.  In these cases, the value returned in this parameter must be 
/// semantically similar.
/// 
/// The value returned in aliasIfExists allows programmatic access to user-defined aliases.  If a VISA implementation does 
/// not implement aliases, the return value must be an empty string.  If a VISA implementation allows multiple aliases for 
/// a single resource, then the implementation must pick one alias (in an implementation-defined manner) and return it in 
/// this parameter.
///
/// @param sesn [in] Resource Manager session (should always be the Default Resource Manager for VISA returned from viOpenDefaultRM()).
/// @param rsrcName [in] Unique symbolic name of a resource.
/// @param intfType [out] Interface type of the given resource string.
/// @param intfNum [out] Board number of the interface of the given resource string.
/// @param rsrcClass [out] Specifies the resource class (for example, “INSTR”) of the given resource string, as defined in Section 5.
/// @param expandedUnaliasedName [out] This is the expanded version of the given resource string.  The format should be similar to the VISA-defined canonical resource name.
/// @param aliasIfExists [out] Specifies the user-defined alias for the given resource string, if a VISA implementation allows aliases and an alias exists for the given resource string. 
/// @return This is the operational return status. It returns either a completion code or an error code as follows.
/// @retval VI_SUCCESS Resource string is valid.
/// @retval VI_WARN_EXT_FUNC_NIMPL The operation succeeded, but a lower level driver did not implement the extended functionality.
/// @retval VI_ERROR_INV_SESSION The given session or object reference is invalid (both are the same value).
/// @retval VI_ERROR_INV_OBJECT The given session or object reference is invalid (both are the same value).
/// @retval VI_ERROR_NSUP_OPER The given sesn does not support this operation. For VISA, this operation is supported only by the Default Resource Manager session.
/// @retval VI_ERROR_INV_RSRC_NAME Invalid resource reference specified. Parsing error.
/// @retval VI_ERROR_RSRC_NFOUND Insufficient location information or resource not present in the system.
/// @retval VI_ERROR_ALLOC Insufficient system resources to parse the string.
/// @retval VI_ERROR_LIBRARY_NFOUND A code library required by VISA could not be located or loaded.
/// @retval VI_ERROR_INTF_NUM_NCONFIG The interface type is valid but the specified interface number is not configured.
ViStatus _VI_FUNC viParseRsrcEx(ViSession rmSesn, ViRsrc rsrcName, ViPUInt16 intfType, ViPUInt16 intfNum, 
				ViChar _VI_FAR rsrcClass[],
                                ViChar _VI_FAR expandedUnaliasedName[],
                                ViChar _VI_FAR aliasIfExists[])
{
  ViStatus ret = viParseRsrc(rmSesn, rsrcName, intfType, intfNum);
  if(rsrcClass) strcpy(rsrcClass, "INSTR");
  if(expandedUnaliasedName) strcpy(expandedUnaliasedName, rsrcName);
  if(aliasIfExists) aliasIfExists[0] = 0;
  return ret;
}

