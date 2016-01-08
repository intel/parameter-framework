/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "TmpFile.hpp"
#include "Config.hpp"

#include <string>
#include <map>

namespace parameterFramework
{

/** Create temporary config files on disk. */
class ConfigFiles
{
public:
    ConfigFiles(const Config &config)
        : mStructureFile(
              format(mStructureTemplate, {{"type", config.subsystemType},
                                          {"instances", config.instances},
                                          {"components", config.components},
                                          {"subsystemMapping", config.subsystemMapping}})),
          mDomainsFile(format(mDomainsTemplate, {{"domains", config.domains}})),
          mConfigFile(format(mConfigTemplate, {{"structurePath", mStructureFile.getPath()},
                                               {"domainsPath", mDomainsFile.getPath()},
                                               {"plugins", toXml(config.plugins)}}))
    {
    }

    std::string getPath() { return mConfigFile.getPath(); }

private:
    std::string toXml(const Config::Plugin::Collection &plugins)
    {
        std::string pluginsXml;
        for (auto &pluginLocation : plugins) {
            std::string pluginsLocationXml;
            auto location = pluginLocation.first;
            auto paths = pluginLocation.second;
            for (auto &path : paths) {
                pluginsLocationXml += "<Plugin Name='" + path + "'/>\n";
            }
            pluginsXml +=
                "<Location Folder='" + location + "'>\n" + pluginsLocationXml + "\n</Location>\n";
        }
        return pluginsXml;
    }

    std::string format(std::string format, std::map<std::string, std::string> subs)
    {
        for (auto &sub : subs) {
            replace(format, '{' + sub.first + '}', sub.second);
        }
        return format;
    }

    void replace(std::string &on, const std::string &from, const std::string &to)
    {
        auto from_pos = on.find(from);
        if (from_pos != std::string::npos) {
            on.replace(from_pos, from.length(), to);
        }
    }

    const char *mConfigTemplate = R"(<?xml version='1.0' encoding='UTF-8'?>
        <ParameterFrameworkConfiguration SystemClassName='test' TuningAllowed='true'>
            <SubsystemPlugins>
                {plugins}
            </SubsystemPlugins>
            <StructureDescriptionFileLocation Path='{structurePath}'/>
            <SettingsConfiguration>
                <ConfigurableDomainsFileLocation Path='{domainsPath}'/>
            </SettingsConfiguration>
        </ParameterFrameworkConfiguration>
     )";
    const char *mStructureTemplate = R"(<?xml version='1.0' encoding='UTF-8'?>
        <SystemClass Name='test'>
            <Subsystem Name='test' Type='{type}' Mapping='{subsystemMapping}'>
                <ComponentLibrary>
                    {components}
                </ComponentLibrary>
                <InstanceDefinition>
                    {instances}
                </InstanceDefinition>
            </Subsystem>
        </SystemClass>
    )";
    const char *mDomainsTemplate = R"(<?xml version='1.0' encoding='UTF-8'?>
         <ConfigurableDomains SystemClassName="test">
             {domains}
         </ConfigurableDomains>
    )";

    utility::TmpFile mStructureFile;
    utility::TmpFile mDomainsFile;
    utility::TmpFile mConfigFile;
};

} // parameterFramework
