#include "command.hh"
#include "shared.hh"
#include "store-api.hh"
#include "common-args.hh"

using namespace nix;

struct CmdCopyRaw : virtual CopyCommand, virtual InstallablesCommand
{
    CheckSigsFlag checkSigs = CheckSigs;

    SubstituteFlag substitute = NoSubstitute;

    CmdCopyRaw()
    {
        addFlag({
            .longName = "no-check-sigs",
            .description = "Do not require that paths are signed by trusted keys.",
            .handler = {&checkSigs, NoCheckSigs},
        });

        addFlag({
            .longName = "substitute-on-destination",
            .shortName = 's',
            .description = "Whether to try substitutes on the destination store (only supported by SSH stores).",
            .handler = {&substitute, Substitute},
        });
    }

    std::string description() override
    {
        return "copy paths between Nix stores without trying build anything";
    }

    std::string doc() override
    {
        return
          #include "copy-raw.md"
          ;
    }

    Category category() override { return catSecondary; }

    void run(ref<Store> srcStore, Installables && installables) override
    {
        settings.readOnlyMode = true;

        auto dstStore = getDstStore();

        BuiltPaths builtPaths;
        for (const auto & i : installables) {
            if (auto storePath = i->getStorePath())
                builtPaths.push_back(BuiltPath::Opaque{*storePath});
            else
                throw Error("only store paths are supported");
        }

        StorePathSet pathsRoots, pathsClosure;
        for (auto & root : builtPaths) {
            auto rootFromThis = root.outPaths();
            pathsRoots.insert(rootFromThis.begin(), rootFromThis.end());
        }
        srcStore->computeFSClosure(pathsRoots, pathsClosure);
        for (auto & path : pathsClosure)
            builtPaths.push_back(BuiltPath::Opaque{path});

        RealisedPath::Set stuffToCopy;
        for (auto & builtPath : builtPaths) {
            auto theseRealisations = builtPath.toRealisedPaths(*srcStore);
            stuffToCopy.insert(theseRealisations.begin(), theseRealisations.end());
        }

        copyPaths(*srcStore, *dstStore, stuffToCopy, NoRepair, checkSigs, substitute);
    }
};

static auto rCmdCopyRaw = registerCommand<CmdCopyRaw>("copy-raw");
