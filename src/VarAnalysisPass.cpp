#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <math.h>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/Metadata.h"

using namespace llvm;

namespace
{
    class VarAnalysis : public ModulePass
    {
    public:
        static char ID;
        std::vector<llvm::StructType *> StructSet;

        void traverseMetadata(MDNode *meta_data);
        VarAnalysis() : ModulePass(ID)
        {
        }
        bool runOnModule(Module &M) override
        {
            StructSet = M.getIdentifiedStructTypes();
            for (std::vector<llvm::StructType *>::iterator sit = StructSet.begin(); sit != StructSet.end(); sit++)
            {
                errs() << "Name: " << (*sit)->getName() << "\n"
                       << *(*sit) << "\n";
            }

            for (llvm::Module::named_metadata_iterator nmdit = M.named_metadata_begin(); nmdit != M.named_metadata_end(); nmdit++)
            {
                for (llvm::NamedMDNode::op_iterator mdit = (*nmdit).op_begin(); mdit != (*nmdit).op_end(); mdit++)
                {
                    errs() << (*mdit)->getMetadataID() << "\n";
                    // traverseMetadata()
                }
            }

            return false;
        }
    };
}

char VarAnalysis::ID = 0;

// Register for opt
static RegisterPass<VarAnalysis> X("VarAnalysis", "Var analysis pass");

// Register for clang
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM)
                                {
                                    PM.add(new VarAnalysis());
                                });