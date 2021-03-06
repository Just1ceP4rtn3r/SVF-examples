#include "llvm/Pass.h"
#include "../include/VarAnalysis.h"

using namespace llvm;

namespace
{
    class VarAnalysisPass : public ModulePass
    {
    public:
        static char ID;

        VarAnalysisPass() : ModulePass(ID)
        {
        }
        bool runOnModule(Module &M) override
        {

            mqttactic::VarAnalysis *var_analyzer = new mqttactic::VarAnalysis(M);

            errs() << "----------------------------------\n";

            std::vector<mqttactic::KeyVariable *> key_variables(3);
            key_variables[0] = new mqttactic::KeyVariable();
            key_variables[0]->name = "mosquitto__subhier::subs";

            key_variables[1] = new mqttactic::KeyVariable();
            key_variables[1]->name = "mosquitto_msg_data::inflight";

            key_variables[2] = new mqttactic::KeyVariable();
            key_variables[2]->name = "mosquitto_msg_data::queued";

            for (auto key_var : key_variables)
            {
                std::set<mqttactic::SemanticKBB *>
                    bb_array;
                var_analyzer->SemanticKeyBasicBlocks.insert(std::pair<mqttactic::KeyVariable *, std::set<mqttactic::SemanticKBB *>>(key_var, bb_array));
            }
            for (Module::iterator mi = M.begin(); mi != M.end(); ++mi)
            {
                Function &f = *mi;
                var_analyzer->SearchKeyVar(M, f, key_variables);
            }

            for (auto sbb : var_analyzer->SemanticKeyBasicBlocks[key_variables[0]])
            {
                errs() << sbb->bb->getParent()->getName() << "\n"
                       << sbb->semantics << "\n";
                for (auto var : sbb->values)
                {
                    errs() << *var << "\n";
                }
                errs() << *(sbb->bb) << "\n\n";
            }

            return false;
        }
    };
}

char VarAnalysisPass::ID = 0;

// Register for opt
static RegisterPass<VarAnalysisPass> X("VarAnalysisPass", "Var analysis pass");

// Register for clang
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM)
                                {
                                    PM.add(new VarAnalysisPass());
                                });