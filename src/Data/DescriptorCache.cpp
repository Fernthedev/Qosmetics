#include "Data/DescriptorCache.hpp"
#include "Utils/FileUtils.hpp"
#include "static-defines.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/prettywriter.h"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/stringbuffer.h"
#include <thread>
#include "Logging.hpp"

#define INFO(value...) QosmeticsLogger::GetContextLogger("Descriptor Cache").info(value)
#define ERROR(value...) QosmeticsLogger::GetContextLogger("Descriptor Cache").error(value)

namespace Qosmetics
{
    void DescriptorCache::Save()
    {
        std::thread SaveToFile([]{
            INFO("Saving Descriptor Cache...");
            rapidjson::Document d;
            d.SetObject();
            rapidjson::Document::AllocatorType& allocator = d.GetAllocator();

            d.AddMember("sabers", MapToValue(descriptors[ItemType::saber], allocator), allocator);
            d.AddMember("notes", MapToValue(descriptors[ItemType::note], allocator), allocator);
            d.AddMember("walls", MapToValue(descriptors[ItemType::wall], allocator), allocator);
            d.AddMember("pointers", MapToValue(descriptors[ItemType::pointer], allocator), allocator);
            d.AddMember("platforms", MapToValue(descriptors[ItemType::platform], allocator), allocator);

            Write(d);
            INFO("Saved Descriptor Cache!");
        });

        SaveToFile.detach();
    }

    void DescriptorCache::Write(rapidjson::Document& d)
    {
        // stringify document
        rapidjson::StringBuffer buffer;
        buffer.Clear();

        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);

        // make string out of buffer data
        std::string json(buffer.GetString(), buffer.GetSize());

        // write to file
        writefile(DESCRIPTORCACHE, json);
    }

    bool DescriptorCache::Load()
    {
        INFO("Loading Descriptor Cache...");
        if (!fileexists(DESCRIPTORCACHE)) return false;
        std::thread LoadFromFile([]{
            std::string json = readfile(DESCRIPTORCACHE);
            rapidjson::Document d;
            d.Parse(json.c_str());

            if (d.HasMember("sabers")) LoadDescriptorsIntoMap(d["sabers"], descriptors[ItemType::saber]);
            if (d.HasMember("walls")) LoadDescriptorsIntoMap(d["walls"], descriptors[ItemType::wall]);
            if (d.HasMember("notes")) LoadDescriptorsIntoMap(d["notes"], descriptors[ItemType::note]);
            if (d.HasMember("pointers")) LoadDescriptorsIntoMap(d["pointers"], descriptors[ItemType::pointer]);
            if (d.HasMember("platforms")) LoadDescriptorsIntoMap(d["platforms"], descriptors[ItemType::platform]);
            INFO("Loaded Descriptor Cache!");
        });
        LoadFromFile.detach();
        return true;
    }

    Descriptor& DescriptorCache::AddDescriptorToCache(Descriptor& descriptor)
    {
        return AddDescriptorToMap(descriptor, descriptors[descriptor.get_type()]);
        /*
        switch(descriptor.get_type())
        {
            case saber:
                return AddDescriptorToMap(descriptor, saberDescriptors);
            case note:
                return AddDescriptorToMap(descriptor, noteDescriptors);
            case wall:
                return AddDescriptorToMap(descriptor, wallDescriptors);
            case pointer:
                return AddDescriptorToMap(descriptor, pointerDescriptors);
            case platform:
                return AddDescriptorToMap(descriptor, platformDescriptors);
            default:
                return invalid;
        }
        */
    }

    Descriptor& DescriptorCache::AddDescriptorToMap(Descriptor& descriptor, Cache& map)
    {
        std::string fileName = descriptor.GetFileName();
        map[fileName] = descriptor;
        return GetDescriptorFromMap(fileName, map);
    }

    Descriptor& DescriptorCache::GetDescriptor(std::string fileName)
    {
        ItemType type = Descriptor::GetTypeFromName(fileName);
        if (type == ItemType::invalid) return invalid;
        return GetDescriptorFromMap(fileName, descriptors[type]);

        /*
        switch (type)
        {
            case saber:
                return GetDescriptorFromMap(fileName, saberDescriptors);
            case note:
                return GetDescriptorFromMap(fileName, noteDescriptors);
            case wall:
                return GetDescriptorFromMap(fileName, wallDescriptors);
            case pointer:
                return GetDescriptorFromMap(fileName, pointerDescriptors);
            case platform:
                return GetDescriptorFromMap(fileName, platformDescriptors);
            default:
                return invalid;
        }
        */
    }

    Descriptor& DescriptorCache::GetDescriptorFromMap(std::string& fileName, Cache& map)
    {
        std::string key = FileUtils::GetFileName(fileName);
        Cache::iterator it = map.find(key);
        if (it != map.end())
        {
            return it->second;
        }
        return invalid;
    }

    void DescriptorCache::LoadDescriptorsIntoMap(rapidjson::Value& val, Cache& out)
    {
        for (auto& descriptor : val.GetArray())
        {
            Descriptor desc = Descriptor(descriptor);
            out[desc.GetFileName()] = desc;
        }
    }

    rapidjson::Value DescriptorCache::MapToValue(Cache& map, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value array(rapidjson::kArrayType);
        array.SetArray();

        for (auto& pair : map)
        {
            array.PushBack(pair.second.ToVal(allocator), allocator);
        }

        return array;
    }
}