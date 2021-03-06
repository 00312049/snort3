//--------------------------------------------------------------------------
// Copyright (C) 2016-2016 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------
// pps_dcerpc.cc author Maya Dagon <mdagon@cisco.com>

#include "pps_dcerpc_server.h"

namespace preprocessors
{
namespace dce
{
class Dcerpc : public ConversionState
{
public:
    Dcerpc(Converter& c) : ConversionState(c) { }
    virtual ~Dcerpc() { }
    virtual bool convert(std::istringstream& data);

private:
    bool add_deleted_comment_to_table(std::string table_name, std::string option);
    bool add_deleted_comment_to_defaults(std::string option);
    bool add_option_to_all(std::string option, const bool val);
    bool add_option_to_all(std::string option, const int val);
    bool add_option_to_type(std::string type, std::string option, std::string value);
    bool parse_int_and_add_to_all(std::string opt_name, std::istringstream& stream);
    bool parse_string_and_add_to_type(std::string type, std::string opt_name,
        std::istringstream& stream);
};

bool Dcerpc::add_deleted_comment_to_table(std::string table_name, std::string option)
{
    table_api.open_table(table_name);
    bool tmpval = table_api.add_deleted_comment(option);
    table_api.close_table();

    return tmpval;
}

bool Dcerpc::add_option_to_all(std::string option, const bool val)
{
    bool tmpval = true;

    for (auto type : transport)
    {
        tmpval = add_option_to_table(table_api, "dce_" + type, option, val);
        for (int i=0; i < DcerpcServer::get_binding_id(); i++)
        {
            tmpval = add_option_to_table(table_api, "dce_" + type + std::to_string(i), option,
                val) && tmpval;
        }
    }
    return tmpval;
}

bool Dcerpc::add_option_to_all(std::string option, const int val)
{
    bool tmpval = true;

    for (auto type : transport)
    {
        tmpval = add_option_to_table(table_api, "dce_" + type, option, val);
        for (int i=0; i < DcerpcServer::get_binding_id(); i++)
        {
            tmpval = add_option_to_table(table_api, "dce_" + type + std::to_string(i), option,
                val) && tmpval;
        }
    }
    return tmpval;
}

bool Dcerpc::add_option_to_type(std::string type, std::string option, const std::string val)
{
    bool tmpval = true;

    tmpval = add_option_to_table(table_api, "dce_" + type, option, val);
    for (int i=0; i < DcerpcServer::get_binding_id(); i++)
    {
        tmpval = add_option_to_table(table_api, "dce_" + type + std::to_string(i), option, val) &&
            tmpval;
    }

    return tmpval;
}

bool Dcerpc::add_deleted_comment_to_defaults(std::string option)
{
    bool tmpval = true;

    for (auto type : transport)
    {
        tmpval = add_deleted_comment_to_table("dce_" + type, option) && tmpval;
    }
    return tmpval;
}

bool Dcerpc::parse_int_and_add_to_all(std::string opt_name, std::istringstream& stream)
{
    int val;

    if (stream >> val)
    {
        return add_option_to_all(opt_name, val);
    }

    return false;
}

bool Dcerpc::parse_string_and_add_to_type(std::string type, std::string opt_name,
    std::istringstream& stream)
{
    std::string val;

    if (stream >> val)
    {
        if (val.back() == ',')
        {
            val.pop_back();
        }
        return add_option_to_type(type, opt_name, val);
    }

    return false;
}

bool Dcerpc::convert(std::istringstream& data_stream)
{
    std::string keyword;
    bool retval = true;

    while (data_stream >> keyword)
    {
        bool tmpval = true;

        if (keyword.back() == ',')
            keyword.pop_back();

        if (keyword.empty())
            continue;

        if (!keyword.compare("memcap"))
        {
            add_deleted_comment_to_defaults("memcap");
            tmpval = eat_option(data_stream);
        }
        else if (!keyword.compare("disable_defrag"))
            tmpval = add_option_to_all("disable_defrag", true);

        else if (!keyword.compare("max_frag_len"))
            tmpval = parse_int_and_add_to_all("max_frag_len", data_stream);

        else if (!keyword.compare("events"))
        {
            std::string events;

            tmpval = add_deleted_comment_to_defaults("events");

            //Skip events - can be either a single arg or [ list ]
            if (!(data_stream >> events))
            {
                data_api.failed_conversion(data_stream, "events");
                retval = false;
                continue;
            }

            if ((events.find('[') != std::string::npos) &&  (events.find(']') ==
                std::string::npos))
            {
                do
                {
                    if (!(data_stream >> events))
                        return false;
                }
                while (events.find(']') == std::string::npos);
            }
        }
        else if (!keyword.compare("reassemble_threshold"))
            tmpval = parse_int_and_add_to_all("reassemble_threshold", data_stream);

        else if (!keyword.compare("disabled"))
            tmpval = add_deleted_comment_to_defaults("disabled");

        else if (!keyword.compare("smb_fingerprint_policy"))
            tmpval = parse_string_and_add_to_type("smb", "smb_fingerprint_policy", data_stream);

        else if (!keyword.compare("smb_legacy_mode"))
            tmpval = add_deleted_comment_to_table("dce_smb", "smb_legacy_mode");
        else
        {
            tmpval = false;
        }

        if (!tmpval)
        {
            data_api.failed_conversion(data_stream, keyword);
            retval = false;
        }
    }

    return retval;
}
} // namespace dce

/**************************
 *******  A P I ***********
 **************************/

static ConversionState* ctor(Converter& c)
{
    return new dce::Dcerpc(c);
}

static const ConvertMap preprocessor_dcerpc =
{
    "dcerpc2",
    ctor,
};

const ConvertMap* dcerpc_map = &preprocessor_dcerpc;
} // namespace preprocessors

