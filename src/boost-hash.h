//TODO: review this bullshit

#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <utility>

struct PairHash{
    template<typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& value) const{
        const std::size_t h1 = std::hash<T1>{}(value.first);
        const std::size_t h2 = std::hash<T2>{}(value.second);
        return h1 ^(h2 + static_cast<std::size_t>(0x9e3779b9) + (h1 << 6) + (h1 >> 2));
    }
};

template<typename K, typename V, typename Hash = std::hash<K>>
class BoostHash{
private:
    using MapType = std::unordered_map<K, V, Hash>;

    MapType data;

public:
    using const_iterator = typename MapType::const_iterator;
    bool contains(const K& key) const{
        return data.find(key) != data.cend();
    }

    void insert(const K& key, const V& value){
        data.emplace(key, value);
    }

    void remove(const K& key){
        data.erase(key);
    }

    V value(const K& key) const{
        auto iter = data.find(key);

        if (iter == data.cend()) {
            return V{};
        }

        return iter->second;
    }

    V value(const K& key, const V& defaultValue) const{
        auto iter = data.find(key);

        if (iter == data.cend()) {
            return defaultValue;
        }

        return iter->second;
    }

    V& operator[](const K& key){
        return data[key];
    }

    const_iterator cbegin() const{
        return data.cbegin();
    }

    const_iterator cend() const{
        return data.cend();
    }
};


template<typename K>
class BoostSet{
private:
    using SetType = std::unordered_set<K>;

    SetType data;

public:
    using const_iterator = typename SetType::const_iterator;

    bool contains(const K& key) const{
        return data.find(key) != data.cend();
    }

    void insert(const K& key){
        data.insert(key);
    }

    void remove(const K& key){
        data.erase(key);
    }

    const_iterator cbegin() const{
        return data.cbegin();
    }

    const_iterator cend() const{
        return data.cend();
    }
};
