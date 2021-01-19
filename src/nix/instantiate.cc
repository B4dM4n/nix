#include "eval.hh"
#include "command.hh"
#include "common-args.hh"
#include "shared.hh"
#include "store-api.hh"
#include "local-fs-store.hh"

#include <nlohmann/json.hpp>

using namespace nix;

struct CmdInstantiate : InstallablesCommand, MixJSON
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

    void run(ref<Store> store) override
    {
        auto buildables = build(store, Realise::Derivation, installables, bmNormal);


        if (auto store2 = store.dynamic_pointer_cast<LocalFSStore>())
            for (const auto & [i, buildable] : enumerate(buildables)) {
                std::visit(overloaded {
                    [&](BuildableOpaque bo) {
                        throw Error("'%1%' is not a derivation", store2->printStorePath(bo.path));
                    },
                    [&](BuildableFromDrv bfd) {
                        if (!json) logger->cout(store2->printStorePath(bfd.drvPath));
                    },
                }, buildable);
            }
        else
            throw Error("can only run on local stores");

        if (json) logger->cout("%s", buildablesToJSON(buildables, store).dump());
    }
};

static auto rCmdInstantiate = registerCommand<CmdInstantiate>("instantiate");
