#include "nix/expr/eval.hh"
#include "nix/cmd/command.hh"
#include "nix/main/common-args.hh"
#include "nix/main/shared.hh"
#include "nix/store/store-api.hh"

#include <nlohmann/json.hpp>

using namespace nix;

static nlohmann::json toJSON(Store & store, const SingleDerivedPath::Opaque & o)
{
    return store.printStorePath(o.path);
}

static nlohmann::json toJSON(Store & store, const SingleDerivedPath & sdp)
{
    return std::visit([&](const auto & buildable) { return toJSON(store, buildable); }, sdp.raw());
}

static nlohmann::json
builtPathsWithResultToJSON(const std::vector<BuiltPathWithResult> & buildables, const Store & store)
{
    auto res = nlohmann::json::array();
    for (auto & b : buildables) {
        auto j = b.path.toJSON(store);
        res.push_back(j);
    }
    return res;
}

struct CmdInstantiate : virtual InstallablesCommand, virtual MixJSON
{
    CmdInstantiate() {}

    std::string description() override
    {
        return "instantiate a derivation print it's store path";
    }

    std::string doc() override
    {
        return
#include "instantiate.md"
            ;
    }

    void run(ref<Store> store, Installables && installables) override
    {
        auto buildables = Installable::build(getEvalStore(), store, Realise::Derivation, installables, bmNormal);

        if (json)
            logger->cout("%s", builtPathsWithResultToJSON(buildables, *store).dump());
        else {
            logger->stop();
            for (auto & buildable : buildables) {
                std::visit(
                    overloaded{
                        [&](const BuiltPath::Opaque & bo) {
                            throw Error("'%1%' is not a derivation", store->printStorePath(bo.path));
                        },
                        [&](const BuiltPath::Built & bfd) {
                            logger->cout(store->printStorePath(bfd.drvPath->outPath()));
                        },
                    },
                    buildable.path.raw());
            }
        }
    }
};

static auto rCmdInstantiate = registerCommand<CmdInstantiate>("instantiate");
