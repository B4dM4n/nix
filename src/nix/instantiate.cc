#include "nix/expr/eval.hh"
#include "nix/cmd/command.hh"
#include "nix/main/common-args.hh"
#include "nix/main/shared.hh"
#include "nix/store/store-api.hh"
#include "nix/store/local-fs-store.hh"

#include <nlohmann/json.hpp>

using namespace nix;

nlohmann::json builtPathsToJSON(const std::vector<BuiltPathWithResult> & buildables, Store & store)
{
    auto res = nlohmann::json::array();
    for (auto & b : buildables) {
        std::visit([&](const auto & t) {
            res.push_back(t.toJSON(store));
        }, b.path.raw());
    }
    return res;
}

struct CmdInstantiate : virtual InstallablesCommand, virtual MixJSON
{
    CmdInstantiate()
    {
    }

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

        if (auto store2 = store.dynamic_pointer_cast<LocalFSStore>())
            for (const auto &buildable : buildables) {
                std::visit(overloaded {
                    [&](BuiltPath::Opaque bo) {
                        throw Error("'%1%' is not a derivation", store2->printStorePath(bo.path));
                    },
                    [&](BuiltPath::Built bfd) {
                        if (!json) logger->cout(store2->printStorePath(bfd.drvPath->outPath()));
                    },
                }, buildable.path.raw());
            }
        else
            throw Error("can only run on local stores");

        if (json) logger->cout("%s", builtPathsToJSON(buildables, *store).dump());
    }
};

static auto rCmdInstantiate = registerCommand<CmdInstantiate>("instantiate");
