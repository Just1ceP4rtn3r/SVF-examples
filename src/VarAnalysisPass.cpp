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

            for (auto key_var : var_analyzer->key_variables)
            {
                std::set<mqttactic::SemanticKBB *>
                    bb_array;
                var_analyzer->SemanticKeyBasicBlocks.insert(std::pair<mqttactic::KeyVariable *, std::set<mqttactic::SemanticKBB *>>(key_var, bb_array));
            }
            for (Module::iterator mi = M.begin(); mi != M.end(); ++mi)
            {
                Function &f = *mi;
                var_analyzer->SearchKeyVar(M, f);
            }

            for (auto key_var : var_analyzer->key_variables)
            {
                errs() << "-----------------KEYVAR-----------------\n"
                       << key_var->name << "\n\n\n\n";
                for (auto sbb : var_analyzer->SemanticKeyBasicBlocks[key_var])
                {
                    errs() << sbb->bb->getParent()->getName() << "\n"
                           << sbb->semantics << "\n";
                    // for (auto var : sbb->values)
                    // {
                    //     errs() << *var << "\n";
                    // }
                    for (auto kbb_c : sbb->contexts)
                    {
                        for (auto bb : kbb_c)
                        {
                            errs() << bb->getParent()->getName() << ":" << var_analyzer->getBBLabel(bb) << " --> ";
                        }
                        errs() << "\n";
                    }
                    errs() << "----------------------------------\n";

                    errs() << *(sbb->bb) << "\n\n";
                }
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