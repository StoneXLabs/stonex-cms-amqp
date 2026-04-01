/*
 * Copyright 2022 - 2023 StoneX Financial Ltd.
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Properties.h"
#include <sstream>

AMQP_DEFINES

        int Properties::size() const { return mProperties.size(); };

        bool Properties::isEmpty() const { return mProperties.empty(); };

        const char* Properties::getProperty(const std::string& name) const
        { 
            if(hasProperty(name))
                return mProperties.at(name).c_str();
            else
                return nullptr;
        };

        std::string Properties::getProperty(const std::string& name, const std::string& defaultValue) const
        { 
            
            if(hasProperty(name))
                return mProperties.at(name);
            else
                return defaultValue;
        };

        void Properties::setProperty(const std::string& name, const std::string& value) { mProperties[name] = value; };

        bool Properties::hasProperty(const std::string& name) const { return mProperties.find(name) != mProperties.end(); };

        std::string Properties::remove(const std::string& name) { return mProperties.extract(name).mapped(); };

        std::vector<std::string> Properties::propertyNames() const
        {
            std::vector<std::string> names;
            for(const auto& [key, value] : mProperties)
                names.push_back(key);
            return names;
        };

        std::vector< std::pair< std::string, std::string > > Properties::toArray() const
        { 
            std::vector< std::pair< std::string, std::string > > properties; 
            for(const auto& [key, value] : mProperties)
                properties.emplace_back(key, value);
            return properties;
        };

        void Properties::copy(const cms::CMSProperties* source) 
        {
            mProperties.clear();
            if (source)
            {
                for (const auto& key : source->propertyNames())
                {
                    mProperties[key] = source->getProperty(key);
                }
            }
        };

        cms::CMSProperties* Properties::clone() const { return new Properties(*this); };

        void Properties::clear() { mProperties.clear();};

        std::string Properties::toString() const
        {
            std::ostringstream oss;
            oss << "{";
            for (auto it = mProperties.begin(); it != mProperties.end(); ++it) {
                oss << it->first << ": " << it->second;
                if (std::next(it) != mProperties.end())
                    oss << ", ";
            }
            oss << "}";
            return oss.str();
        };
AMQP_DEFINES_CLOSE