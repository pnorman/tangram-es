#pragma once

#include "data/filters.h"
#include "style/styleParamMap.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Tangram {

class Style;
struct Feature;

struct DrawRule {
    std::shared_ptr<Style> style;
    StyleParamMap parameters;
    DrawRule merge(DrawRule& _other) const;
    bool operator<(const DrawRule& _rhs) const;
};

class SceneLayer {

    Filter m_filter;
    std::string m_name;
    std::vector<DrawRule> m_rules;
    std::vector<SceneLayer> m_sublayers;
    size_t m_depth;

public:

    SceneLayer(std::string _name, Filter _filter,
        std::vector<DrawRule> _rules, std::vector<SceneLayer> _sublayers);

    const auto& name() const { return m_name; }
    const auto& filter() const { return m_filter; }
    const auto& rules() const { return m_rules; }
    const auto& sublayers() const { return m_sublayers; }

    // Recursively match and combine draw rules that apply to the given Feature in the given Context
    void match(const Feature& _feat, const Context& _ctx, std::vector<DrawRule>& _matches) const;

};

}

