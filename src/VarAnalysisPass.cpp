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
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/DebugInfo.h"

using namespace llvm;

namespace
{
    class VarAnalysis : public ModulePass
    {
    public:
        static char ID;
        std::vector<llvm::StructType *> StructSet;

        void traverseMetadata(MDNode *MD);
        VarAnalysis() : ModulePass(ID)
        {
        }
        bool runOnModule(Module &M) override
        {
            // StructSet = M.getIdentifiedStructTypes();
            // for (std::vector<llvm::StructType *>::iterator sit = StructSet.begin(); sit != StructSet.end(); sit++)
            // {
            //     errs() << "Name: " << (*sit)->getName() << "\n"
            //            << *(*sit) << "\n";
            // }

            DebugInfoFinder *dbgFinder = new DebugInfoFinder();
            dbgFinder->processModule(M);
            for (const DIType *T : dbgFinder->types())
            {
                if (!T->getName().empty())
                {
                    errs() << "Type:";
                    errs() << ' ' << T->getName() << " ";

                    switch (T->getMetadataID())
                    {
                    case DIBasicTypeKind:
                    {
                        auto *BT = dyn_cast<DIBasicType>(T);
                        auto Encoding = dwarf::AttributeEncodingString(BT->getEncoding());
                        if (!Encoding.empty())
                            errs() << Encoding;
                        else
                            errs() << "unknown-encoding(" << BT->getEncoding() << ')';
                        break;
                    }
                    case DIDerivedTypeKind:
                    {
                        auto Tag = dwarf::TagString(T->getTag());
                        if (!Tag.empty())
                            errs() << Tag;
                        else
                            errs() << "unknown-tag(" << T->getTag() << ")";
                        break;
                    }
                    case DICompositeTypeKind:
                    {
                        auto *CT = dyn_cast<DICompositeType>(T);
                        auto Tag = dwarf::TagString(T->getTag());
                        if (!Tag.empty())
                            errs() << Tag;
                        else
                            errs() << "unknown-tag(" << CT->getTag() << ")";
                        switch (CT->getTag())
                        {
                        case dwarf::DW_TAG_structure_type:
                        {
                            for (auto *field : CT->getElements())
                            {
                                if (auto *DerivedT = dyn_cast<DIDerivedType>(field))
                                {
                                    errs() << "    ";
                                    errs() << DerivedT->getName() << "\n";
                                }
                            }
                            break;
                        }
                        case dwarf::DW_TAG_class_type:
                        {
                            break;
                        }
                        case dwarf::DW_TAG_union_type:
                        {
                            break;
                        }
                        case dwarf::DW_TAG_enumeration_type:
                        {
                            break;
                        }
                        default:
                            break;
                        }
                        break;
                    }
                    }
                    errs() << '\n';
                }
            }

            // for (llvm::Module::named_metadata_iterator nmdit = M.named_metadata_begin(); nmdit != M.named_metadata_end(); nmdit++)
            // {
            //     for (llvm::NamedMDNode::op_iterator mdit = (*nmdit).op_begin(); mdit != (*nmdit).op_end(); mdit++)
            //     {
            //         traverseMetadata((*mdit));
            //     }
            // }

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