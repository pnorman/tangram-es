#pragma once

#include "util/variant.h"

#include <vector>
#include <memory>

namespace Tangram {

class SceneLayer;
class StyleContext;
struct Feature;
struct Filter;

// Reduce Filters for current zoom-level
// Produce 3 filter sets for each geometry type

enum class FilterKeyword : uint8_t {
    zoom,
    geometry,
    undefined,
};

struct FiltersAndKeys {
    std::vector<Filter*> filters;
    std::vector<std::string> keys;
};

struct Filter {
    struct OperatorAll {
        std::vector<Filter> operands;
    };
    struct OperatorAny {
        std::vector<Filter> operands;
    };
    struct OperatorNone {
        std::vector<Filter> operands;
    };

    struct EqualitySet {
        size_t keyID;
        std::vector<Value> values;
        std::string key;
    };
    struct Equality {
        size_t keyID;
        Value value;
        std::string key;
    };
    struct Range {
        size_t keyID;
        float min;
        float max;
        std::string key;
    };
    struct Existence {
        size_t keyID;
        bool exists;
        std::string key;
    };
    struct Function {
        uint32_t id;
    };
    using Data = variant<none_type,
                         OperatorAll,
                         OperatorNone,
                         OperatorAny,
                         EqualitySet,
                         Equality,
                         Range,
                         Existence,
                         Function>;
    Data data;

    Filter() : data(none_type{}) {}
    Filter(Data _data) : data(std::move(_data)) {}

    bool eval(StyleContext& ctx) const;

    // Create an 'any', 'all', or 'none' filter
    inline static Filter MatchAny(std::vector<Filter> filters) {
        sort(filters);
        return { OperatorAny{ std::move(filters) }};
    }
    inline static Filter MatchAll(std::vector<Filter> filters) {
        sort(filters);
        return { OperatorAll{ std::move(filters) }};
    }
    inline static Filter MatchNone(std::vector<Filter> filters) {
        sort(filters);
        return { OperatorNone{ std::move(filters) }};
    }
    // Create an 'equality' filter
    inline static Filter MatchEquality(const std::string& k, const std::vector<Value>& vals) {
        auto t = keywordType(k);

        if (t == FilterKeyword::geometry) {
            if (vals.size() == 1) {
                return { Equality{ 0, vals[0], k }};
            } else {
                return { EqualitySet{ 0, vals, k }};
            }

        } else {
            if (vals.size() == 1) {
                return { Equality{ 0, vals[0], k }};
            } else {
                return { EqualitySet{ 0, vals, k }};
            }
        }
    }
    // Create a 'range' filter
    inline static Filter MatchRange(const std::string& k, float min, float max) {
        return { Range{ 0, min, max, k }};
    }
    // Create an 'existence' filter
    inline static Filter MatchExistence(const std::string& k, bool ex) {
        return { Existence{ 0, ex, k }};
    }
    // Create an 'function' filter with reference to Scene function id
    inline static Filter MatchFunction(uint32_t id) {
        return { Function{ id }};
    }

    static FilterKeyword keywordType(const std::string& _key) {
        if (_key == "$geometry") {
            return FilterKeyword::geometry;
        } else if (_key == "$zoom") {
            return  FilterKeyword::zoom;
        }
        return  FilterKeyword::undefined;
    }

    /* Public for testing */
    static void sort(std::vector<Filter>& filters);

    static void collectFilters(Filter& f, FiltersAndKeys& fk);
    static std::vector<std::string> assignPropertyKeys(FiltersAndKeys& fk);

    void print(int _indent = 0) const;
    int filterCost() const;
    const bool isOperator() const;
    const std::string& key() const;
    const std::vector<Filter>& operands() const;

};
}
