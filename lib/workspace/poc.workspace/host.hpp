#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <poc.core/types.hpp>
#include <poc.workspace/extension.hpp>
#include <poc.workspace/ingester.hpp>
#include <poc.workspace/selection.hpp>
#include <poc.workspace/store.hpp>

namespace poc::workspace {

class host
{
public:
private:
    using extensions = std::vector<std::unique_ptr<extension>>;
    using extension_by_ids = std::unordered_map<string, extension*>;

    extensions _extensions;

    store _store;
    ingester _ingester;
    selection _selection;
};

} // namespace poc::workspace
